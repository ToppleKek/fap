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
   qDebug() << "pTabListContextMenu: NOT IMPLEMENTED"; 
}

void pTabTreeItemDoubleClicked(Player *mpd, QTreeWidgetItem *item) {
    mpd->playSong(item->text(3));
}
