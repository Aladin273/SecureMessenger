QT       += core gui widgets sql network

TARGET = Messenger
CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    userwidgets.cpp

HEADERS += \
    mainwindow.h \
    userwidgets.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

RC_ICONS = $$PWD/img/icon_final.ico

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#MessengerLib connecting
######################################################
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../MessengerLib/release/ -lMessengerLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../MessengerLib/debug/ -lMessengerLib
else:unix: LIBS += -L$$OUT_PWD/../MessengerLib/ -lMessengerLib

INCLUDEPATH += $$PWD/../MessengerLib
DEPENDPATH += $$PWD/../MessengerLib


win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MessengerLib/release/libMessengerLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MessengerLib/debug/libMessengerLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MessengerLib/release/MessengerLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../MessengerLib/debug/MessengerLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../MessengerLib/libMessengerLib.a

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
