#include <qdesktopwidget.h>
#include <qapplication.h>
#include <qmainwindow.h>
#include <qaction.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qboxlayout.h>
#include <qpainter.h>

#include "tcl/commands.h"
#include "utility/log.h"


namespace eda {

  int DeviceEditor(ClientData data, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]) {
    int argc = objc;
    char** argv = NULL;
    QApplication* main_app = new QApplication(argc, argv);
    QMainWindow* main_window = new QMainWindow();
    main_window->show();
    main_app->exec();
    delete main_window;
    delete main_app;

    return TCL_OK;
  }


}