#pragma once

#include <QObject>
#include <QTreeWidget>
#include <QListWidget>
#include <QAction>
#include <QInputDialog>
#include <QFontMetrics>
#include "ui_main.h"
#include "player.h"

class PlaylistTab : public QObject {
    Q_OBJECT
    
    public:
        explicit PlaylistTab(Ui::Fap *ui, Player *mpd);
        virtual ~PlaylistTab() {};
        void updateList();
        void updateTree(QString name);

    private slots:
        void listContextMenu(const QPoint &pos);
        void treeContextMenu(const QPoint &pos); 
        void treeItemDoubleClicked(QTreeWidgetItem *item);

    private:
        void contextShuffleSet(QListWidgetItem *item);
        void contextLoad(QListWidgetItem *item);
        void contextRename(QListWidgetItem *item);
        void contextDelete(QListWidgetItem *item);
        void treeContextDelete(QTreeWidgetItem *item);
        void contextAddToPlaylist();
        void contextAddToNewPlaylist(QTreeWidgetItem *item);
        void contextAppendQueue(QTreeWidgetItem *item);
        void contextPlayNext(QTreeWidgetItem *item);

        Ui::Fap *ui;
        Player *mpd;
};
