TEMPLATE = app
TARGET = fap
INCLUDEPATH += . ./discordrpc/include/
LIBS += -lmpdclient -ldiscord-rpc -L./discordrpc/lib/ -ltag -lcurl
QT += core gui widgets
QMAKE_LFLAGS += -Wl,-rpath,./discordrpc/lib/

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += fap.h player.h cover.h discordrpc/assets.h dialogs/mpdconf.h dialogs/fapconf.h tabs/songtab.h tabs/playlisttab.h tabs/foldertab.h
SOURCES += main.cpp fap.cpp player.cpp dialogs/mpdconf.cpp dialogs/fapconf.cpp cover.cpp discordrpc/assets.cpp tabs/songtab.cpp tabs/playlisttab.cpp tabs/foldertab.cpp
FORMS += forms/main.ui forms/mpdconf.ui forms/fapconf.ui forms/about.ui
RESOURCES += resources/res.qrc
