#include <QString>
#include <QVector>
#include <QStringList>
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <curl/curl.h>
#include <cstddef>
#include <chrono>

struct DiscordAsset {
    QString key;
    QString id;
};

typedef struct DiscordAsset DiscordAsset;

static const QStringList RESERVED_ASSETS = { "unknown", "play", "pause", "stop" };

size_t curlWrite(void *ptr, size_t size, size_t nmemb, std::string *data);
QVector<DiscordAsset> dAppGetAssets(QString appid, QSettings *settings);
void dAppUploadAsset(QString appid, QString data, QString album, QSettings *settings);
void dAppDeleteAsset(QString appid, DiscordAsset asset, QSettings *settings);
void dAppSyncConfig(QString appid, QSettings *settings);
