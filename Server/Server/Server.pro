QT -= gui
QT       += core sql network

CONFIG += c++11 console
CONFIG -= app_bundle

TARGET = Server

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp

RC_ICONS = $$PWD/img/ServerIcon.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#ServerLib connecting
######################################################
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ServerLib/release/ -lServerLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ServerLib/debug/ -lServerLib
else:unix: LIBS += -L$$OUT_PWD/../ServerLib/ -lServerLib

INCLUDEPATH += $$PWD/../ServerLib
DEPENDPATH += $$PWD/../ServerLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ServerLib/release/libServerLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ServerLib/debug/libServerLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ServerLib/release/ServerLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ServerLib/debug/ServerLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../ServerLib/libServerLib.a

#CommonLib connecting
######################################################
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Common/CommonLib/release/ -lCommonLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Common/CommonLib/debug/ -lCommonLib
else:unix: LIBS += -L$$OUT_PWD/../../Common/CommonLib/ -lCommonLib

INCLUDEPATH += $$PWD/../../Common/CommonLib
DEPENDPATH += $$PWD/../../Common/CommonLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Common/CommonLib/release/libCommonLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Common/CommonLib/debug/libCommonLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Common/CommonLib/release/CommonLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Common/CommonLib/debug/CommonLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../Common/CommonLib/libCommonLib.a
