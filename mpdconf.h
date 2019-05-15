#pragma once

#include <QDialog>
#include <QDebug>

#include "ui_mpdconf.h"

class MPDConfDialog : public QDialog {
    Q_OBJECT

    public:
        explicit MPDConfDialog(QDialog *parent = nullptr);
        ~MPDConfDialog();

        QString getHost();
        unsigned getPort();
        QString getMusicDir();

    private:
        Ui::MPDConfDialog ui;
};