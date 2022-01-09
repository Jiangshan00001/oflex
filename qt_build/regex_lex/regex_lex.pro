#-------------------------------------------------
#
# Project created by QtCreator 2022-01-09T10:59:20
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = regex_lex
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += QT_BUILD DEBUG_ALL

INCLUDEPATH += ../../src/

SOURCES += \
    ../../src/dot_generator.cpp \
    ../../src/main.cpp \
    ../../src/NFAState.cpp \
    ../../src/ORegexList.cpp \
    ../../src/ORegexParse.cpp \
    main_qt.cpp \
    ../../src/nfaconvert.cpp

HEADERS += \
    ../../src/dot_generator.h \
    ../../src/NFAState.h \
    ../../src/ORegexList.h \
    ../../src/ORegexParse.h \
    ../../src/nfaconvert.h
