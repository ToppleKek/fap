/********************************************************************************
** Form generated from reading UI file 'main.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_H
#define UI_MAIN_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Fap
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QTabWidget *tabWidget;
    QWidget *songsTab;
    QHBoxLayout *horizontalLayout;
    QTreeWidget *songTree;
    QTreeWidget *queueTree;
    QWidget *artistsTab;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *prevButton;
    QPushButton *playPauseButton;
    QPushButton *nextButton;
    QPushButton *stopButton;
    QSlider *seekSlider;
    QLabel *timeLabel;
    QHBoxLayout *horizontalLayout_2;
    QLabel *coverLabel;
    QVBoxLayout *verticalLayout;
    QLabel *titleArtistLabel;
    QLabel *albumLabel;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *Fap)
    {
        if (Fap->objectName().isEmpty())
            Fap->setObjectName(QStringLiteral("Fap"));
        Fap->resize(1128, 840);
        centralwidget = new QWidget(Fap);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        songsTab = new QWidget();
        songsTab->setObjectName(QStringLiteral("songsTab"));
        horizontalLayout = new QHBoxLayout(songsTab);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        songTree = new QTreeWidget(songsTab);
        songTree->setObjectName(QStringLiteral("songTree"));
        songTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
        songTree->setProperty("showDropIndicator", QVariant(false));
        songTree->setItemsExpandable(false);
        songTree->setExpandsOnDoubleClick(false);
        songTree->setColumnCount(4);

        horizontalLayout->addWidget(songTree);

        queueTree = new QTreeWidget(songsTab);
        queueTree->setObjectName(QStringLiteral("queueTree"));
        queueTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
        queueTree->setProperty("showDropIndicator", QVariant(false));
        queueTree->setItemsExpandable(false);
        queueTree->setExpandsOnDoubleClick(false);
        queueTree->setColumnCount(4);

        horizontalLayout->addWidget(queueTree);

        tabWidget->addTab(songsTab, QString());
        artistsTab = new QWidget();
        artistsTab->setObjectName(QStringLiteral("artistsTab"));
        tabWidget->addTab(artistsTab, QString());

        verticalLayout_2->addWidget(tabWidget);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        prevButton = new QPushButton(centralwidget);
        prevButton->setObjectName(QStringLiteral("prevButton"));
        prevButton->setMaximumSize(QSize(25, 16777215));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/prev-button.png"), QSize(), QIcon::Normal, QIcon::Off);
        prevButton->setIcon(icon);
        prevButton->setFlat(true);

        horizontalLayout_3->addWidget(prevButton);

        playPauseButton = new QPushButton(centralwidget);
        playPauseButton->setObjectName(QStringLiteral("playPauseButton"));
        playPauseButton->setMaximumSize(QSize(25, 16777215));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/images/play-button.png"), QSize(), QIcon::Normal, QIcon::On);
        playPauseButton->setIcon(icon1);
        playPauseButton->setAutoDefault(false);
        playPauseButton->setFlat(true);

        horizontalLayout_3->addWidget(playPauseButton);

        nextButton = new QPushButton(centralwidget);
        nextButton->setObjectName(QStringLiteral("nextButton"));
        nextButton->setMaximumSize(QSize(25, 16777215));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/images/next-button.png"), QSize(), QIcon::Normal, QIcon::Off);
        nextButton->setIcon(icon2);
        nextButton->setFlat(true);

        horizontalLayout_3->addWidget(nextButton);

        stopButton = new QPushButton(centralwidget);
        stopButton->setObjectName(QStringLiteral("stopButton"));
        stopButton->setMaximumSize(QSize(25, 16777215));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/images/stop-button.png"), QSize(), QIcon::Normal, QIcon::On);
        stopButton->setIcon(icon3);
        stopButton->setFlat(true);

        horizontalLayout_3->addWidget(stopButton);

        seekSlider = new QSlider(centralwidget);
        seekSlider->setObjectName(QStringLiteral("seekSlider"));
        seekSlider->setMaximumSize(QSize(500, 16777215));
        seekSlider->setOrientation(Qt::Horizontal);
        seekSlider->setInvertedAppearance(false);
        seekSlider->setInvertedControls(false);

        horizontalLayout_3->addWidget(seekSlider);

        timeLabel = new QLabel(centralwidget);
        timeLabel->setObjectName(QStringLiteral("timeLabel"));
        timeLabel->setMaximumSize(QSize(100, 16777215));

        horizontalLayout_3->addWidget(timeLabel);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(15);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        coverLabel = new QLabel(centralwidget);
        coverLabel->setObjectName(QStringLiteral("coverLabel"));
        coverLabel->setMinimumSize(QSize(100, 100));
        coverLabel->setMaximumSize(QSize(100, 100));
        coverLabel->setPixmap(QPixmap(QString::fromUtf8(":/images/unknown.png")));
        coverLabel->setScaledContents(false);

        horizontalLayout_2->addWidget(coverLabel);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
        titleArtistLabel = new QLabel(centralwidget);
        titleArtistLabel->setObjectName(QStringLiteral("titleArtistLabel"));
        QFont font;
        font.setPointSize(14);
        font.setBold(false);
        font.setWeight(50);
        titleArtistLabel->setFont(font);

        verticalLayout->addWidget(titleArtistLabel);

        albumLabel = new QLabel(centralwidget);
        albumLabel->setObjectName(QStringLiteral("albumLabel"));
        QFont font1;
        font1.setPointSize(12);
        albumLabel->setFont(font1);

        verticalLayout->addWidget(albumLabel);


        horizontalLayout_2->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout_2);

        Fap->setCentralWidget(centralwidget);
        menubar = new QMenuBar(Fap);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1128, 28));
        Fap->setMenuBar(menubar);
        statusbar = new QStatusBar(Fap);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        Fap->setStatusBar(statusbar);

        retranslateUi(Fap);

        tabWidget->setCurrentIndex(0);
        playPauseButton->setDefault(false);


        QMetaObject::connectSlotsByName(Fap);
    } // setupUi

    void retranslateUi(QMainWindow *Fap)
    {
        Fap->setWindowTitle(QApplication::translate("Fap", "Fantastic Audio Player", nullptr));
#ifndef QT_NO_TOOLTIP
        tabWidget->setToolTip(QApplication::translate("Fap", "<html><head/><body><p>Bruh</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        QTreeWidgetItem *___qtreewidgetitem = songTree->headerItem();
        ___qtreewidgetitem->setText(3, QApplication::translate("Fap", "ID", nullptr));
        ___qtreewidgetitem->setText(2, QApplication::translate("Fap", "Album", nullptr));
        ___qtreewidgetitem->setText(1, QApplication::translate("Fap", "Artist", nullptr));
        ___qtreewidgetitem->setText(0, QApplication::translate("Fap", "Title", nullptr));
        QTreeWidgetItem *___qtreewidgetitem1 = queueTree->headerItem();
        ___qtreewidgetitem1->setText(3, QApplication::translate("Fap", "ID", nullptr));
        ___qtreewidgetitem1->setText(2, QApplication::translate("Fap", "Album", nullptr));
        ___qtreewidgetitem1->setText(1, QApplication::translate("Fap", "Artist", nullptr));
        ___qtreewidgetitem1->setText(0, QApplication::translate("Fap", "Title", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(songsTab), QApplication::translate("Fap", "Songs", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(artistsTab), QApplication::translate("Fap", "Artists", nullptr));
        prevButton->setText(QString());
        playPauseButton->setText(QString());
        nextButton->setText(QString());
        stopButton->setText(QString());
        timeLabel->setText(QApplication::translate("Fap", "0:00/0:00", nullptr));
        coverLabel->setText(QString());
        titleArtistLabel->setText(QApplication::translate("Fap", "Not Playing", nullptr));
        albumLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Fap: public Ui_Fap {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_H
