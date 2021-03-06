#pragma once

#include <QObject>
#include <QTreeWidget>
#include <QListWidget>
#include <QAction>
#include <QInputDialog>
#include <QFontMetrics>
#include <QList>
#include "ui_main.h"
#include "player.h"

class FolderTab : public QObject {
    Q_OBJECT
    
    public:
        explicit FolderTab(Ui::Fap *fapUi, Player *fapMpd, QSettings *fapSettings);
        virtual ~FolderTab() {};
        void updateTree();

    private slots:
        void treeContextMenu(const QPoint &pos); 
        void treeItemDoubleClicked(QTreeWidgetItem *item);

    private:
        QVector<Player::FapSong> getSelectedSongs(QList<QTreeWidgetItem *> items);
        QTreeWidgetItem *createChildItem(Player::FapDir dir);
        void contextAddToPlaylist();
        void contextAddToNewPlaylist();
        void contextAppendQueue();
        void contextPlayNext();

        Ui::Fap *ui;
        Player *mpd;
        QSettings *settings;
};
