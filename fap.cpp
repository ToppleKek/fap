#include "fap.h"

Fap::Fap(QMainWindow *parent) : QMainWindow(parent), settings("ToppleKek", "Fap"), APPLICATION_ID("492434528644759564") {
    ui.setupUi(this);

    QPixmap unknownCover;
    unknownCover.load(":/images/unknown");

    ui.coverLabel->setPixmap(unknownCover.scaled(ui.coverLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    ui.songTree->setColumnWidth(0, 300);
    ui.songTree->setColumnWidth(1, 200);
    ui.songTree->setColumnWidth(2, 200);

    ui.queueList->setContextMenuPolicy(Qt::CustomContextMenu);

    initDiscord();

    curl_global_init(CURL_GLOBAL_DEFAULT);

    if (settings.value("mpd/host").isNull() || settings.value("mpd/port").isNull()) {
        DiscordRichPresence presence;
        memset(&presence, 0, sizeof(presence));

        presence.details = "Configuring MPD Server";
        presence.state = "FAP - Fantastic Audio Player";
        presence.startTimestamp = std::time(nullptr);

        Discord_UpdatePresence(&presence);
        setNewHost();
    }

    bool ok;
    unsigned port = settings.value("mpd/port").toUInt(&ok);

    conn = mpd_connection_new(settings.value("mpd/host").toByteArray().data(), ok ? port : 0, 300000);

    while (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        fprintf(stderr, "ERROR: Failed to connect to the MPD server: %d\n", mpd_connection_get_error(conn));

        DiscordRichPresence presence;
        memset(&presence, 0, sizeof(presence));

        presence.details = "Reconfiguring MPD Server";
        presence.state = "Failed to connect";
        presence.startTimestamp = std::time(nullptr);

        Discord_UpdatePresence(&presence);

        if (setNewHost() == QDialog::Rejected) {
            mpd_connection_free(conn);
            Discord_Shutdown();
            exit(0);
        }

        bool ok;
        unsigned port = settings.value("mpd/port").toUInt(&ok);

        conn = mpd_connection_new(settings.value("mpd/host").toByteArray().data(), ok ? port : 0, 300000);
    }

    testMpd = new Player(conn);

    printf("MPD Connection established with host: %s port: %d\n", settings.value("mpd/host").toByteArray().data(), port);

    testMpd->update();

    QVector<Player::FapSong> songs = testMpd->getSongs();
    QVector<Player::FapSong> queue = testMpd->getQueueSongs();
    
    updateStatus();

    ui.songTree->hideColumn(3);
    //ui.queueTree->hideColumn(3);

    for (int i = 0; i < songs.size(); i++)
        ui.songTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << songs.at(i).title << songs.at(i).artist << songs.at(i).album << songs.at(i).path, 1));

    for (int i = 0; i < queue.size(); i++) {
        Player::FapSong currentSong = testMpd->getCurrentSong();
        ui.queueList->addItem((currentSong.path == queue.at(i).path ? "-> " : "") + queue.at(i).title);
    }
        //ui.queueTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << queue.at(i).title << queue.at(i).artist << queue.at(i).album << queue.at(i).path, 1));

    connect(testMpd, &Player::mpdEvent, this, &Fap::handleEvents);

    QTimer *eventTimer = new QTimer(this);
    connect(eventTimer, &QTimer::timeout, testMpd, &Player::pollEvents);
    eventTimer->start(500);

    testMpd->getMusicDir(&settings);
}

Fap::~Fap() {
    mpd_connection_free(conn);
    Discord_Shutdown();
    curl_global_cleanup();
}

int Fap::setNewHost() {
    MPDConfDialog *d = new MPDConfDialog();

    if (d->exec() == QDialog::Accepted) {
        settings.setValue("mpd/host", d->getHost().toUtf8().data());
        settings.setValue("mpd/port", d->getPort());

        return QDialog::Accepted;
    } else
        return QDialog::Rejected;
}

