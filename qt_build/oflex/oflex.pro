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
    ../../src/flex_sample1.cpp \
    ../../src/main.cpp \
    ../../src/NFAState.cpp \
    ../../src/ORegexParse.cpp \
    ../../src/regex_match.cpp \
    ../../src/util/num2str.cpp \
    main_qt.cpp \
    ../../src/nfaconvert.cpp \
    ../../src/lexfileparse.cpp \
    ../../src/util/trim.cpp \
    ../../src/util/string_eval.cpp \
    ../../src/util/str2number.cpp \
    ../../src/util/split.cpp \
    ../../src/util/replace.cpp \
    ../../src/util/mylog.cpp \
    ../../src/fsa_to_dot.cpp

HEADERS += \
    ../../src/NFAState.h \
    ../../src/ORegexParse.h \
    ../../src/flex_sample1.h \
    ../../src/nfaconvert.h \
    ../../src/regex_match.h \
    ../../src/util/flex_sample1_temp.inc \
    ../../src/util/num2str.h \
    ../../src/lexfileparse.h \
    ../../test/quut_com_ansi_c.l \
    ../../src/util/trim.h \
    ../../src/util/string_eval.h \
    ../../src/util/str2number.h \
    ../../src/util/split.h \
    ../../src/util/replace.h \
    ../../src/util/mylog.h \
    ../../src/fsa_to_dot.h \
    ../../src/util/argv.h

OTHER_FILES += \
    ../../test/CMakeLists.txt \
    ../../README.md
