#include "tabs/playlisttab.h"

PlaylistTab::PlaylistTab(Ui::Fap *fapUi, Player *fapMpd) : ui(fapUi), mpd(fapMpd) {
    connect(ui->playlistList, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
                updateTree(item->text()); 
            });

    connect(ui->playlistTree, &QWidget::customContextMenuRequested, this, &PlaylistTab::treeContextMenu);
    connect(ui->playlistTree, &QTreeWidget::itemDoubleClicked, this, &PlaylistTab::treeItemDoubleClicked);
    connect(ui->playlistList, &QWidget::customContextMenuRequested, this, &PlaylistTab::listContextMenu);
}

void PlaylistTab::updateList() {
    ui->playlistList->clear();

    QStringList plists = mpd->getPlaylists();

    for (int i = 0; i < plists.size(); i++)
        ui->playlistList->addItem(plists.at(i));
}

void PlaylistTab::updateTree(QString name) {
    ui->playlistTree->clear();
    
    QVector<Player::FapSong> plist = mpd->getPlaylistSongs(name);

    for (int i = 0; i < plist.size(); i++)
        ui->playlistTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << plist.at(i).title << plist.at(i).artist << plist.at(i).album << plist.at(i).path));
}

void PlaylistTab::listContextMenu(const QPoint &pos) {
    QListWidgetItem *item = ui->playlistList->itemAt(pos);

    if (item == nullptr)
        return;
    
    ui->playlistList->setCurrentItem(item);
    
    QMenu *contextMenu = new QMenu();
    QAction *shuffle = new QAction(mpd->getShufflePlaylist() == item->text() ? "Stop Shuffle Playing" : "Shuffle Play This");
    QAction *load = new QAction("Load into Queue");
    QAction *rename = new QAction("Rename");
    QAction *del = new QAction("Delete");
    
    connect(shuffle, &QAction::triggered, [this, item]() {
                contextShuffleSet(item);
            });

    connect(load, &QAction::triggered, [this, item]() {
                contextLoad(item);
            });

    connect(rename, &QAction::triggered, [this, item]() {
                contextRename(item);
            });

    connect(del, &QAction::triggered, [this, item]() {
                contextDelete(item);
            });

    contextMenu->addAction(shuffle);
    contextMenu->addAction(load);
    contextMenu->addAction(rename);
    contextMenu->addAction(del);
    contextMenu->exec(ui->playlistList->mapToGlobal(pos));
}

void PlaylistTab::treeContextMenu(const QPoint &pos) {
    QTreeWidgetItem *item = ui->playlistTree->itemAt(pos);

    if (item == nullptr)
        return;

    ui->playlistTree->setCurrentItem(item);

    QMenu *contextMenu = new QMenu();
    QAction *add = new QAction("Add to Play Queue");
    QAction *next = new QAction("Play Next");
    QMenu *plistSubMenu = contextMenu->addMenu("Add to Playlist...");
    QAction *del = new QAction("Remove From Playlist");

    QStringList plists = mpd->getPlaylists();

    for (int i = 0; i < plists.size(); i++) {
        QAction *action = new QAction(plists.at(i));
        plistSubMenu->addAction(action);

        connect(action, &QAction::triggered, this, &PlaylistTab::contextAddToPlaylist);
    }

    QAction *newPlist = new QAction("New Playlist...");
    plistSubMenu->addSeparator();
    plistSubMenu->addAction(newPlist);
    
    connect(newPlist, &QAction::triggered, [this, item]() {
                contextAddToNewPlaylist(item);
            });

    connect(add, &QAction::triggered, [this, item]() {
                contextAppendQueue(item);
            });

    connect(next, &QAction::triggered, [this, item]() {
                contextPlayNext(item);
            });

    connect(del, &QAction::triggered, [this, item]() {
                treeContextDelete(item);
            });

    contextMenu->addAction(add);
    contextMenu->addAction(next);
    contextMenu->addAction(del);

    if (mpd->getStatus() == MPD_STATE_STOP)
        next->setEnabled(false);

    contextMenu->exec(ui->playlistTree->mapToGlobal(pos));
}

void PlaylistTab::treeItemDoubleClicked(QTreeWidgetItem *item) {
    mpd->playSong(item->text(3));
}

void PlaylistTab::contextShuffleSet(QListWidgetItem *item) {  
    QString shufflePlaylist = mpd->getShufflePlaylist();

    if (shufflePlaylist == item->text())
        mpd->setShufflePlaylist(QString());
    else
        mpd->setShufflePlaylist(item->text());
    
    shufflePlaylist = mpd->getShufflePlaylist();

    QFontMetrics m(ui->queueLabel->font());
    ui->queueLabel->setText(m.elidedText("Play Queue" + (shufflePlaylist == "" ? "" : " - " + shufflePlaylist), Qt::ElideRight, ui->queueLabel->width()));

    if (shufflePlaylist != "" && mpd->getCurrentSong().pos + 1 == mpd->getQueueLength())
        mpd->appendToQueue(mpd->getRandomSong(shufflePlaylist).path);
}

void PlaylistTab::contextLoad(QListWidgetItem *item) {
    mpd->loadPlaylist(item->text());
}

void PlaylistTab::contextRename(QListWidgetItem *item) {
    QString oldName(item->text());
    
    bool ok;
    QString newName = QInputDialog::getText(nullptr, "Rename Playlist", "New Playlist Name:", QLineEdit::Normal, oldName, &ok);
    
    if (ok) {
        mpd->renamePlaylist(oldName, newName);
        updateList();
    }
}

void PlaylistTab::contextDelete(QListWidgetItem *item) {
    mpd->deletePlaylist(item->text());
    updateList();
    ui->playlistTree->clear();
}

void PlaylistTab::contextAddToPlaylist() {
    QAction *s = qobject_cast<QAction *>(sender());
    mpd->addToPlaylist(s->text(), ui->playlistTree->currentItem()->text(3));
    if (ui->playlistList->currentItem() != nullptr)
        updateTree(ui->playlistList->currentItem()->text());
}

void PlaylistTab::treeContextDelete(QTreeWidgetItem *item) {
    QString plist = ui->playlistList->currentItem()->text();
    QVector<Player::FapSong> songs = mpd->getPlaylistSongs(plist);
    int pos;

    for (pos = 0; pos < songs.size(); pos++)
        if (songs.at(pos).path == item->text(3))
            break;

    mpd->deleteFromPlaylist(plist, pos);
    updateTree(plist);
}

void PlaylistTab::contextAddToNewPlaylist(QTreeWidgetItem *item) {
    bool ok;
    QString name = QInputDialog::getText(nullptr, "New Playlist", "Playlist Name:", QLineEdit::Normal, QString(), &ok);

    if (ok) {
        mpd->addToPlaylist(name, item->text(3));
        updateList();
        if (ui->playlistList->currentItem() != nullptr)
            updateTree(ui->playlistList->currentItem()->text());
    }
}

void PlaylistTab::contextAppendQueue(QTreeWidgetItem *item) {
    mpd->appendToQueue(item->text(3));
}

void PlaylistTab::contextPlayNext(QTreeWidgetItem *item) {
    if (mpd->getStatus() == MPD_STATE_STOP)
        return;

    Player::FapSong s = mpd->getCurrentSong();
    mpd->insertIntoQueue(item->text(3), s.pos + 1);
}
