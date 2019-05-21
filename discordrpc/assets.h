#include <QString>
#include <QDebug>
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

size_t curlWrite(void *ptr, size_t size, size_t nmemb, std::string *data);
QList<DiscordAsset> dAppGetAssets(QString appid, QSettings *settings);
void dAppUploadAsset(QString appid, QString data, QString album, QSettings *settings);
void testUpload(QString appid, QSettings *settings);