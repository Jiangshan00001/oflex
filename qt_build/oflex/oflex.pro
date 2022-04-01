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

INCLUDEPATH += ../../src/ ../../src/util

SOURCES += \
    ../../src/dot_generator.cpp \
    ../../src/main.cpp \
    ../../src/NFAState.cpp \
    ../../src/ORegexList.cpp \
    ../../src/ORegexParse.cpp \
    ../../src/util/num2str.cpp \
    main_qt.cpp \
    ../../src/nfaconvert.cpp \
    lexfileparse.cpp \
    ../../src/util/trim.cpp \
    ../../src/util/string_eval.cpp \
    ../../src/util/str2number.cpp \
    ../../src/util/split.cpp \
    ../../src/util/replace.cpp \
    ../../src/util/mylog.cpp

HEADERS += \
    ../../src/dot_generator.h \
    ../../src/NFAState.h \
    ../../src/ORegexList.h \
    ../../src/ORegexParse.h \
    ../../src/nfaconvert.h \
    ../../src/util/num2str.h \
    lexfileparse.h \
    ../../test/quut_com_ansi_c.l \
    ../../src/util/trim.h \
    ../../src/util/string_eval.h \
    ../../src/util/str2number.h \
    ../../src/util/split.h \
    ../../src/util/replace.h \
    ../../src/util/mylog.h
