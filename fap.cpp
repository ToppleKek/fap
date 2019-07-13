#include "fap.h"

Fap::Fap(QMainWindow *parent) : QMainWindow(parent), settings("ToppleKek", "Fap") {
    ui.setupUi(this);
    
    std::srand(std::time(nullptr));

    qDebug() << "Syncing asset config...";
    dAppSyncConfig(settings.value("discord/appid").toString(), &settings);

    // Load the default cover, as it is not scaled
    QPixmap unknownCover;
    unknownCover.load(":/images/unknown");

    ui.coverLabel->setPixmap(unknownCover.scaled(ui.coverLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    
    // Make columns a reasonable size
    ui.songTree->setColumnWidth(0, 300);
    ui.songTree->setColumnWidth(1, 200);
    ui.songTree->setColumnWidth(2, 200);

    ui.playlistTree->setColumnWidth(0, 300);
    ui.playlistTree->setColumnWidth(1, 200);
    ui.playlistTree->setColumnWidth(2, 200);

    initDiscord();
    curl_global_init(CURL_GLOBAL_DEFAULT);
    
    // No MPD server set
    if (settings.value("mpd/host").isNull() || settings.value("mpd/port").isNull()) {
        if (settings.value("discord/enabled").toBool()) {
            DiscordRichPresence presence;
            memset(&presence, 0, sizeof(presence));

            presence.details = "Configuring MPD Server";
            presence.state = "FAP - Fantastic Audio Player";
            presence.startTimestamp = std::time(nullptr);

            Discord_UpdatePresence(&presence);
        }

        setNewHost();
    }
    
    // Attempt first connection
    bool ok;
    unsigned port = settings.value("mpd/port").toUInt(&ok);

    conn = mpd_connection_new(settings.value("mpd/host").toByteArray().data(), ok ? port : 0, 300000);
    
    // If the connection failed
    while (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        fprintf(stderr, "ERROR: Failed to connect to the MPD server: %d\n", mpd_connection_get_error(conn));

        if (settings.value("discord/enabled").toBool()) {
            DiscordRichPresence presence;
            memset(&presence, 0, sizeof(presence));

            presence.details = "Reconfiguring MPD Server";
            presence.state = "Failed to connect";
            presence.startTimestamp = std::time(nullptr);

            Discord_UpdatePresence(&presence);
        }

        // Free and shutdown
        if (setNewHost() == QDialog::Rejected) {
            mpd_connection_free(conn);
            Discord_Shutdown();
            curl_global_cleanup();
            exit(0);
        }
        
        // Retry connection
        bool ok;
        unsigned port = settings.value("mpd/port").toUInt(&ok);

        conn = mpd_connection_new(settings.value("mpd/host").toByteArray().data(), ok ? port : 0, 300000);
    }

    mpd = new Player(conn);

    mpd->restoreSavedVolume(&settings);
   
    QStringList p = mpd->getPlaylists(); 
    qDebug() << "Looking for playlists";

    for (int i = 0; i < p.size(); i++)
        qDebug() << p.at(i);

    pTab = new PlaylistTab(&ui, mpd);
    pTab->updateList();

    // Set some config 
    if (!settings.contains("discord/enabled"))
        settings.setValue("discord/enabled", false);
    if (!settings.contains("discord/appid"))
        settings.setValue("discord/appid", APPLICATION_ID);

    printf("MPD Connection established with host: %s port: %d\n", settings.value("mpd/host").toByteArray().data(), port);
    
    // Update UI
    mpd->update();
    updateStatus();
    updateSongList();
    updateQueue();

    // Hide "ID" column
    ui.songTree->hideColumn(3);
    ui.playlistTree->hideColumn(3);
    
    // Connect slots
    connect(mpd, &Player::mpdEvent, this, &Fap::handleEvents);
    connect(ui.actionMPD_Configuration, &QAction::triggered, this, &Fap::setNewHost);
    connect(ui.actionFAP_Configuration, &QAction::triggered, this, &Fap::openConfDialog);
    connect(ui.actionAbout_FAP, &QAction::triggered, this, &Fap::showAbout);
    connect(ui.queueList, &QWidget::customContextMenuRequested, this, &Fap::queueContextMenu);
    connect(ui.songTree, &QWidget::customContextMenuRequested, this, &Fap::songTreeContextMenu);
    

    // Start MPD polling
    QTimer *eventTimer = new QTimer(this);
    connect(eventTimer, &QTimer::timeout, mpd, &Player::pollEvents);
    eventTimer->start(500);
}

Fap::~Fap() {
    if (mpd->getVolume() > 0)
        mpd->saveVolume();

    settings.setValue("player/volume", mpd->getSavedVolume());

    mpd_connection_free(conn);
    Discord_Shutdown();
    curl_global_cleanup();
}

void Fap::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    
    QFontMetrics mArtistLabel(ui.titleArtistLabel->font());
    QFontMetrics mAlbumLabel(ui.albumLabel->font());
    
    if (mpd->getStatus() == MPD_STATE_STOP) {
        ui.titleArtistLabel->setText(mArtistLabel.elidedText("Not Playing", Qt::ElideRight, ui.titleArtistLabel->width()));
        ui.albumLabel->setText("");
    } else {
        Player::FapSong fSong = mpd->getCurrentSong();

        ui.titleArtistLabel->setText(mArtistLabel.elidedText(fSong.title + " - " + fSong.artist, Qt::ElideRight, ui.titleArtistLabel->width()));
        ui.albumLabel->setText(mAlbumLabel.elidedText(fSong.album, Qt::ElideRight, ui.albumLabel->width()));
    }
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

void Fap::applySettings(FAPConfDialog *d) {
    settings.setValue("discord/enabled", d->getDRPCEnabled());
    settings.setValue("discord/appid", d->getDRPCAppID());
    settings.setValue("discord/token", d->getDRPCToken());    

    if (!settings.value("discord/enabled").toBool())
        Discord_Shutdown();
    else {
        Discord_Shutdown();
        
        qDebug("init discord");
        Discord_Initialize(settings.value("discord/appid").toString().toUtf8().data(), nullptr, 1, NULL);
        updateStatus();
    }
}

void Fap::openConfDialog() {
    FAPConfDialog *d = new FAPConfDialog(nullptr, &settings);
    
    connect(d->ui.fapConfButtonBox, &QDialogButtonBox::clicked, [d, this](QAbstractButton *button) {
                qDebug() << "APPID: " << d->getDRPCAppID() << " BUTTON: " << button->text();
                if (button->text() == "Apply")
                    applySettings(d);
                else if (button->text() == "&OK")
                    d->accept();
                else
                    d->reject();
            });

    if (d->exec() == QDialog::Accepted)
        applySettings(d);
}

void Fap::showAbout() {
    QDialog *d = new QDialog();

    Ui::About aboutUi;
    aboutUi.setupUi(d);

    d->show();
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
    if (!settings.value("discord/enabled").toBool())
        return;

    Discord_Initialize(settings.value("discord/appid").toString().toUtf8().data(), nullptr, 1, NULL);
}

void Fap::updateDiscordPresence(QPixmap cover, bool hasCover) {
    if (!settings.value("discord/enabled").toBool())
        return;

    int status = mpd->getStatus();
    DiscordRichPresence presence;
    memset(&presence, 0, sizeof(presence));

    if (status == 0)
        return;

    if (status != MPD_STATE_STOP) {
        Player::FapSong fSong = mpd->getCurrentSong();

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

            dAppUploadAsset(settings.value("discord/appid").toString().toUtf8().data(), encoded.data(), fSong.album == "Unknown" ? fSong.title : fSong.album, &settings);

            presence.largeImageKey = settings.value("assets/" + fSong.album).toString().toStdString().c_str();
        } else
            presence.largeImageKey = "unknown";

        presence.largeImageText = album;
        presence.smallImageKey = (status == MPD_STATE_PLAY ? "play" : "pause");
        presence.smallImageText = (status == MPD_STATE_PLAY ? "Playing" : "Paused");
        presence.startTimestamp = (status == MPD_STATE_PLAY ? std::time(nullptr) : NULL);
        presence.endTimestamp = (status == MPD_STATE_PLAY ? std::time(nullptr) + fSong.duration - mpd->getElapsedTime() : NULL);
    } else {
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

    if (event & MPD_IDLE_MIXER)
        updateVolume();

    if (event & FAP_CURRENT_SONG_CHANGE)
        updateCurrentSong();

    if (event & FAP_ELAPSED_TIME)
        updateElapsed();
}

void Fap::updateQueue() {
    qDebug("Updating queue");
    ui.queueList->clear();

    QVector<Player::FapSong> queue = mpd->getQueueSongs();

    for (int i = 0; i < queue.size(); i++) {
        Player::FapSong currentSong = mpd->getCurrentSong();
        ui.queueList->addItem((currentSong.pos == i ? "-> " : "") + queue.at(i).title);
    }

    QFontMetrics m(ui.queueLabel->font());
    ui.queueLabel->setText(m.elidedText("Play Queue" + (mpd->getShufflePlaylist() == "" ? "" : " - " + mpd->getShufflePlaylist()), Qt::ElideRight, ui.queueLabel->width()));
}

void Fap::updateSongList() {
    ui.songTree->clear();

    QVector<Player::FapSong> songs = mpd->getSongs();

    for (int i = 0; i < songs.size(); i++)
        ui.songTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << songs.at(i).title << songs.at(i).artist << songs.at(i).album << songs.at(i).path, 1));
}

