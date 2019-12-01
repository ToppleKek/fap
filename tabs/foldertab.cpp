#include "tabs/foldertab.h"

FolderTab::FolderTab(Ui::Fap *fapUi, Player *fapMpd, QSettings *fapSettings) : ui(fapUi), mpd(fapMpd), settings(fapSettings) {
    ui->folderTree->setColumnWidth(0, 300);
    ui->folderTree->setColumnWidth(1, 200);
    ui->folderTree->setColumnWidth(2, 200);
    ui->folderTree->hideColumn(3);

    connect(ui->folderTree, &QWidget::customContextMenuRequested, this, &FolderTab::treeContextMenu);
    connect(ui->folderTree, &QTreeWidget::itemDoubleClicked, this, &FolderTab::treeItemDoubleClicked);
}

QVector<Player::FapSong> FolderTab::getSelectedSongs(QList<QTreeWidgetItem *> items) {
    Player::FapDir currentDir;
    QVector<Player::FapSong> songs;

    for (int i = 0; i < items.size(); i++) {
        if (items.at(i)->text(3) == "") {
            currentDir = mpd->scanDir(items.at(i)->text(0), settings, false);
            songs += mpd->getDirSongs(currentDir);
        } else if (items.at(i)->text(3) != "")
            songs += mpd->getSong(items.at(i)->text(3));
    }

    return songs;
}

void FolderTab::updateTree() {
    ui->folderTree->clear();
    
    Player::FapDir dir = mpd->scanDir("", settings, true);
    QTreeWidgetItem *parent = new QTreeWidgetItem(QStringList() << "MPD Music Directory");

    for (int i = 0; i < dir.subDirs.size(); i++)
        parent->addChild(createChildItem(dir.subDirs.at(i)));

    for (int i = 0; i < dir.songs.size(); i++)
        parent->addChild(new QTreeWidgetItem(QStringList() << dir.songs.at(i).title << dir.songs.at(i).artist << dir.songs.at(i).album << dir.songs.at(i).path));

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

    contextMenu->addAction(add);
    contextMenu->addAction(next);

    if (mpd->getStatus() == MPD_STATE_STOP)
        next->setEnabled(false);

    contextMenu->exec(ui->folderTree->mapToGlobal(pos));
}

void FolderTab::treeItemDoubleClicked(QTreeWidgetItem *item) {
    Player::FapSong s = mpd->getCurrentSong();

    mpd->insertIntoQueue(item->text(3), s.pos + 1);
    mpd->playPos(s.pos + 1);
}

void FolderTab::contextAddToPlaylist() {
    QVector<Player::FapSong> songs = getSelectedSongs(ui->folderTree->selectedItems());

    QAction *s = qobject_cast<QAction *>(sender());

    for (int i = 0; i < songs.size(); i++)
        mpd->addToPlaylist(s->text(), songs.at(i).path);

    mpd->emitEvent(FAP_PLAYLIST_UPDATE);
}

void FolderTab::contextAddToNewPlaylist() {
    bool ok;
    QString name = QInputDialog::getText(nullptr, "New Playlist", "Playlist Name:", QLineEdit::Normal, QString(), &ok);
    QVector<Player::FapSong> songs = getSelectedSongs(ui->folderTree->selectedItems());

    if (ok) {
        for (int i = 0; i < songs.size(); i++)
            mpd->addToPlaylist(name, songs.at(i).path);

        mpd->emitEvent(FAP_PLAYLIST_UPDATE);
    }
}

void FolderTab::contextAppendQueue() {
    QVector<Player::FapSong> songs = getSelectedSongs(ui->folderTree->selectedItems());

    for (int i = 0; i < songs.size(); i++)
        mpd->appendToQueue(songs.at(i).path);
}

void FolderTab::contextPlayNext() {
    if (mpd->getStatus() == MPD_STATE_STOP)
        return;

    Player::FapSong s = mpd->getCurrentSong();
    QVector<Player::FapSong> songs = getSelectedSongs(ui->folderTree->selectedItems());

    for (int i = 0; i < songs.size(); i++)
        mpd->insertIntoQueue(songs.at(i).path, s.pos + (i + 1));
}
