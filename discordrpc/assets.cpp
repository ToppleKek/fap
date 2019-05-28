#include "assets.h"

size_t curlWrite(void *ptr, size_t size, size_t nmemb, std::string *data) {
    data->append((char *) ptr, size * nmemb);
    return size * nmemb;
}

QList<DiscordAsset> dAppGetAssets(QString appid, QSettings *settings) {
    QList<DiscordAsset> items;
    CURL *curl;
    curl = curl_easy_init();

    if (curl) {
        QString url = "https://discordapp.com/api/oauth2/applications/" + appid + "/assets";

        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().data());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "dAppGetAssets: failed curl_easy_perform: %s\n", curl_easy_strerror(res));
        else {
            printf("RESPONSE: %s\n", response.c_str());
            QString qResponse = QString::fromStdString(response);
            QJsonArray json = QJsonDocument::fromJson(qResponse.toUtf8()).array();

            for (int i = 0; i < json.size(); i++) {
                DiscordAsset a;

                a.key = json[i].toObject()["key"].toString();
                a.id = json[i].toObject()["id"].toString();

                items << a;
            }
        }

        curl_easy_cleanup(curl);
    }

    return items;
}

void dAppUploadAsset(QString appid, QString data, QString album, QSettings *settings) {
    settings->setValue("assets/" + album, "unknown");

    QList<DiscordAsset> assets = dAppGetAssets(appid, settings);

    if (assets.size() >= 150) {
        settings->beginGroup("assets");
        QStringList localAssets = settings->allKeys();
        settings->endGroup();

        QString deleteKey;
        int oldest;
        bool ok;
        int n;

        // Find oldest asset
        for (int i = 0; i < localAssets.size(); i++) {
            n = localAssets.at(i).toInt(&ok, 10);
            if (oldest > n && ok)
                oldest = n;
        }

        deleteKey = QString::number(oldest);

        DiscordAsset a;

        a.key = deleteKey;

        // Find id of oldest asset
        for (int i = 0; i < assets.size(); i++) {
            if (assets.at(i).key == deleteKey) {
                a.id = assets.at(i).id;
                break;
            }
        }

        dAppDeleteAsset(appid, a, settings);
    }

    CURL *curl = curl_easy_init();
    CURLcode res;
    QString url = "https://discordapp.com/api/oauth2/applications/" + appid + "/assets";
    QString bearerHeader = "authorization: " + settings->value("discord/token").toString();

    auto t = std::chrono::system_clock::now().time_since_epoch();
    QString key = QString::number(std::chrono::duration_cast<std::chrono::milliseconds>(t).count());

    if (curl) {
        struct curl_slist *headers = nullptr;
        char *postFields = (char *) malloc(key.size() + data.size() + 53);
        sprintf(postFields, "{\"name\":\"%s\",\"type\":1,\"image\":\"data:image/png;base64,%s\"}", key.toUtf8().data(), data.toUtf8().data());

        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, bearerHeader.toUtf8().data());

        curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().data());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "dAppUploadAsset: failed curl_easy_perform: %s\n", curl_easy_strerror(res));
        else
            settings->setValue("assets/" + album, key);

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        free(postFields);
    }
}

void dAppLocalAssets(QSettings *settings) {
    settings->beginGroup("assets");
    QStringList localAssets = settings->childKeys();

    for (int i = 0; i < localAssets.size(); i++)
        qDebug() << localAssets.at(i);

    settings->endGroup();
}

void dAppDeleteAsset(QString appid, DiscordAsset asset, QSettings *settings) {
    CURL *curl = curl_easy_init();
    CURLcode res;
    QString url = "https://discordapp.com/api/oauth2/applications/" + appid + "/assets/" + asset.id;
    QString bearerHeader = "authorization: " + settings->value("discord/token").toString();

    if (curl) {
        struct curl_slist *headers = nullptr;

        headers = curl_slist_append(headers, bearerHeader.toUtf8().data());

        curl_easy_setopt(curl, CURLOPT_URL, url.toUtf8().data());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "dAppDeleteAsset: failed curl_easy_perform: %s\n", curl_easy_strerror(res));
        else {
            QList<DiscordAsset> assets = dAppGetAssets(appid, settings);

            settings->beginGroup("assets");
            QStringList localAssets = settings->childKeys();
            settings->endGroup();

            for (int i = 0; i < localAssets.size(); i++)
                if (settings->value("assets/" + localAssets.at(i)).toString() == asset.key)
                    settings->remove("assets/" + localAssets.at(i));
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
}
