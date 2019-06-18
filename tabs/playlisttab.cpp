#include "tabs/playlisttab.h"

void pTabUpdateList(Ui::Fap *ui, Player *mpd) {
    ui->playlistList->clear();

    QStringList plists = mpd->getPlaylists();

    for (int i = 0; i < plists.size(); i++)
        ui->playlistList->addItem(plists.at(i));
}

void pTabUpdateTree(Ui::Fap *ui, Player *mpd, QString name) {
    ui->playlistTree->clear();
    
    QVector<Player::FapSong> plist = mpd->getPlaylistSongs(name);

    for (int i = 0; i < plist.size(); i++)
        ui->playlistTree->addTopLevelItem(new QTreeWidgetItem(QStringList() << plist.at(i).title << plist.at(i).artist << plist.at(i).album << plist.at(i).path));
}

void pTabListContextMenu(Ui::Fap *ui, Player *mpd, const QPoint &pos) {
    QListWidgetItem *item = ui->playlistList->itemAt(pos);

    if (item == nullptr)
        return;
    
    ui->playlistList->setCurrentItem(item);
    
    QMenu *contextMenu = new QMenu();
    QAction *rename = new QAction("Rename");
    QAction *del = new QAction("Delete");

    Fap::connect(rename, &QAction::triggered, pTabContextRename);
    Fap::connect(del, &QAction::triggered, [ui, mpd, item]() {
                pTabContextDelete(ui, mpd, item);
            });
    
    contextMenu->addAction(rename);
    contextMenu->addAction(del);
    contextMenu->exec(ui->playlistList->mapToGlobal(pos));
}

void pTabTreeItemDoubleClicked(Player *mpd, QTreeWidgetItem *item) {
    mpd->playSong(item->text(3));
}

void pTabContextRename() {
    qDebug() << "pTabContextRename: NOT IMPLEMENTED";
}

void pTabContextDelete(Ui::Fap *ui, Player *mpd, QListWidgetItem *item) {
    mpd->deletePlaylist(item->text());
    pTabUpdateList(ui, mpd);
    ui->playlistTree->clear();
}
