QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#CONFIG  += c++17
QMAKE_CXXFLAGS += /std:c++latest
#msvc:{
#    QMAKE_CXXFLAGS += /std:c++17
#}

#gcc:{
#     QMAKE_CXXFLAGS += -std=c++1z
#}

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
DEFINES += _USE_MATH_DEFINES=1

SOURCES += \
    arcprogressbar.cpp \
    graphicsview.cpp \
    lcd.cpp \
    main.cpp \
    mainwindow.cpp \
    test.c \
    testwindow.cpp

HEADERS += \
    arcprogressbar.h \
    circle.h \
    graphicsview.h \
    lcd.h \
    line.h \
    mainwindow.h \
    point.h \
    test.h \
    testwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#include(../ST7789-STM32-uGUI.pri)
