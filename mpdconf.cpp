#include "mpdconf.h"

MPDConfDialog::MPDConfDialog(QDialog *parent) : QDialog(parent) {
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    
    qDebug("Opened dialog");
}

MPDConfDialog::~MPDConfDialog() {
    qDebug("Destructing dialog");
}

QString MPDConfDialog::getHost() {
    return ui.hostLineEdit->text();
}

unsigned MPDConfDialog::getPort() {
    bool ok;
    unsigned port = ui.portLineEdit->text().toUInt(&ok, 10);

    return ok ? port : 0;
}

QString MPDConfDialog::getMusicDir() {
    return ui.dirLineEdit->text();
}