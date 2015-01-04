#-------------------------------------------------
#
# Project created by QtCreator 2014-12-08T19:54:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = guidedog
TEMPLATE = app


SOURCES += main.cpp\
        guidedog.cpp \
    simelineedit.cpp \
    guidedogdoc.cpp \
    commandrunner.cpp \
    iprange.cpp \
    aboutdialog.cpp

HEADERS  += guidedog.h \
    simelineedit.h \
    commandrunner.h \
    iprange.h \
    guidedogdoc.h \
    aboutdialog.h

FORMS += \
    guidedogwindow.ui \
    aboutdialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    AUTHORS \
    COPYING \
    INSTALL \
    README \
    ChangeLog
