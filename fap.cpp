#include "fap.h"

Fap::Fap(QMainWindow *parent) : QMainWindow(parent), conn(mpd_connection_new(NULL, 0, 300000)), testMpd(conn), APPLICATION_ID("492434528644759564") {
    ui.setupUi(this);

    testMpd.update();

    QVector<Player::FapSong> songs = testMpd.getSongs();
    QVector<Player::FapSong> queue = testMpd.getQueueSongs();
    initDiscord();
    updateStatus();

    ui.songTree->hideColumn(3);
    ui.queueTree->hideColumn(3);

    foreach(Player::FapSong song, songs) {
        ui.songTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << song.title << song.artist << song.album << song.path, 1));
    }
    foreach(Player::FapSong song, queue) {
        ui.queueTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << song.title << song.artist << song.album << song.path, 1));
    }

    connect(&testMpd, &Player::mpdEvent, this, &Fap::handleEvents);

    QTimer *eventTimer = new QTimer(this);
    connect(eventTimer, &QTimer::timeout, &testMpd, &Player::pollEvents);
    eventTimer->start(500);
}

Fap::~Fap() {
    mpd_connection_free(conn);
    Discord_Shutdown();
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

void Fap::updateDiscordPresence() {
    qDebug("update presence");
    int status = testMpd.getStatus();
    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(presence));

    if (status == 0)
        return;

    if (status != MPD_STATE_STOP) {
        qDebug("not stopped");
        Player::FapSong fSong = testMpd.getCurrentSong();

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
        presence.largeImageKey = "obama_white_person";
        presence.largeImageText = album;
        presence.smallImageKey = (status == MPD_STATE_PLAY ? "play" : "pause");
        presence.smallImageText = (status == MPD_STATE_PLAY ? "Playing" : "Paused");
        presence.startTimestamp = (status == MPD_STATE_PLAY ? std::time(nullptr) : NULL);
        presence.endTimestamp = (status == MPD_STATE_PLAY ? std::time(nullptr) + fSong.duration - testMpd.getElapsedTime() : NULL);
    } else {
        qDebug("stopped");
        presence.details = "Fantastic Audio Player";
        presence.state = "Playback Stopped";
        presence.largeImageKey = "obama_white_person";
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
    //     testMpd.remove(0);

    if (event & FAP_ELAPSED_TIME)
        updateElapsed();
}

void Fap::updateQueue() {
    qDebug("Updating queue");
    ui.queueTree->clear();

    QVector<Player::FapSong> queue = testMpd.getQueueSongs();

    foreach(Player::FapSong song, queue) {
        ui.queueTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << song.title << song.artist << song.album << song.path, 1));
    }
}

void Fap::updateSongList() {
    ui.songTree->clear();

    QVector<Player::FapSong> songs = testMpd.getSongs();

    foreach(Player::FapSong song, songs) {
        ui.songTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << song.title << song.artist << song.album << song.path, 1));
    }
}

void Fap::updateStatus() {
    qDebug("updateStatus: calling getStatus()");
    int status = testMpd.getStatus();

    updateDiscordPresence();

    if (status == 0)
        return;

    if (status != MPD_STATE_STOP) {
        Player::FapSong fSong = testMpd.getCurrentSong();

        ui.titleArtistLabel->setText(fSong.title + " - " + fSong.artist);
        ui.albumLabel->setText(fSong.album);
    }

    if (status == MPD_STATE_STOP || status == MPD_STATE_PAUSE)
        ui.playPauseButton->setIcon(QIcon(":/images/play-button"));
    else if (status == MPD_STATE_PLAY)
        ui.playPauseButton->setIcon(QIcon(":/images/pause-button"));
}

void Fap::updateElapsed() {
    int elapsedTime = testMpd.getElapsedTime();
    int duration = testMpd.getCurrentSong().duration;
    printf("Duration: %d Elapsed: %d\n", duration, elapsedTime);

    ui.timeLabel->setText(secToMMSS(elapsedTime) + "/" + secToMMSS(duration));
    ui.seekSlider->setMaximum(testMpd.getCurrentSong().duration);

    ui.seekSlider->blockSignals(true);
    ui.seekSlider->setValue(elapsedTime);
    ui.seekSlider->blockSignals(false);
}

void Fap::updateCurrentSong() {
    int status = testMpd.getStatus();

    if (status == 0)
        return;

    if (status != MPD_STATE_STOP) {
        Player::FapSong fSong = testMpd.getCurrentSong();

        ui.titleArtistLabel->setText(fSong.title + " - " + fSong.artist);
        ui.albumLabel->setText(fSong.album);
    }
}

// Slots
void Fap::on_songTree_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    testMpd.playSong(item->text(3));
    updateQueue();
}

void Fap::on_playPauseButton_clicked() {
    int status = testMpd.getStatus();

    if (status == 0)
        return;

    if (status == MPD_STATE_STOP)
        testMpd.play();
    else
        testMpd.playPause();

    updateStatus();
}

void Fap::on_stopButton_clicked() {
    testMpd.stopSong();

    ui.titleArtistLabel->setText("Not playing");
    ui.albumLabel->setText("");
    ui.timeLabel->setText("00:00/00:00");
}

void Fap::on_nextButton_clicked() {
    testMpd.next();
    // testMpd.remove(0);
}

void Fap::on_prevButton_clicked() {
    testMpd.prev();
}

void Fap::on_seekSlider_valueChanged(int value) {
    testMpd.seek(value);
    updateDiscordPresence();
}