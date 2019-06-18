#include "player.h"

Player::Player(struct mpd_connection *mpdConn) : conn(mpdConn) {
    qDebug("Player init");

    currentSong.path = "";

    mpd_connection_set_keepalive(conn, true);
}

QVector<Player::FapSong> Player::getSongs() {
    QVector<Player::FapSong> songs;
    struct mpd_entity *entity;
    const struct mpd_song *song;
    Player::FapSong fSong;

    if (!mpd_send_list_all_meta(conn, "")) {
        qDebug("Failed to send list_all_meta");
        handle_error();
    } else {
        while ((entity = mpd_recv_entity(conn)) != NULL) {
            if (mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG) {
                song = mpd_entity_get_song(entity);

                QString title = QString(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
                QString artist = QString(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
                QString album = QString(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));

                fSong.title = (title.length() < 1 ? QString(mpd_song_get_uri(song)) : title);
                fSong.artist = (artist.length() < 1 ? "Unknown" : artist);
                fSong.album = (album.length() < 1 ? "Unknown" : album);
                fSong.path = QString(mpd_song_get_uri(song));
                fSong.duration = mpd_song_get_duration(song);
                fSong.pos = (mpd_song_get_pos(song) >= UINT_MAX ? 0 : mpd_song_get_pos(song));

                songs << fSong;
            }

            mpd_entity_free(entity);
        }
    }

    return songs;
}

QVector<Player::FapSong> Player::getQueueSongs() {
    QVector<Player::FapSong> songs;
    struct mpd_entity *entity;
    const struct mpd_song *song;
    Player::FapSong fSong;

    if (!mpd_send_list_queue_meta(conn)) {
        qDebug("Failed to send list_queue_meta");
        handle_error();
    } else {
        while ((entity = mpd_recv_entity(conn)) != NULL) {
            if (mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG) {
                song = mpd_entity_get_song(entity);

                QString title = QString(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
                QString artist = QString(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
                QString album = QString(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));

                fSong.title = (title.length() < 1 ? QString(mpd_song_get_uri(song)) : title);
                fSong.artist = (artist.length() < 1 ? "Unknown" : artist);
                fSong.album = (album.length() < 1 ? "Unknown" : album);
                fSong.path = QString(mpd_song_get_uri(song));
                fSong.duration = mpd_song_get_duration(song);
                fSong.pos = (mpd_song_get_pos(song) >= UINT_MAX ? 0 : mpd_song_get_pos(song));

                songs << fSong;
            }

            mpd_entity_free(entity);
        }
    }

    return songs;
}

QString Player::getMusicDir(QSettings *settings) {
    qDebug() << settings->value("mpd/host").toString();
    if (!mpd_send_command(conn, "config", NULL) && mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        qDebug("getMusicDir: calling handle_error");
        handle_error();
    }

    struct mpd_pair *pair = mpd_recv_pair_named(conn, "music_directory");
    QString dir;

    if (pair != NULL) {
        qDebug(pair->value);
        dir = pair->value;

        mpd_return_pair(conn, pair);
    }

    mpd_response_finish(conn);
    
    return dir;
}

int Player::getStatus() {
    struct mpd_status *status;

    status = mpd_run_status(conn);
    if (status == NULL) {
        qDebug("Get status: calling handle_error");
        handle_error();
        return 0;
    }

    int s = mpd_status_get_state(status);
    
    mpd_status_free(status);

    return s;
}

unsigned Player::getElapsedTime() {
    struct mpd_status *status;

    status = mpd_run_status(conn);
    if (status == NULL) {
        qDebug("Get status: calling handle_error");
        handle_error();
        return 0;
    }

    return mpd_status_get_elapsed_time(status);
}

unsigned Player::getSongCount() {
    struct mpd_stats *stats = mpd_run_stats(conn);
    unsigned n = mpd_stats_get_number_of_songs(stats);

    mpd_stats_free(stats);

    if (n == 0 && mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        handle_error();
        return 0;
    }

    return n;
}

unsigned Player::getQueueLength() {
    struct mpd_status *status = mpd_run_status(conn);
    unsigned n = mpd_status_get_queue_length(status);

    mpd_status_free(status);

    if (n == 0 && mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        handle_error();
        return 0;
    }

    return n;
}

Player::FapSong Player::getCurrentSong() {
    FapSong fSong;

    fSong.path = "";
    struct mpd_song *song = mpd_run_current_song(conn);

    if (song == NULL)
        return fSong;

    //int status = getStatus();

    QString title = QString(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
    QString artist = QString(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
    QString album = QString(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));

    fSong.title = (title.length() < 1 ? QString(mpd_song_get_uri(song)) : title);
    fSong.artist = (artist.length() < 1 ? "Unknown" : artist);
    fSong.album = (album.length() < 1 ? "Unknown" : album);
    fSong.path = QString(mpd_song_get_uri(song));
    fSong.duration = mpd_song_get_duration(song);
    fSong.pos = (mpd_song_get_pos(song) >= UINT_MAX ? 0 : mpd_song_get_pos(song));

    mpd_song_free(song);

    return fSong;
}

void Player::play() {
    mpd_run_play(conn);
}

void Player::playPause() {
    mpd_run_toggle_pause(conn);
}

void Player::next() {
    int status = getStatus();

    if (status == MPD_STATE_STOP)
        return;

    mpd_run_next(conn);
}

void Player::prev() {
    int status = getStatus();

    if (status == MPD_STATE_STOP)
        return;

    mpd_run_previous(conn);
}

void Player::seek(int time) {
    mpd_run_seek_current(conn, time, false);
}

void Player::playPos(unsigned pos) {
    if (!mpd_run_play_pos(conn, pos) && mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS)
        return handle_error();
}

void Player::pollEvents() {
    int newStatus = getStatus();
    FapSong newSong = getCurrentSong();
    unsigned newElapsedTime = getElapsedTime();
    unsigned newSongCount = getSongCount();
    unsigned newQueueCount = getQueueLength();
    

    QVector<Player::FapSong> newQueueSongList = getQueueSongs();

    if (newStatus != currentStatus) {
        qDebug("MPD_IDLE_PLAYER");
        currentStatus = newStatus;
        emit mpdEvent(MPD_IDLE_PLAYER);
    }

    if (currentSong.path != newSong.path) {
        qDebug("MPD_IDLE_PLAYER (currentSong)");
        currentSong = newSong;
        emit mpdEvent(MPD_IDLE_PLAYER);
    }

    if (newElapsedTime != currentElapsedTime) {
        qDebug("SPECIAL: 2048 (elapsed changed)");
        currentElapsedTime = newElapsedTime;
        emit mpdEvent(FAP_ELAPSED_TIME);
    }

    if (newElapsedTime == currentSong.duration && newElapsedTime != 0) {
        qDebug("SPECIAL: FAP_CURRENT_SONG_END");
        emit mpdEvent(FAP_CURRENT_SONG_END);
    }

    if (newSongCount != currentSongCount) {
        if (newSongCount < 1)
            return;

        qDebug("MPD_IDLE_DATABASE");
        currentSongCount = newSongCount;

        emit mpdEvent(MPD_IDLE_DATABASE);
    }

        if (newQueueCount != currentQueueCount) {
        if (newQueueCount < 1)
            return;

        qDebug("MPD_IDLE_QUEUE");
        currentQueueCount = newQueueCount;

        emit mpdEvent(MPD_IDLE_QUEUE);
    }
}

void Player::update() {
    if (mpd_run_update(conn, NULL) < 1 && mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        qDebug("update: calling handle_error");
        return handle_error();
    }
}


void Player::handle_error() {
    assert(mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS);

    fprintf(stderr, "ERROR: %s\n", mpd_connection_get_error_message(conn));

    bool cleared = mpd_connection_clear_error(conn);

    fprintf(stderr, "Cleared: %d\n", cleared);
    
    if (!cleared) {
        fprintf(stderr, "FATAL: Failed to recover from mpd connection error! Reconnecting...\n");
        mpd_connection_free(conn);
        qDebug("Cleared conn");

        conn = mpd_connection_new(NULL, 0, 300000);
        qDebug("created new conn");
    }
}

void Player::playSong(QString path) {
    int songId = mpd_run_add_id_to(conn, path.toStdString().c_str(), 0);
    if (songId == -1) {
        qDebug("Failed to run add_id_to");
        handle_error();
    }
    else {
        if (!mpd_run_stop(conn) || !mpd_run_play_id(conn, songId))
            qDebug("Failed to stop and play new song");
    }
}

void Player::stopSong() {
    if (!mpd_run_stop(conn) && mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        qDebug("stop Song: calling handle_error");

        return handle_error();
    }
}

void Player::remove(unsigned pos) {
    if (!mpd_run_delete(conn, pos) && mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS) {
        qDebug("remove: calling handle_error");

        return handle_error();
    }
}

void Player::appendToQueue(QString path) {
    if (!mpd_run_add(conn, path.toUtf8().data()))
        return handle_error();
}

void Player::insertIntoQueue(QString path, unsigned pos) {
    if (mpd_run_add_id_to(conn, path.toUtf8().data(), pos) < 0)
        return handle_error();
}

void Player::clearQueue() {
    mpd_run_clear(conn);
}

// Playlists
QStringList Player::getPlaylists() {
    struct mpd_playlist *plist;
    QStringList plists;

    if (!mpd_send_list_playlists(conn))
        handle_error();
    else {
        while ((plist = mpd_recv_playlist(conn)) != NULL) {
            plists << mpd_playlist_get_path(plist);
            mpd_playlist_free(plist);
        }

        if (mpd_connection_get_error(conn) != MPD_ERROR_SUCCESS || !mpd_response_finish(conn))
            handle_error();
    }

    return plists;
}

QVector<Player::FapSong> Player::getPlaylistSongs(QString name) {
    QVector<Player::FapSong> songs;
    struct mpd_entity *entity;
    const struct mpd_song *song;
    Player::FapSong fSong;

    if (!mpd_send_list_playlist_meta(conn, name.toUtf8().data())) {
        qDebug("Failed to send list_playlist_meta");
        handle_error();
    } else {
        while ((entity = mpd_recv_entity(conn)) != NULL) {
            if (mpd_entity_get_type(entity) == MPD_ENTITY_TYPE_SONG) {
                song = mpd_entity_get_song(entity);

                QString title = QString(mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
                QString artist = QString(mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));
                QString album = QString(mpd_song_get_tag(song, MPD_TAG_ALBUM, 0));

                fSong.title = (title.length() < 1 ? QString(mpd_song_get_uri(song)) : title);
                fSong.artist = (artist.length() < 1 ? "Unknown" : artist);
                fSong.album = (album.length() < 1 ? "Unknown" : album);
                fSong.path = QString(mpd_song_get_uri(song));
                fSong.duration = mpd_song_get_duration(song);
                fSong.pos = (mpd_song_get_pos(song) >= UINT_MAX ? 0 : mpd_song_get_pos(song));

                songs << fSong;
            }

            mpd_entity_free(entity);
        }
    }

    return songs;
}

void Player::addToPlaylist(QString pName, QString sPath) {
    if (!mpd_run_playlist_add(conn, pName.toUtf8().data(), sPath.toUtf8().data()))
        return handle_error();
}
