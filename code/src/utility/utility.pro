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

HEADERS += $$top_srcdir/include/utility/app.h \
           $$top_srcdir/include/utility/assert.h \
           $$top_srcdir/include/utility/data_var.h \
           $$top_srcdir/include/utility/exception.h \
           $$top_srcdir/include/utility/file.h \
           $$top_srcdir/include/utility/log.h \
           $$top_srcdir/include/utility/time.h \
           $$top_srcdir/include/utility/utility.h \
           $$top_srcdir/include/utility/win32.h \

SOURCES += app.cpp \
           data_var.cpp \
           log.cpp \
           time.cpp \
           utility.cpp\
           