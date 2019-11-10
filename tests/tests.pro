TEMPLATE = app

QT -= core widgets

TARGET = necro-test

isEmpty(SHV_PROJECT_TOP_BUILDDIR) {
	SHV_PROJECT_TOP_BUILDDIR=$$shadowed($$PWD)/..
}
message ( SHV_PROJECT_TOP_BUILDDIR: '$$SHV_PROJECT_TOP_BUILDDIR' )

DESTDIR = $$SHV_PROJECT_TOP_BUILDDIR/bin

unix:LIBDIR = $$SHV_PROJECT_TOP_BUILDDIR/lib
win32:LIBDIR = $$SHV_PROJECT_TOP_BUILDDIR/bin

LIBS += \
    -L$$LIBDIR \
    -lnecrolog

INCLUDEPATH += \
	../include

LIBS += \
    -L$$shadowed($$PWD)/../lib \
    -lnecrolog

SOURCES += \
	necro-test.cpp \

HEADERS += \

