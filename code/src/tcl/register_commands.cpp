//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <tcl.h>

#include "tcl/commands.h"

namespace eda {

  Commands gCommands;

  int registerAllCmds(Tcl_Interp* interp) {
    Commands::set_interp(interp);

    
    return TCL_OK;
  }
}