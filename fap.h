#pragma once

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QBuffer>
#include <QSettings>
#include <cstring>
#include <cstdio>
#include <curl/curl.h>

#include "ui_main.h"
#include "player.h"
#include "mpdconf.h"
#include "cover.h"
#include "discord_rpc.h"
#include "discordrpc/assets.h"

class Fap : public QMainWindow {
    Q_OBJECT

    public:
        explicit Fap(QMainWindow *parent = nullptr);
        ~Fap();

        QSettings settings;

    private slots:
        void on_songTree_itemDoubleClicked(QTreeWidgetItem *item, int column);
        void on_playPauseButton_clicked();
        void on_stopButton_clicked();
        void on_nextButton_clicked();
        void on_prevButton_clicked();
        void on_seekSlider_valueChanged(int value);
        void on_queueList_itemDoubleClicked(QListWidgetItem *item);
        void queueContextMenu(const QPoint &pos);
        void songTreeContextMenu(const QPoint &pos);

    private:
        Ui::Fap ui;

        struct mpd_connection *conn = nullptr;

        Player *mpd = nullptr;

        int setNewHost();
        QString secToMMSS(int time);
        void initDiscord();
        void updateDiscordPresence(QPixmap cover, bool hasCover);
        void handleEvents(int event);
        void updateQueue();
        void updateSongList();
        void updateStatus();
        void updateElapsed();
        void updateCurrentSong();
        
        void removeFromQueue();
        void playNow();
        void contextAppendQueue();
        void contextPlayNext();

        void playSong(QString path);
        void playPause();

        char *APPLICATION_ID;
};
