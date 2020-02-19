TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../served/lib/release/ -lserved
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../served/lib/debug/ -lserved
else:unix: LIBS += -L$$PWD/../served/lib/ -lserved


INCLUDEPATH += $$PWD/../served/src
DEPENDPATH += $$PWD/../served/src

LIBS += -lpthread -lboost_system
