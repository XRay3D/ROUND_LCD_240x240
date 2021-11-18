#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication::setOrganizationName("XrSoft");
    QApplication::setApplicationName("ROUND_LCD_240x240");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
