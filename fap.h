#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QDebug>
#include <QTimer>
#include <QSettings>
#include <cstring>
#include <cstdio>

#include "ui_main.h"
#include "player.h"
#include "mpdconf.h"
#include "discord_rpc.h"

class Fap : public QMainWindow {
    Q_OBJECT

    public:
        explicit Fap(QMainWindow *parent = nullptr);
        ~Fap();

    private slots:
        void on_songTree_itemDoubleClicked(QTreeWidgetItem *item, int column);
        void on_playPauseButton_clicked();
        void on_stopButton_clicked();
        void on_nextButton_clicked();
        void on_prevButton_clicked();
        void on_seekSlider_valueChanged(int value);

    private:
        Ui::Fap ui;

        struct mpd_connection *conn = nullptr;

        Player *testMpd = nullptr;

        QSettings settings;

        QString secToMMSS(int time);
        void initDiscord();
        void updateDiscordPresence();
        void handleEvents(int event);
        void updateQueue();
        void updateSongList();
        void updateStatus();
        void updateElapsed();
        void updateCurrentSong();

        void playSong(QString path);
        void playPause();

        char *APPLICATION_ID;
};