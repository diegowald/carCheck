TEMPLATE = app
CONFIG += console c++11
CONFIG += openssl-linked
CONFIG -= app_bundle

QT       += network

SOURCES += \
        dispatch.cpp \
        main.cpp



win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../served/lib/release/ -lserved
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../served/lib/debug/ -lserved
else:unix: LIBS += -L$$PWD/../served/lib/ -lserved


INCLUDEPATH += $$PWD/../served/src
DEPENDPATH += $$PWD/../served/src

# For asio
LIBS += -lpthread -lboost_system

HEADERS += \
  dispatch.h
