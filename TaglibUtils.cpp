#include "TaglibUtils.h"

using namespace TaglibUtils;

QPixmap TaglibUtils::getCover(QString file) {
    TagLib::MPEG::File mFile(file.toUtf8().data());
    TagLib::ID3v2::Tag *tag = mFile.ID3v2Tag();

    TagLib::ID3v2::FrameList l = tag->frameList("APIC");

    QPixmap image;

    if (l.isEmpty())
        return image;

    TagLib::ID3v2::AttachedPictureFrame *f = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(l.front());

    image.loadFromData((const uchar *) f->picture().data(), f->picture().size());

    return image;
}