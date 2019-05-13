#pragma once

#include <QList>
#include <mpd/client.h>
#include <mpd/status.h>
#include <mpd/entity.h>
#include <mpd/search.h>
#include <mpd/tag.h>
#include <mpd/message.h>

#define FAP_ELAPSED_TIME 2048
#define FAP_CURRENT_SONG_END 4096

class Player : public QObject {
    Q_OBJECT

    public:
        struct FapSong {        
            QString title;
            QString artist;
            QString album;
            QString path;
            unsigned duration = 0;

            bool operator ==(const FapSong &a) const {
                return this->path == a.path;
            }
        };

        typedef struct FapSong FapSong;

        explicit Player(struct mpd_connection *mpdConn);
        virtual ~Player() {};

        QVector<Player::FapSong> getSongs();
        QVector<Player::FapSong> getQueueSongs();
        FapSong getCurrentSong();
        int getStatus();
        unsigned getElapsedTime();
        unsigned getSongCount();
        unsigned getQueueLength();
        void play();
        void playPause();
        void next();
        void prev();
        void seek(int time);

        // void idle();
        // void noIdle();
        void update();

        void pollEvents();

        void recvEvent();
        void playSong(QString path);
        void stopSong();
        void remove(unsigned pos);

        void handle_error();

        bool idling;

    signals:
        void mpdEvent(int event);
        void statusChanged(int newStatus);
        void currentSongChanged(FapSong newSong);
        void songListChanged(QList<Player::FapSong> newSongList);
        void queueSongListChanged(QList<Player::FapSong> newQueueSongList);

    private:
        int currentStatus;
        unsigned currentElapsedTime;
        FapSong currentSong;

        unsigned currentSongCount = 0;
        unsigned currentQueueCount = 0;

        struct mpd_connection *conn = nullptr;
};