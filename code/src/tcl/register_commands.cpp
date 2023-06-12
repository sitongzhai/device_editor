//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <tcl.h>

#include "tcl/commands.h"

namespace eda {
  extern int DeviceEditor(ClientData data, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);
  Commands gCommands;

  int registerAllCmds(Tcl_Interp* interp) {
    Commands::set_interp(interp);
    gCommands.register_cmd(interp, "device_editor", "", DeviceEditor);
    
    return TCL_OK;
  }
}