QString Fap::secToMMSS(int time) {
    time %= 3600;
    int m = (int) (time / 60);

    QString minutes = QString::number(m);

    time %= 60;
    int s = time;

    QString seconds = QString::number(s);

    return QString(((m < 10) ? ("0" + minutes) : minutes) + ":" + ((s < 10) ? ("0" + seconds) : seconds));
}

void Fap::initDiscord() {
    DiscordEventHandlers handlers;

    memset(&handlers, 0, sizeof(handlers));

    // handlers.ready = &FapDRPC::ready;
    // handlers.disconnected = &FapDRPC::disconnected;
    // handlers.errored = &FapDRPC::errored;
    qDebug("init discord");
    Discord_Initialize("492434528644759564", &handlers, 1, NULL);
}

void Fap::updateDiscordPresence(QPixmap cover, bool hasCover) {
    qDebug("update presence");
    int status = testMpd->getStatus();
    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(presence));

    if (status == 0)
        return;

    if (status != MPD_STATE_STOP) {
        qDebug("not stopped");
        Player::FapSong fSong = testMpd->getCurrentSong();

        fSong.title.truncate(128);
        fSong.artist.truncate(128);
        fSong.album.truncate(128);

        if (fSong.title.length() < 2)
            fSong.title += "​";

        if (fSong.artist.length() < 2)
            fSong.artist += "​";

        if (fSong.album.length() < 2)
            fSong.album += "​";

        QByteArray titleArr = fSong.title.toUtf8();
        QByteArray artistArr = fSong.artist.toUtf8();
        QByteArray albumArr = fSong.album.toUtf8();

        const char *title = titleArr.data();
        const char *album = albumArr.data();
        const char *artist = artistArr.data();

        presence.details = title;
        presence.state = artist;

        if (settings.contains("assets/" + fSong.album) || settings.contains("assets/" + fSong.title))
            presence.largeImageKey = settings.value("assets/" + (fSong.album == "Unknown" ? fSong.title : fSong.album)).toString().toStdString().c_str();
        else if (hasCover) {
            QBuffer b;
            b.open(QIODevice::WriteOnly);
            cover.save(&b, "PNG");
            auto encoded = b.data().toBase64();

            dAppUploadAsset(APPLICATION_ID, encoded.data(), fSong.album == "Unknown" ? fSong.title : fSong.album, &settings);

            presence.largeImageKey = settings.value("assets/" + fSong.album).toString().toStdString().c_str();
        } else
            presence.largeImageKey = "unknown";

        presence.largeImageText = album;
        presence.smallImageKey = (status == MPD_STATE_PLAY ? "play" : "pause");
        presence.smallImageText = (status == MPD_STATE_PLAY ? "Playing" : "Paused");
        presence.startTimestamp = (status == MPD_STATE_PLAY ? std::time(nullptr) : NULL);
        presence.endTimestamp = (status == MPD_STATE_PLAY ? std::time(nullptr) + fSong.duration - testMpd->getElapsedTime() : NULL);
    } else {
        qDebug("stopped");
        presence.details = "Fantastic Audio Player";
        presence.state = "Playback Stopped";
        presence.largeImageKey = "unknown";
        presence.smallImageKey = "stop";
        presence.smallImageText = "Stopped";
    }

    Discord_UpdatePresence(&presence);
}

void Fap::handleEvents(int event) {
    qDebug() << "EVENT: " << event;

    if (event & MPD_IDLE_DATABASE)
        updateSongList();

    if (event & MPD_IDLE_QUEUE)
        updateQueue();

    if (event & MPD_IDLE_PLAYER)
        updateStatus();

    // if (event & FAP_CURRENT_SONG_END)
    //     testMpd->remove(0);

    if (event & FAP_ELAPSED_TIME)
        updateElapsed();
}

