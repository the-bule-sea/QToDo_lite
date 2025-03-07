QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutwindow.cpp \
    config_manager.cpp \
    data_manager.cpp \
    main.cpp \
    mainwindow.cpp \
    settingwindow.cpp

HEADERS += \
    aboutwindow.h \
    config_manager.h \
    data_manager.h \
    mainwindow.h \
 \#    qLineEdit.h
    settingwindow.h

FORMS += \
    aboutwindow.ui \
    mainwindow.ui \
    settingwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc

RC_ICONS = icon.ico
