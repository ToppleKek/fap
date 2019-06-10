#pragma once

#include <QDialog>
#include <QSettings>
#include <QDebug>

#include "ui_fapconf.h"

class FAPConfDialog : public QDialog {
    Q_OBJECT

    public:
        explicit FAPConfDialog(QDialog *parent = nullptr, QSettings *s = nullptr) ;
        ~FAPConfDialog();
        
        Ui::FAPConfDialog ui;
        
        bool getDRPCEnabled();
        QString getDRPCAppID();
        QString getDRPCToken();
        
        void setDRPCEnabled(bool enabled);
        void setDRPCAppID(QString appid);
        void setDRPCToken(QString token);

    private:
        QSettings *settings;
};
