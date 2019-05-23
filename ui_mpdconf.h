/********************************************************************************
** Form generated from reading UI file 'mpdconf.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MPDCONF_H
#define UI_MPDCONF_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MPDConfDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *infoLabel;
    QLabel *hostLabel;
    QLineEdit *hostLineEdit;
    QLabel *portLabel;
    QLineEdit *portLineEdit;
    QLabel *dirLabel;
    QLineEdit *dirLineEdit;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *MPDConfDialog)
    {
        if (MPDConfDialog->objectName().isEmpty())
            MPDConfDialog->setObjectName(QStringLiteral("MPDConfDialog"));
        MPDConfDialog->resize(400, 300);
        verticalLayout = new QVBoxLayout(MPDConfDialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        infoLabel = new QLabel(MPDConfDialog);
        infoLabel->setObjectName(QStringLiteral("infoLabel"));
        QFont font;
        font.setFamily(QStringLiteral("DejaVu Sans"));
        font.setPointSize(12);
        infoLabel->setFont(font);

        verticalLayout->addWidget(infoLabel);

        hostLabel = new QLabel(MPDConfDialog);
        hostLabel->setObjectName(QStringLiteral("hostLabel"));

        verticalLayout->addWidget(hostLabel);

        hostLineEdit = new QLineEdit(MPDConfDialog);
        hostLineEdit->setObjectName(QStringLiteral("hostLineEdit"));

        verticalLayout->addWidget(hostLineEdit);

        portLabel = new QLabel(MPDConfDialog);
        portLabel->setObjectName(QStringLiteral("portLabel"));

        verticalLayout->addWidget(portLabel);

        portLineEdit = new QLineEdit(MPDConfDialog);
        portLineEdit->setObjectName(QStringLiteral("portLineEdit"));

        verticalLayout->addWidget(portLineEdit);

        dirLabel = new QLabel(MPDConfDialog);
        dirLabel->setObjectName(QStringLiteral("dirLabel"));

        verticalLayout->addWidget(dirLabel);

        dirLineEdit = new QLineEdit(MPDConfDialog);
        dirLineEdit->setObjectName(QStringLiteral("dirLineEdit"));

        verticalLayout->addWidget(dirLineEdit);

        buttonBox = new QDialogButtonBox(MPDConfDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(MPDConfDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), MPDConfDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), MPDConfDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(MPDConfDialog);
    } // setupUi

    void retranslateUi(QDialog *MPDConfDialog)
    {
        MPDConfDialog->setWindowTitle(QApplication::translate("MPDConfDialog", "MPD Server Configuration", nullptr));
        infoLabel->setText(QApplication::translate("MPDConfDialog", "MPD Server Configuration", nullptr));
        hostLabel->setText(QApplication::translate("MPDConfDialog", "Host name, IP address or Unix socket of the MPD Server:", nullptr));
        portLabel->setText(QApplication::translate("MPDConfDialog", "Port:", nullptr));
#ifndef QT_NO_TOOLTIP
        dirLabel->setToolTip(QApplication::translate("MPDConfDialog", "If the MPD server is not local (not using a Unix socket), the Music directory cannot be determined automatically.", nullptr));
#endif // QT_NO_TOOLTIP
        dirLabel->setText(QApplication::translate("MPDConfDialog", "MPD Music Directory (For \"remote\" servers)", nullptr));
#ifndef QT_NO_TOOLTIP
        dirLineEdit->setToolTip(QApplication::translate("MPDConfDialog", "If the MPD server is not local (not using a Unix socket), the Music directory cannot be determined automatically.", nullptr));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class MPDConfDialog: public Ui_MPDConfDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MPDCONF_H
