TARGET = MessengerLibTest
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        test.cpp

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

#GoogleTest connecting
######################################################
unix:!macx|win32: LIBS += -L$$PWD/../../GoogleTest/ -lgtest

INCLUDEPATH += $$PWD/../../GoogleTest/googletest/include
DEPENDPATH += $$PWD/../../GoogleTest/googletest/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../GoogleTest/gtest.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/../../GoogleTest/libgtest.a
