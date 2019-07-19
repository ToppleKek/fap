#include "tabs/foldertab.h"

FolderTab::FolderTab(Ui::Fap *fapUi, Player *fapMpd, QSettings *fapSettings) : ui(fapUi), mpd(fapMpd), settings(fapSettings) {
    ui->folderTree->setColumnWidth(0, 300);
    ui->folderTree->setColumnWidth(1, 200);
    ui->folderTree->setColumnWidth(2, 200);
    ui->folderTree->hideColumn(3);

    connect(ui->folderTree, &QWidget::customContextMenuRequested, this, &FolderTab::treeContextMenu);
    connect(ui->folderTree, &QTreeWidget::itemDoubleClicked, this, &FolderTab::treeItemDoubleClicked);
}

void FolderTab::updateTree() {
    ui->folderTree->clear();
    
    Player::FapDir dir = mpd->scanDir("", settings, true);
    QTreeWidgetItem *parent = new QTreeWidgetItem(QStringList() << "MPD Music Directory");

    for (int i = 0; i < dir.subDirs.size(); i++)
        parent->addChild(createChildItem(dir.subDirs.at(i)));

    ui->folderTree->addTopLevelItem(parent);
}

QTreeWidgetItem *FolderTab::createChildItem(Player::FapDir dir) {
    QTreeWidgetItem *parent(new QTreeWidgetItem(QStringList() << dir.path));
    parent->setIcon(0, dir.icon);
    
    for (int i = 0; i < dir.songs.size(); i++)
        parent->addChild(new QTreeWidgetItem(QStringList() << dir.songs.at(i).title << dir.songs.at(i).artist << dir.songs.at(i).album << dir.songs.at(i).path));

    for (int i = 0; i < dir.subDirs.size(); i++)
        parent->addChild(createChildItem(dir.subDirs.at(i)));

    return parent;
}

void FolderTab::treeContextMenu(const QPoint &pos) {
    QList<QTreeWidgetItem *> items = ui->folderTree->selectedItems();

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

        connect(action, &QAction::triggered, this, &FolderTab::contextAddToPlaylist);
    }

    QAction *newPlist = new QAction("New Playlist...");
    plistSubMenu->addSeparator();
    plistSubMenu->addAction(newPlist);
    
    connect(newPlist, &QAction::triggered, this, &FolderTab::contextAddToNewPlaylist);
    connect(add, &QAction::triggered, this, &FolderTab::contextAppendQueue);
    connect(next, &QAction::triggered, this, &FolderTab::contextPlayNext);
    connect(del, &QAction::triggered, this, &FolderTab::treeContextDelete);

    contextMenu->addAction(add);
    contextMenu->addAction(next);
    contextMenu->addAction(del);

    if (mpd->getStatus() == MPD_STATE_STOP)
        next->setEnabled(false);

    contextMenu->exec(ui->folderTree->mapToGlobal(pos));
}

void FolderTab::treeItemDoubleClicked(QTreeWidgetItem *item) {
    mpd->playSong(item->text(3));
}

void FolderTab::contextLoad(QListWidgetItem *item) {
    mpd->loadPlaylist(item->text());
}

void FolderTab::contextAddToPlaylist() {
    qDebug() << "FolderTab::contextAddToPlaylist: UNIMPLEMENTED";
    /*
    QAction *s = qobject_cast<QAction *>(sender());
    QList<QTreeWidgetItem *> items = ui->playlistTree->selectedItems();

    for (int i = 0; i < items.size(); i++)
        mpd->addToPlaylist(s->text(), items.at(i)->text(3));

    if (ui->playlistList->currentItem() != nullptr)
        updateTree(ui->playlistList->currentItem()->text());
    */
}

void FolderTab::treeContextDelete() {
    qDebug() << "FolderTab::treeContextDelete: UNIMPLEMENTED";
    /*
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
    */
}

void FolderTab::contextAddToNewPlaylist() {
    qDebug() << "FolderTab::contextAddToNewPlaylist: UNIMPLEMENTED";
    /*
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
    */
}

void FolderTab::contextAppendQueue() {
    qDebug() << "FolderTab::contextAppendQueue: UNIMPLEMENTED";
    /*
    QList<QTreeWidgetItem *> items = ui->playlistTree->selectedItems();

    for (int i = 0; i < items.size(); i++)
        mpd->appendToQueue(items.at(i)->text(3));
    */
}

void FolderTab::contextPlayNext() {
    qDebug() << "FolderTab::contextPlayNext: UNIMPLEMENTED";
    /*
    if (mpd->getStatus() == MPD_STATE_STOP)
        return;

    Player::FapSong s = mpd->getCurrentSong();
    QList<QTreeWidgetItem *> items = ui->playlistTree->selectedItems();
    
    for (int i = 0; i < items.size(); i++)
        mpd->insertIntoQueue(items.at(i)->text(3), s.pos + (i + 1));
    */
}
