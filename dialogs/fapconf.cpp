#include "fapconf.h"

FAPConfDialog::FAPConfDialog(QDialog *parent, QSettings *s) : QDialog(parent), settings(s) {
    ui.setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    ui.dRPCCheckBox->setCheckState(settings->value("discord/enabled").toBool() ? Qt::Checked : Qt::Unchecked);
    ui.dRPCIDLineEdit->setText(settings->value("discord/appid").toString());
    ui.dRPCTokenLineEdit->setText(settings->value("discord/token").toString());
}

FAPConfDialog::~FAPConfDialog() {
    qDebug("Destructing dialog");
}

bool FAPConfDialog::getDRPCEnabled() {
    return ui.dRPCCheckBox->isChecked();
}

QString FAPConfDialog::getDRPCAppID() {
    return ui.dRPCIDLineEdit->text();
}

QString FAPConfDialog::getDRPCToken() {
    return ui.dRPCTokenLineEdit->text();
}

void FAPConfDialog::setDRPCEnabled(bool enabled) {
    ui.dRPCCheckBox->setCheckState(Qt::Checked);
}

void FAPConfDialog::setDRPCAppID(QString appid) {
    ui.dRPCIDLineEdit->setText(appid);
}

void FAPConfDialog::setDRPCToken(QString token) {
    ui.dRPCTokenLineEdit->setText(token);
}
