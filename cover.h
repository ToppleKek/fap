#pragma once

#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>
#include <taglib/id3v2header.h>
#include <taglib/id3v1tag.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/flacfile.h>
#include <taglib/flacpicture.h>
#include <taglib/tlist.h>

#include <QPixmap>
#include <QString>
#include <QFileInfo>

#include <regex>

bool fileExists(QString path);
QString getDir(QString path);
QPixmap getCover(QString file);
bool hasCover(QString file);