// TODO: This looks like utter garbage. Fix it!
void Fap::updateStatus() {
    int status = mpd->getStatus();

    QString file = mpd->getMusicDir(&settings) + "/" + mpd->getCurrentSong().path;

    if (status == 0)
        return;

    if (status != MPD_STATE_STOP) {
        QFontMetrics mArtistLabel(ui.titleArtistLabel->font());
        QFontMetrics mAlbumLabel(ui.albumLabel->font());
        Player::FapSong fSong = mpd->getCurrentSong();

        ui.titleArtistLabel->setText(mArtistLabel.elidedText(fSong.title + " - " + fSong.artist, Qt::ElideRight, ui.titleArtistLabel->width()));
        ui.albumLabel->setText(mAlbumLabel.elidedText(fSong.album, Qt::ElideRight, ui.albumLabel->width()));
        QPixmap cover = getCover(mpd->getMusicDir(&settings) + "/" + fSong.path);
        ui.coverLabel->setPixmap(cover.scaled(ui.coverLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    
    if (status == MPD_STATE_STOP) {
        ui.playPauseButton->setIcon(QIcon(":images/play-button"));
        ui.titleArtistLabel->setText("Not playing");
        ui.albumLabel->setText("");
        ui.timeLabel->setText("00:00/00:00");
        QPixmap unknownCover;
        unknownCover.load(":/images/unknown");

        ui.coverLabel->setPixmap(unknownCover.scaled(ui.coverLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else if (status == MPD_STATE_PAUSE)
        ui.playPauseButton->setIcon(QIcon(":/images/play-button"));
    else if (status == MPD_STATE_PLAY)
        ui.playPauseButton->setIcon(QIcon(":/images/pause-button"));

    updateQueue();
    updateDiscordPresence(getCover(file), hasCover(file));
}

void Fap::updateVolume() {
    int vol = mpd->getVolume();

    if (vol < 0)
        vol = mpd->getSavedVolume();

    ui.volumeSlider->blockSignals(true);
    ui.volumeSlider->setValue(vol < 0 ? 0 : vol);
    ui.volumeSlider->blockSignals(false);
    ui.volumeSlider->setToolTip("Vol. " + QString::number(vol) + "%");
    
    if (vol == 0) {
        ui.volumeButton->setIcon(QIcon(":/images/vol-mute"));
        ui.volumeButton->setToolTip("Unmute");
    } else {
        ui.volumeButton->setIcon(QIcon(":/images/vol"));
        ui.volumeButton->setToolTip("Mute");
    }
}

void Fap::updateCurrentSong() {
    updateStatus();

    QString shufflePlaylist = mpd->getShufflePlaylist();
    if (shufflePlaylist != "" && mpd->getCurrentSong().pos + 1 == mpd->getQueueLength())
        mpd->appendToQueue(mpd->getRandomSong(shufflePlaylist).path);
}

void Fap::updateElapsed() {
    int elapsedTime = mpd->getElapsedTime();
    int duration = mpd->getCurrentSong().duration;
    printf("Duration: %d Elapsed: %d\n", duration, elapsedTime);

    ui.timeLabel->setText(secToMMSS(elapsedTime) + "/" + secToMMSS(duration));

    ui.seekSlider->blockSignals(true);
    ui.seekSlider->setMaximum(mpd->getCurrentSong().duration);
    ui.seekSlider->setValue(elapsedTime);
    ui.seekSlider->blockSignals(false);
}

// QAction handlers
void Fap::removeFromQueue() {
    qDebug() << "Remove from queue action requested on item: " << ui.queueList->currentItem()->text();
    mpd->remove(ui.queueList->currentRow());
    updateQueue();
}

void Fap::playNow() {
    qDebug() << "Play now action requested on item: " << ui.queueList->currentItem()->text();
    mpd->playPos(ui.queueList->currentRow());
}

void Fap::contextAppendQueue() {
    QList<QTreeWidgetItem *> items = ui.songTree->selectedItems();

    for (int i = 0; i < items.size(); i++)
        mpd->appendToQueue(items.at(i)->text(3));
}

void Fap::contextPlayNext() {
    if (mpd->getStatus() == MPD_STATE_STOP)
        return;

    Player::FapSong s = mpd->getCurrentSong();
    QList<QTreeWidgetItem *> items = ui.songTree->selectedItems();
    for (int i = 0; i < items.size(); i++)
        mpd->insertIntoQueue(items.at(i)->text(3), s.pos + (i + 1));
}

void Fap::contextAddToPlaylist() {
    QAction *s = qobject_cast<QAction *>(sender());
    QList<QTreeWidgetItem *> items = ui.songTree->selectedItems();

    for (int i = 0; i < items.size(); i++)
        mpd->addToPlaylist(s->text(), items.at(i)->text(3));

    if (ui.playlistList->currentItem() != nullptr)
        pTab->updateTree(ui.playlistList->currentItem()->text());
}

void Fap::contextAddToNewPlaylist() {
    bool ok;
    QString name = QInputDialog::getText(this, "New Playlist", "Playlist Name:", QLineEdit::Normal, QString(), &ok);

    if (ok) {
        QList<QTreeWidgetItem *> items = ui.songTree->selectedItems();

        for (int i = 0; i < items.size(); i++)
            mpd->addToPlaylist(name, items.at(i)->text(3));

        pTab->updateList();
        if (ui.playlistList->currentItem() != nullptr)
            pTab->updateTree(ui.playlistList->currentItem()->text());
    }
}

// Slots
void Fap::on_songTree_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    mpd->playSong(item->text(3));
    updateQueue();
}

void Fap::on_playPauseButton_clicked() {
    int status = mpd->getStatus();

    if (status == 0)
        return;

    if (status == MPD_STATE_STOP && mpd->getShufflePlaylist() != "" && mpd->getQueueLength() == 0) {
        mpd->appendToQueue(mpd->getRandomSong(mpd->getShufflePlaylist()).path);
        mpd->play();
    } else if (status == MPD_STATE_STOP)
        mpd->play();
    else
        mpd->playPause();

    updateStatus();
}

void Fap::on_stopButton_clicked() {
    mpd->stopSong();
}

void Fap::on_nextButton_clicked() {
    mpd->next();
}

void Fap::on_prevButton_clicked() {
    mpd->prev();
}

void Fap::on_seekSlider_valueChanged(int value) {
    mpd->seek(value);
    QString file = mpd->getMusicDir(&settings) + "/" + mpd->getCurrentSong().path;
    updateDiscordPresence(getCover(file), hasCover(file));
}

void Fap::on_volumeSlider_valueChanged(int value) {
    mpd->setVolume(value);
    updateVolume();
}

void Fap::on_volumeButton_clicked() {
    int vol = mpd->getVolume();
    
    if (vol == 0)
        mpd->restoreVolume();
    else {
        mpd->saveVolume();
        mpd->setVolume(0);
    }

    updateVolume();
}

void Fap::on_queueList_itemDoubleClicked(QListWidgetItem *item) {
    mpd->playPos(ui.queueList->currentRow());
}

void Fap::queueContextMenu(const QPoint &pos) {
    QListWidgetItem *item = ui.queueList->itemAt(pos);
    QAction *clear = new QAction("Clear Queue", this);

    connect(clear, &QAction::triggered, mpd, &Player::clearQueue);

    if (item == nullptr) {
        QMenu *contextMenu = new QMenu(this);

        contextMenu->addAction(clear);
        contextMenu->exec(ui.queueList->mapToGlobal(pos));
        return;
    }
    
    ui.queueList->setCurrentItem(item);

    QMenu *contextMenu = new QMenu(this);  
    QAction *remove = new QAction("Remove", this);
    QAction *playNow = new QAction("Play Now", this);

    connect(remove, &QAction::triggered, this, &Fap::removeFromQueue);
    connect(playNow, &QAction::triggered, this, &Fap::playNow);
    
    contextMenu->addAction(remove); 
    contextMenu->addAction(playNow);
    contextMenu->addSeparator();
    contextMenu->addAction(clear);
    contextMenu->exec(ui.queueList->mapToGlobal(pos));
}

void Fap::songTreeContextMenu(const QPoint &pos) {
    QList <QTreeWidgetItem *> items = ui.songTree->selectedItems();

    if (items.at(0) == nullptr)
        return;

    QMenu *contextMenu = new QMenu(this);
    QAction *add = new QAction("Add to Play Queue", this);
    QAction *next = new QAction("Play Next", this);
    QMenu *plistSubMenu = contextMenu->addMenu("Add to Playlist...");

    QStringList plists = mpd->getPlaylists();

    for (int i = 0; i < plists.size(); i++) {
        QAction *action = new QAction(plists.at(i), this);
        plistSubMenu->addAction(action);
        connect(action, &QAction::triggered, this, &Fap::contextAddToPlaylist);
    }

    QAction *newPlist = new QAction("New Playlist...", this);
    plistSubMenu->addSeparator();
    plistSubMenu->addAction(newPlist);
    
    connect(newPlist, &QAction::triggered, this, &Fap::contextAddToNewPlaylist);
    connect(add, &QAction::triggered, this, &Fap::contextAppendQueue);
    connect(next, &QAction::triggered, this, &Fap::contextPlayNext);

    contextMenu->addAction(add);
    contextMenu->addAction(next);

    if (mpd->getStatus() == MPD_STATE_STOP)
        next->setEnabled(false);

    contextMenu->exec(ui.songTree->mapToGlobal(pos));
}
