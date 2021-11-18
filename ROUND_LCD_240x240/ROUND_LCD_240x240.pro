QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += /std:c++latest

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += _USE_MATH_DEFINES=1

SOURCES += \
    arcprogressbar.cpp \
    lcd.cpp \
    main.cpp \
    mainwindow.cpp \
    test.c

HEADERS += \
    arcprogressbar.h \
    circle.h \
    lcd.h \
    line.h \
    mainwindow.h \
    point.h \
    test.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include(../ST7789-STM32-uGUI.pri)