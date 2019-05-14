#pragma once

#include <QDialog>

#include "ui_mpdconf.h"

class MPDConfDialog : public QDialog {
    Q_OBJECT

    public:
        explicit MPDConfDialog(QDialog *parent = nullptr);
        virtual ~MPDConfDialog();
}