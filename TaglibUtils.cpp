#include "TaglibUtils.h"

QPixmap getCover(QString file) {
    QPixmap image;

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
        if (file.endsWith(".mp3")) {
        TagLib::MPEG::File mFile(file.toUtf8().data());
        TagLib::ID3v2::Tag *tag = mFile.ID3v2Tag();

        if (tag == nullptr)
            return false;

        TagLib::ID3v2::FrameList l = tag->frameList("APIC");

        return (!l.isEmpty());

    } else if (file.endsWith(".flac")) {
        TagLib::FLAC::File mFile(file.toUtf8().data());
        TagLib::List<TagLib::FLAC::Picture *> pictures = mFile.pictureList();

        return (!pictures.isEmpty());
    }
}