# We need this hack until quickbox
# will not be split to 2 projects (quickevent + libqf)
SHV_PROJECT_TOP_BUILDDIR=$$shadowed($$PWD)/../../..
SHV_PROJECT_TOP_SRCDIR=$$PWD/../../..

QT -= core gui

CONFIG += C++11
CONFIG += hide_symbols

TEMPLATE = lib
TARGET = necrolog

unix:DESTDIR = $$SHV_PROJECT_TOP_BUILDDIR/lib
win32:DESTDIR = $$SHV_PROJECT_TOP_BUILDDIR/bin

message ( DESTDIR: $$DESTDIR )

DEFINES += NECROLOG_BUILD_DLL

SOURCES += \
    necrolog.cpp

HEADERS += \
    necrolog.h \
    necrologlevel.h \
    necrologglobal.h

