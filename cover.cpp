#include "cover.h"

bool fileExists(QString path) {
    QFileInfo file(path);

    return file.exists() && file.isFile();
}

QString getDir(QString path) {
    std::string s = path.toStdString();
    std::smatch m;
    std::regex e("^(.+)/");

    if (std::regex_search(s, m, e) && m.size() > 1)
        return QString::fromUtf8(m.str(0).c_str());
    else
        return QString("");
}

QString getCoverFile(QString d) {
    QDir dir(d);
    dir.setFilter(QDir::Files);
    QStringList files = dir.entryList();
    
    std::string s;
    std::smatch m;
    std::regex e("cover\\.(png|jpg|jpeg)", std::regex_constants::icase | std::regex_constants::nosubs);

    for (int i = 0; i < files.size(); i++) {
        s = files.at(i).toStdString();
        if (std::regex_search(s, m, e) && m.size() >= 1)
            return QString::fromUtf8(m.str(0).c_str());
    }

    return QString();
}

QPixmap getCover(QString file) {
    QPixmap image;

    QString dir = getDir(file);
    QString coverFile = getCoverFile(dir);

    if (coverFile != "") {
        image.load(dir + coverFile);
        return image;
    }

    if (file.endsWith(".mp3")) {
        TagLib::MPEG::File mFile(file.toUtf8().data());
        TagLib::ID3v2::Tag *tag = mFile.ID3v2Tag();

        if (tag == nullptr) {
            image.load(":/images/unknown");
            return image;
        }

        TagLib::ID3v2::FrameList l = tag->frameList("APIC");

        if (l.isEmpty()) {
            image.load(":/images/unknown");
            return image;
        }

        TagLib::ID3v2::AttachedPictureFrame *f = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(l.front());

        image.loadFromData((const uchar *) f->picture().data(), f->picture().size());
    } else if (file.endsWith(".flac")) {
        TagLib::FLAC::File mFile(file.toUtf8().data());
        TagLib::List<TagLib::FLAC::Picture *> pictures = mFile.pictureList();

        if (pictures.isEmpty()) {
            image.load(":/images/unknown");
            return image;
        }

        TagLib::ByteVector bv = pictures.front()->data();
        const char *d = bv.data();

        image.loadFromData((const uchar *) d, bv.size());
    }

    return image;
}

bool hasCover(QString file) {
    QString dir = getDir(file);
    QString coverFile = getCoverFile(dir);

    if (file.endsWith(".mp3")) {
        TagLib::MPEG::File mFile(file.toUtf8().data());
        TagLib::ID3v2::Tag *tag = mFile.ID3v2Tag();

        if (tag == nullptr)
            return false;

        TagLib::ID3v2::FrameList l = tag->frameList("APIC");

        if (!l.isEmpty())
            return true;
    } else if (file.endsWith(".flac")) {
        TagLib::FLAC::File mFile(file.toUtf8().data());
        TagLib::List<TagLib::FLAC::Picture *> pictures = mFile.pictureList();

        if (!pictures.isEmpty())
            return true;
    }

    return coverFile != "" && fileExists(dir + coverFile);
}
