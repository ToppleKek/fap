#pragma once

#include <QList>
#include <QSettings>
#include <QDebug>
#include <mpd/client.h>
#include <mpd/status.h>
#include <mpd/entity.h>
#include <mpd/search.h>
#include <mpd/tag.h>
#include <mpd/message.h>
#include <climits>

enum FapSignal { FAP_ELAPSED_TIME = 2048, FAP_CURRENT_SONG_CHANGE = 4096 };

class Player : public QObject {
    Q_OBJECT

    public:
        struct FapSong {        
            QString title;
            QString artist;
            QString album;
            QString path;
            unsigned duration = 0;
            unsigned pos;
            unsigned id;

            bool operator ==(const FapSong &a) const {
                return this->path == a.path && this->pos == a.pos;
            }

            bool operator !=(const FapSong &a) const {
                return this->path != a.path || this->pos != a.pos;
            }
        };

        typedef struct FapSong FapSong;

        explicit Player(struct mpd_connection *mpdConn);
        virtual ~Player() {};

        QVector<Player::FapSong> getSongs();
        QVector<Player::FapSong> getQueueSongs();
        QString getMusicDir(QSettings *settings);
        FapSong getCurrentSong();
        int getVolume();
        int getSavedVolume();
        int getStatus();
        unsigned getElapsedTime();
        unsigned getSongCount();
        unsigned getQueueLength();
        void play();
        void playPause();
        void next();
        void prev();
        void seek(int time);
        void setVolume(int vol);
        void saveVolume();
        void restoreSavedVolume(QSettings *settings);
        void restoreVolume();
        void playPos(unsigned pos);

        void update();

        void pollEvents();

        void recvEvent();
        void playSong(QString path);
        void stopSong();
        void remove(unsigned pos);
        void removeByID(unsigned id);
        void appendToQueue(QString path);
        void insertIntoQueue(QString path, unsigned pos);
        void clearQueue();
        QStringList getPlaylists();
        QVector<Player::FapSong> getPlaylistSongs(QString name);
        void addToPlaylist(QString pName, QString sPath);
        void deletePlaylist(QString name);
        void renamePlaylist(QString oldName, QString newName);
        void deleteFromPlaylist(QString pName, unsigned pos);
        void loadPlaylist(QString name);
        FapSong getRandomSong(QString plist);
        QString getShufflePlaylist();
        void setShufflePlaylist(QString plist);
        void handle_error();

    signals:
        void mpdEvent(int event);

    private:
        int currentStatus;
        unsigned currentElapsedTime;
        FapSong currentSong;
        unsigned currentSongCount;
        unsigned currentQueueCount;
        int currentVolume;
        int savedVolume;
        QString shufflePlaylist = "";

        struct mpd_connection *conn = nullptr;
};
