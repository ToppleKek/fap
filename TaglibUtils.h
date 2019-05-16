#pragma once

#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v1tag.h>
#include <taglib/attachedpictureframe.h>

#include <QPixmap>
#include <QString>

namespace TaglibUtils {
    QPixmap getCover(QString file);
}