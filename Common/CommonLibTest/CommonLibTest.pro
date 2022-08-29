TARGET = CommonLibTest
TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += test.cpp


#CommonLib connecting
######################################################
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../CommonLib/release/ -lCommonLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../CommonLib/debug/ -lCommonLib
else:unix:!macx: LIBS += -L$$OUT_PWD/../CommonLib/ -lCommonLib

INCLUDEPATH += $$PWD/../CommonLib
DEPENDPATH += $$PWD/../CommonLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../CommonLib/release/libCommonLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../CommonLib/debug/libCommonLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../CommonLib/release/CommonLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../CommonLib/debug/CommonLib.lib
else:unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../CommonLib/libCommonLib.a

#GoogleTest connecting
######################################################
unix:!macx|win32: LIBS += -L$$PWD/../../GoogleTest/ -lgtest

INCLUDEPATH += $$PWD/../../GoogleTest/googletest/include
DEPENDPATH += $$PWD/../../GoogleTest/googletest/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../GoogleTest/gtest.lib
else:unix:!macx|win32-g++: PRE_TARGETDEPS += $$PWD/../../GoogleTest/libgtest.a
