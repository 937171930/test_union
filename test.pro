#-------------------------------------------------
#
# Project created by QtCreator 2020-03-16T01:14:45
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app


SOURCES += main.cpp\
    choosedialog.cpp \
    failuredialog.cpp \
    infodialog.cpp \
    injectdialog.cpp \
        mainwindow.cpp \
    backendwindow.cpp \
    myLineEdit.cpp \
    myModel.cpp \
    mycombobox.cpp \
    udpReceived.cpp

HEADERS  += mainwindow.h \
    backendwindow.h \
    choosedialog.h \
    failuredialog.h \
    infodialog.h \
    injectdialog.h \
    myLineEdit.h \
    myModel.h \
    mycombobox.h \
    udpReceiver.h

FORMS    += mainwindow.ui \
    backendwindow.ui \
    choosedialog.ui \
    failuredialog.ui \
    infodialog.ui \
    injectdialog.ui

DISTFILES += \
    css.qss

RESOURCES += \
    myapp.qrc
