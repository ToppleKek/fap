#pragma once

#include <QObject>
#include <QTreeWidget>
#include <QListWidget>
#include <QAction>
#include <QInputDialog>
#include <QFontMetrics>
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
        QTreeWidgetItem *createChildItem(Player::FapDir dir);
        void contextLoad(QListWidgetItem *item);
        void contextRename(QListWidgetItem *item);
        void contextDelete(QListWidgetItem *item);
        void treeContextDelete();
        void contextAddToPlaylist();
        void contextAddToNewPlaylist();
        void contextAppendQueue();
        void contextPlayNext();

        Ui::Fap *ui;
        Player *mpd;
        QSettings *settings;
};
