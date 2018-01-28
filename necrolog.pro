TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += \
    libnecrolog \

CONFIG(debug, debug|release) {
SUBDIRS += \
    tests \
}

