#include "fapconf.h"

FAPConfDialog::FAPConfDialog(QDialog *parent) : QDialog(parent) {
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}
/*
FAPConfDialog::~FAPConfDialog() {
    qDebug("Destructing dialog");
}
*/
bool FAPConfDialog::getDRPCEnabled() {
    return ui.dRPCCheckBox->isChecked();
}

QString FAPConfDialog::getDRPCAppID() {
    return ui.dRPCIDLineEdit->text();
}

QString FAPConfDialog::getDRPCToken() {
    return ui.dRPCTokenLineEdit->text();
}
