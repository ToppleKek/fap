#include "tabs/songtab.h"

SongTab::SongTab(Ui::Fap *fapUi, Player *fapMpd) : ui(fapUi), mpd(fapMpd) {
    ui->songTree->setColumnWidth(0, 300);
    ui->songTree->setColumnWidth(1, 200);
    ui->songTree->setColumnWidth(2, 200);
    ui->songTree->hideColumn(3);

    connect(ui->songTree, &QWidget::customContextMenuRequested, this, &SongTab::treeContextMenu);
    connect(ui->songTree, &QTreeWidget::itemDoubleClicked, this, &SongTab::treeItemDoubleClicked);
}

void SongTab::treeItemDoubleClicked(QTreeWidgetItem *item) {
    unsigned pos;

    if (mpd->getStatus() == MPD_STATE_STOP)
        pos = 0;
    else
        pos = mpd->getCurrentSong().pos + 1;

    mpd->insertIntoQueue(item->text(3), pos);
    mpd->playPos(pos);
    mpd->setShufflePlaylist(QString());
}

void SongTab::treeContextMenu(const QPoint &pos) {
    QList <QTreeWidgetItem *> items = ui->songTree->selectedItems();

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
        connect(action, &QAction::triggered, this, &SongTab::contextAddToPlaylist);
    }

    QAction *newPlist = new QAction("New Playlist...");
    plistSubMenu->addSeparator();
    plistSubMenu->addAction(newPlist);
    
    connect(newPlist, &QAction::triggered, this, &SongTab::contextAddToNewPlaylist);
    connect(add, &QAction::triggered, this, &SongTab::contextAppendQueue);
    connect(next, &QAction::triggered, this, &SongTab::contextPlayNext);

    contextMenu->addAction(add);
    contextMenu->addAction(next);

    if (mpd->getStatus() == MPD_STATE_STOP)
        next->setEnabled(false);

    contextMenu->exec(ui->songTree->mapToGlobal(pos));
}

void SongTab::updateTree() {
    ui->songTree->clear();

    QVector<Player::FapSong> songs = mpd->getSongs();

    for (int i = 0; i < songs.size(); i++)
        ui->songTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << songs.at(i).title << songs.at(i).artist << songs.at(i).album << songs.at(i).path, 1));
}

void SongTab::contextAppendQueue() {
    QList<QTreeWidgetItem *> items = ui->songTree->selectedItems();

    for (int i = 0; i < items.size(); i++)
        mpd->appendToQueue(items.at(i)->text(3));
}

void SongTab::contextPlayNext() {
    if (mpd->getStatus() == MPD_STATE_STOP)
        return;

    Player::FapSong s = mpd->getCurrentSong();
    QList<QTreeWidgetItem *> items = ui->songTree->selectedItems();

    for (int i = 0; i < items.size(); i++)
        mpd->insertIntoQueue(items.at(i)->text(3), s.pos + (i + 1));
}

void SongTab::contextAddToPlaylist() {
    QAction *s = qobject_cast<QAction *>(sender());
    QList<QTreeWidgetItem *> items = ui->songTree->selectedItems();

    for (int i = 0; i < items.size(); i++)
        mpd->addToPlaylist(s->text(), items.at(i)->text(3));

    mpd->emitEvent(FAP_PLAYLIST_UPDATE);
}

void SongTab::contextAddToNewPlaylist() {
    bool ok;
    QString name = QInputDialog::getText(nullptr, "New Playlist", "Playlist Name:", QLineEdit::Normal, QString(), &ok);

    if (ok) {
        QList<QTreeWidgetItem *> items = ui->songTree->selectedItems();

        for (int i = 0; i < items.size(); i++)
            mpd->addToPlaylist(name, items.at(i)->text(3));

        mpd->emitEvent(FAP_PLAYLIST_UPDATE);
    }
}