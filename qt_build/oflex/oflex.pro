#-------------------------------------------------
#
# Project created by QtCreator 2022-01-09T10:59:20
#
#-------------------------------------------------

QT       += core

QT       -= gui

CONFIG += C++11


TARGET = oflex
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += QT_BUILD

INCLUDEPATH += ../../src/

SOURCES += \
    ../../src/dot_generator.cpp \
    ../../src/main.cpp \
    ../../src/NFAState.cpp \
    ../../src/ORegexList.cpp \
    ../../src/ORegexParse.cpp \
    main_qt.cpp \
    ../../src/nfaconvert.cpp \
    lexfileparse.cpp

HEADERS += \
    ../../src/dot_generator.h \
    ../../src/NFAState.h \
    ../../src/ORegexList.h \
    ../../src/ORegexParse.h \
    ../../src/nfaconvert.h \
    lexfileparse.h
