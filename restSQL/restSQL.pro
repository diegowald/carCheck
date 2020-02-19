TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        crudbase.cpp \
        main.cpp

HEADERS += \
  crudbase.h


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../served/lib/release/ -lserved
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../served/lib/debug/ -lserved
else:unix: LIBS += -L$$PWD/../served/lib/ -lserved


INCLUDEPATH += $$PWD/../served/src
DEPENDPATH += $$PWD/../served/src

# For asio
LIBS += -lpthread -lboost_system

# For MYSQL
LIBS += -lmysqlcppconn
