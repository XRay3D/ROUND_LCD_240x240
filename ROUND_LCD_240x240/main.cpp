#include "mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {

    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Unset);
//    QApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    qputenv("QT_ENABLE_HIGHDPI_SCALING", QByteArray("0"));
    qputenv("QT_SCREEN_SCALE_FACTORS", QByteArray("1;1;1"));

    QApplication::setOrganizationName("XrSoft");
    QApplication::setApplicationName("ROUND_LCD_240x240");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
