include( ../subproject_integration.pri )

TEMPLATE = app

QT -= core widgets

TARGET = necro-test

DESTDIR = $$SHV_PROJECT_TOP_BUILDDIR/bin

unix:LIBDIR = $$SHV_PROJECT_TOP_BUILDDIR/lib
win32:LIBDIR = $$SHV_PROJECT_TOP_BUILDDIR/bin

LIBS += \
    -L$$LIBDIR \
    -lnecrolog

INCLUDEPATH += \
	../include

LIBS += \
	-L$$LIBDIR/lib \
    -lnecrolog

unix {
	LIBS += \
		-Wl,-rpath,\'\$\$ORIGIN/../lib\'
}

SOURCES += \
	necro-test.cpp \

HEADERS += \

