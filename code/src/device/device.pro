!include($$top_srcdir/common.pri) {
    error("Couldn't find the common.pri file!")
}

TEMPLATE = lib
CONFIG += staticlib

unix {
    QMAKE_CXXFLAGS -= -Werror
}
win32 {
    QMAKE_CXXFLAGS -= /WX
}

HEADERS += $$top_srcdir/include/device/device_manager.h \

SOURCES += device_manager.cpp \
           