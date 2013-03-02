TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++0x
LIBS += -lclang

SOURCES += main.cpp

HEADERS += \
    scopedclangoptions.h

SOURCES += \
    scopedclangoptions.cpp

HEADERS += \
    generator.h

SOURCES += \
    generator.cpp

HEADERS += \
    generators/headergen.h

SOURCES += \
    generators/headergen.cpp

HEADERS += \
    constants.h

HEADERS += \
    generators/pwdgen.h

SOURCES += \
    generators/pwdgen.cpp

HEADERS += \
    generators/visitorgen.h

SOURCES += \
    generators/visitorgen.cpp

HEADERS += \
    generators/implementationgen.h

SOURCES += \
    generators/implementationgen.cpp

HEADERS += \
    generators/astkindsgen.h

SOURCES += \
    generators/astkindsgen.cpp

HEADERS += \
    generators/fullvisitorgen.h

SOURCES += \
    generators/fullvisitorgen.cpp

