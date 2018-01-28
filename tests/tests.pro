TEMPLATE = app

QT -= core widgets

isEmpty(SHV_PROJECT_TOP_BUILDDIR) {
	SHV_PROJECT_TOP_BUILDDIR=$$shadowed($$PWD)/..
}
message ( SHV_PROJECT_TOP_BUILDDIR: '$$SHV_PROJECT_TOP_BUILDDIR' )

unix:DESTDIR = $$SHV_PROJECT_TOP_BUILDDIR/lib
win32:DESTDIR = $$SHV_PROJECT_TOP_BUILDDIR/bin

LIBS += \
    -L$$DESTDIR \
    -lnecrolog

INCLUDEPATH += \
	../include

LIBS += \
    -L$$shadowed($$PWD)/../lib \
    -lnecrolog

SOURCES += \
	necro-test.cpp \
    foo.cpp \
    bar.cpp

HEADERS += \
    foo.h \
    bar.h

