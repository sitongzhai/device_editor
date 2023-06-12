!include( $$top_srcdir/common.pri ) {
  error( "Couldn't find the common.pri file!" )
}

TEMPLATE = lib
CONFIG += staticlib

MOC_DIR = moc

HEADERS += $$top_srcdir/include/gui/gui.h \
           $$top_srcdir/include/gui/command_context.h \
           $$top_srcdir/include/gui/main_app.h \
           $$top_srcdir/include/gui/main_event.h \
           $$top_srcdir/include/gui/main_window.h \
           $$top_srcdir/include/gui/gui_utility.h \
           $$top_srcdir/include/gui/project/project.h \
           $$top_srcdir/include/gui/project/project_widget.h \
           $$top_srcdir/include/gui/project/new_project_wizard.h \
           $$top_srcdir/include/gui/project/mdi_subwindow.h \
           $$top_srcdir/include/gui/project/source_file_selector.h \
           $$top_srcdir/include/gui/console/console.h \
           $$top_srcdir/include/gui/console/main_console.h \
           $$top_srcdir/include/gui/console/command_executor.h \
           $$top_srcdir/include/gui/console/command_line.h \
           $$top_srcdir/include/gui/layout/layout_window.h \

SOURCES += editor.cpp