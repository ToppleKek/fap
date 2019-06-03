#pragma once

#include <QDialog>
#include <QDebug>

#include "ui_fapconf.h"

class FAPConfDialog : public QDialog {
    Q_OBJECT

    public:
        explicit FAPConfDialog(QDialog *parent = nullptr);
        virtual ~FAPConfDialog() {};

        bool getDRPCEnabled();
        QString getDRPCAppID();
        QString getDRPCToken();

    private:
        Ui::FAPConfDialog ui;
};
