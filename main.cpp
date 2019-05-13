#include <QApplication>

#include "fap.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Fap window;

    window.show();

    return app.exec();
}