#include "tabs/playlisttab.h"

PlaylistTab::PlaylistTab(Ui::Fap *fapUi, Player *fapMpd) : ui(fapUi), mpd(fapMpd) {
    connect(ui->playlistList, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
                updateTree(item->text()); 
            });

    ui->playlistTree->setColumnWidth(0, 300);
    ui->playlistTree->setColumnWidth(1, 200);
    ui->playlistTree->setColumnWidth(2, 200);
    ui->playlistTree->hideColumn(3);

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
    QAction *shuffle = new QAction(mpd->getShufflePlaylist() == item->text() && mpd->getShuffleEnabled() ? "Stop Shuffle Playing" : "Shuffle Play This");
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
    QList<QTreeWidgetItem *> items = ui->playlistTree->selectedItems();

    if (items.at(0) == nullptr)
        return;

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
    
    connect(newPlist, &QAction::triggered, this, &PlaylistTab::contextAddToNewPlaylist);
    connect(add, &QAction::triggered, this, &PlaylistTab::contextAppendQueue);
    connect(next, &QAction::triggered, this, &PlaylistTab::contextPlayNext);
    connect(del, &QAction::triggered, this, &PlaylistTab::treeContextDelete);

    contextMenu->addAction(add);
    contextMenu->addAction(next);
    contextMenu->addAction(del);

    if (mpd->getStatus() == MPD_STATE_STOP)
        next->setEnabled(false);

    contextMenu->exec(ui->playlistTree->mapToGlobal(pos));
}

void PlaylistTab::treeItemDoubleClicked(QTreeWidgetItem *item) {
    unsigned pos;

    if (mpd->getStatus() == MPD_STATE_STOP)
        pos = 0;
    else
        pos = mpd->getCurrentSong().pos + 1;

    mpd->insertIntoQueue(item->text(3), pos);
    mpd->playPos(pos);
    mpd->setShufflePlaylist(ui->playlistList->currentItem()->text());
}

void PlaylistTab::contextShuffleSet(QListWidgetItem *item) {  
    QString shufflePlaylist = mpd->getShufflePlaylist();

    if (shufflePlaylist == item->text() && mpd->getShuffleEnabled()) {
        mpd->setShufflePlaylist(QString());
        mpd->setShuffle(false);
    } else if (!mpd->getShuffleEnabled()) {
        mpd->setShufflePlaylist(item->text());
        mpd->setShuffle(true);
    }
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
    QList<QTreeWidgetItem *> items = ui->playlistTree->selectedItems();

    for (int i = 0; i < items.size(); i++)
        mpd->addToPlaylist(s->text(), items.at(i)->text(3));

    if (ui->playlistList->currentItem() != nullptr)
        updateTree(ui->playlistList->currentItem()->text());
}

void PlaylistTab::treeContextDelete() {
    QString plist = ui->playlistList->currentItem()->text();
    QVector<Player::FapSong> songs = mpd->getPlaylistSongs(plist);
    QList<QTreeWidgetItem *> items = ui->playlistTree->selectedItems();
    int pos;

    for (int i = 0; i < items.size(); i++) {
        for (pos = 0; pos < songs.size(); pos++)
            if (songs.at(pos).path == items.at(i)->text(3))
                break;

        mpd->deleteFromPlaylist(plist, pos);
        songs = mpd->getPlaylistSongs(plist);
    }

    updateTree(plist);
}

void PlaylistTab::contextAddToNewPlaylist() {
    bool ok;
    QString name = QInputDialog::getText(nullptr, "New Playlist", "Playlist Name:", QLineEdit::Normal, QString(), &ok);
    QList<QTreeWidgetItem *> items = ui->playlistTree->selectedItems();

    if (ok) {
        for (int i = 0; i < items.size(); i++)
            mpd->addToPlaylist(name, items.at(i)->text(3));

        updateList();
        if (ui->playlistList->currentItem() != nullptr)
            updateTree(ui->playlistList->currentItem()->text());
    }
}

void PlaylistTab::contextAppendQueue() {
    QList<QTreeWidgetItem *> items = ui->playlistTree->selectedItems();

    for (int i = 0; i < items.size(); i++)
        mpd->appendToQueue(items.at(i)->text(3));
}

void PlaylistTab::contextPlayNext() {
    if (mpd->getStatus() == MPD_STATE_STOP)
        return;

    Player::FapSong s = mpd->getCurrentSong();
    QList<QTreeWidgetItem *> items = ui->playlistTree->selectedItems();
    
    for (int i = 0; i < items.size(); i++)
        mpd->insertIntoQueue(items.at(i)->text(3), s.pos + (i + 1));
}
