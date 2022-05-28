QT += widgets

QMAKE_CXX = clang++

CONFIG += c++14

DSG = ../DeSiGNAR

INCLUDEPATH += $${DSG}/include ../

LIBS += -L$${DSG}/lib -lDesignar

HEADERS = \
    ../Definitions.h \
    ../Geom.h \
    ../Pathfinder.h \
    canvas.H

SOURCES += \
    ../Definitions.cpp \
    canvas.C \
    main.C

