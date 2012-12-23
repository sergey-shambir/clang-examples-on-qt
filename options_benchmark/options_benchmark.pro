TEMPLATE = app
CONFIG += console
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x
LIBS += -lclang -lboost_timer -lboost_system

SOURCES += main.cpp \
    directorywalker.cpp \
    actions.cpp

HEADERS += \
    directorywalker.h \
    actions.h

