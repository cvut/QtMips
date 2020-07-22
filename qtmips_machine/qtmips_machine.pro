QT -= gui

TARGET = qtmips_machine
CONFIG += c++11

TEMPLATE = lib
CONFIG += staticlib

LIBS += -lelf
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_CXXFLAGS_DEBUG += -ggdb

DEFINES += QTMIPS_MACHINE_LIBRARY
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    qtmipsmachine.cpp \
    qtmipsexception.cpp \
    core.cpp \
    instruction.cpp \
    registers.cpp \
    programloader.cpp \
    alu.cpp \
    machineconfig.cpp \
    utils.cpp \
    symboltable.cpp \
    cop0state.cpp \
    memory/cache.cpp \
    memory/mmu.cpp \
    memory/frontend_memory.cpp \
    memory/backend/memory.cpp \
    memory/backend/peripspiled.cpp \
    memory/backend/lcddisplay.cpp \
    memory/backend/peripheral.cpp \
    memory/backend/serialport.cpp

HEADERS += \
    qtmipsmachine.h \
    qtmipsexception.h \
    core.h \
    instruction.h \
    registers.h \
    programloader.h \
    alu.h \
    machineconfig.h \
    utils.h \
    machinedefs.h \
    symboltable.h \
    cop0state.h \
    memory/address.h \
    memory/cache.h \
    memory/mmu.h \
    memory/access_size.h \
    memory/frontend_memory.h \
    memory/backend/memory.h \
    memory/backend/peripspiled.h \
    memory/backend/lcddisplay.h \
    memory/backend/peripheral.h \
    memory/backend/serialport.h \
    memory/backend/backend_memory.h
