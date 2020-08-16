TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lpthread

SOURCES += \
        main.cpp \
        src/tt.authmanager.cpp \
        src/tt.client.cpp \
        src/tt.historymanager.cpp \
        src/tt.server.cpp

HEADERS += \
    src/tt.authmanager.h \
    src/tt.client.h \
    src/tt.helpers.h \
    src/tt.historymanager.h \
    src/tt.server.h
