QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20 console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    hashthreadedworker.cpp \
    main.cpp \
    mainwindow.cpp \
    sha256.cpp \
    threadcontroller.cpp

HEADERS += \
    hashthreadedworker.h \
    mainwindow.h \
    sha256.h \
    threadcontroller.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    QtProjectLab1_en_US.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
