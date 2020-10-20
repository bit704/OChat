QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addfriend.cpp \
    chatui.cpp \
    customtabstyle.cpp \
    main.cpp \
    login.cpp \
    maininterface.cpp \
    portrait.cpp \
    pwordchange.cpp \
    register.cpp

HEADERS += \
    addfriend.h \
    chatui.h \
    customtabstyle.h \
    login.h \
    maininterface.h \
    portrait.h \
    pwordchange.h \
    register.h

FORMS += \
    addfriend.ui \
    chatui.ui \
    login.ui \
    maininterface.ui \
    portrait.ui \
    pwordchange.ui \
    register.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Photo.qrc