void Fap::updateQueue() {
    qDebug("Updating queue");
    ui.queueList->clear();

    QVector<Player::FapSong> queue = testMpd->getQueueSongs();

    for (int i = 0; i < queue.size(); i++) {
        Player::FapSong currentSong = testMpd->getCurrentSong();
        ui.queueList->addItem((currentSong.path == queue.at(i).path ? "-> " : "") + queue.at(i).title);
    }
        //ui.queueTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << queue.at(i).title << queue.at(i).artist << queue.at(i).album << queue.at(i).path, 1));
}

void Fap::updateSongList() {
    ui.songTree->clear();

    QVector<Player::FapSong> songs = testMpd->getSongs();

    for (int i = 0; i < songs.size(); i++)
        ui.songTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << songs.at(i).title << songs.at(i).artist << songs.at(i).album << songs.at(i).path, 1));
}

void Fap::updateStatus() {
    qDebug("updateStatus: calling getStatus()");
    int status = testMpd->getStatus();

    QString file = testMpd->getMusicDir(&settings) + "/" + testMpd->getCurrentSong().path;

    if (status == 0)
        return;

    if (status != MPD_STATE_STOP) {
        Player::FapSong fSong = testMpd->getCurrentSong();

        ui.titleArtistLabel->setText(fSong.title + " - " + fSong.artist);
        ui.albumLabel->setText(fSong.album);
        QPixmap cover = getCover(testMpd->getMusicDir(&settings) + "/" + fSong.path);
        ui.coverLabel->setPixmap(cover.scaled(ui.coverLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    if (status == MPD_STATE_STOP || status == MPD_STATE_PAUSE)
        ui.playPauseButton->setIcon(QIcon(":/images/play-button"));
    else if (status == MPD_STATE_PLAY)
        ui.playPauseButton->setIcon(QIcon(":/images/pause-button"));

    updateQueue();
    updateDiscordPresence(getCover(file), hasCover(file));
}

void Fap::updateElapsed() {
    int elapsedTime = testMpd->getElapsedTime();
    int duration = testMpd->getCurrentSong().duration;
    printf("Duration: %d Elapsed: %d\n", duration, elapsedTime);

    ui.timeLabel->setText(secToMMSS(elapsedTime) + "/" + secToMMSS(duration));
    ui.seekSlider->setMaximum(testMpd->getCurrentSong().duration);

    ui.seekSlider->blockSignals(true);
    ui.seekSlider->setValue(elapsedTime);
    ui.seekSlider->blockSignals(false);
}

void Fap::updateCurrentSong() {
    int status = testMpd->getStatus();

    if (status == 0)
        return;

    if (status != MPD_STATE_STOP) {
        Player::FapSong fSong = testMpd->getCurrentSong();

        ui.titleArtistLabel->setText(fSong.title + " - " + fSong.artist);
        ui.albumLabel->setText(fSong.album);
    }
}

// Slots
void Fap::on_songTree_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    testMpd->playSong(item->text(3));
    updateQueue();
}

void Fap::on_playPauseButton_clicked() {
    int status = testMpd->getStatus();

    if (status == 0)
        return;

    if (status == MPD_STATE_STOP)
        testMpd->play();
    else
        testMpd->playPause();

    updateStatus();
}

void Fap::on_stopButton_clicked() {
    testMpd->stopSong();

    ui.titleArtistLabel->setText("Not playing");
    ui.albumLabel->setText("");
    ui.timeLabel->setText("00:00/00:00");
}

void Fap::on_nextButton_clicked() {
    testMpd->next();
    // testMpd->remove(0);
}

void Fap::on_prevButton_clicked() {
    testMpd->prev();
}

void Fap::on_seekSlider_valueChanged(int value) {
    testMpd->seek(value);
    QString file = testMpd->getMusicDir(&settings) + "/" + testMpd->getCurrentSong().path;
    updateDiscordPresence(getCover(file), hasCover(file));
}