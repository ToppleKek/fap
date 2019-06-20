#pragma once

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QBuffer>
#include <QSettings>
#include <QInputDialog>
#include <cstring>
#include <cstdio>
#include <curl/curl.h>

#include "ui_main.h"
#include "ui_about.h"
#include "player.h"
#include "tabs/playlisttab.h"
#include "dialogs/mpdconf.h"
#include "dialogs/fapconf.h"
#include "cover.h"
#include "tabs/playlisttab.h"
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
        struct mpd_connection *conn = nullptr;

        int setNewHost();
        void applySettings(FAPConfDialog *d);
        void openConfDialog();
        void showAbout(); 
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
        void contextAddToPlaylist();
        void contextAddToNewPlaylist(); 

        void playSong(QString path);
        void playPause();

        char *APPLICATION_ID = "585225607718109197";

        Ui::Fap ui;
        Player *mpd = nullptr;
        PlaylistTab *pTab = nullptr;
};

void discordReady(const DiscordUser *connectedUser);
void discordErrored(int errcode, const char* message);
void discordDisconnected(int errcode, const char* message);
