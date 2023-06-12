!include($$top_srcdir/common.pri) {
    error("Couldn't find the common.pri file!")
}

TEMPLATE = lib
CONFIG += staticlib

HEADERS += $$top_srcdir/include/tcl/commands.h

SOURCES += commands.cpp \
           register_commands.cpp \
           tcl_init.cpp \