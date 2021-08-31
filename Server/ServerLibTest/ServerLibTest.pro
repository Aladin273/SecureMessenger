TARGET = ServerLibTest
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        test.cpp

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

#GoogleTest connecting
######################################################
unix:!macx|win32: LIBS += -L$$PWD/../../GoogleTest/ -lgtest

INCLUDEPATH += $$PWD/../../GoogleTest/googletest/include
DEPENDPATH += $$PWD/../../GoogleTest/googletest/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../GoogleTest/gtest.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/../../GoogleTest/libgtest.a
