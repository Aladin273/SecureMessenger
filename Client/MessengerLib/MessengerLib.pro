QT += sql network
QT -= gui

TARGET = MessengerLib
TEMPLATE = lib
CONFIG += staticlib
CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    client.cpp \
    clientdatabase.cpp \
    clientsocket.cpp

HEADERS += \
    client.h \
    clientdatabase.h \
    clientsocket.h \
    common.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

#CommonLib connecting
####################################################
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Common/CommonLib/release/ -lCommonLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Common/CommonLib/debug/ -lCommonLib
else:unix:!macx: LIBS += -L$$OUT_PWD/../../Common/CommonLib/ -lCommonLib

INCLUDEPATH += $$PWD/../../Common/CommonLib
DEPENDPATH += $$PWD/../../Common/CommonLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Common/CommonLib/release/libCommonLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Common/CommonLib/debug/libCommonLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Common/CommonLib/release/CommonLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Common/CommonLib/debug/CommonLib.lib
else:unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../../Common/CommonLib/libCommonLib.a
