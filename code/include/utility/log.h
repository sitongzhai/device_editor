//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#ifndef UTILITY_LOG_H
#define UTILITY_LOG_H
#include <climits>
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdarg.h>
#include <stdio.h>

#ifndef WIN32
#include <execinfo.h>
#include <unistd.h>
#endif // !WIN32


#include "utility/win32.h"
#include "utility/assert.h"
#include "gui/console/console.h"

const std::string EDAVersion = "1.0";
const std::string default_logfile_name = ".log_file/eda.log";
const std::string default_history_file = ".eda_history_cmd";
const std::string prompt_prefix = "EDA>";

namespace eda {

  void eda_print_cmd_profile(const char* cmd_name, const timeval time_begin, const timeval time_end, const timespec tp_begin, const timespec tp_end);


#define eda_print eda::Console::print
#define eda_info eda::Console::printInfoMsg
#define eda_warning eda::Console::printWarnMsg
#define eda_error eda::Console::printErrorMsg
#define eda_debug eda::Console::printDebugMsg

#define eda_gui_info eda::Console::printGUIInfoMsg
#define eda_gui_warning eda::Console::printGUIWarnMsg
#define eda_gui_error eda::Console::printGUIErrorMsg


  


  void dump_stack();


  


  

}

#endif // !UTILITY_EDA_LOG_H
