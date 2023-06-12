//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#ifndef GUI_CONSOLE_H
#define GUI_CONSOLE_H

#include <string>
#include <vector>

namespace eda {

  class MainConsole;

  class Console {
  private:
    static MainConsole* console_;
    static FILE* fp_;
    static std::string logfile_name_;

  public:
    static FILE* fp();
    static void set_fp(FILE* fp);
    static std::string logfile_name();
    static void set_logfile_name(std::string logfile_name);
    static MainConsole* console_window();
    static void set_console(MainConsole* console);

  public:
    // In any case, after using the console, we should call
    // Console::done() to make it available again.
    static void done();
    // Print anything on console
    static void print(const char* format, ...);

    // Print Info message on console
    static void printInfoMsg(const char* format, ...);
    // Print Warnning message on console
    static void printWarnMsg(const char* format, ...);
    // Print Error message on console
    static void printErrorMsg(const char* format, ...);
    // Print Debug message on console when Debug mode
    static void printDebugMsg(const char* format, ...);

    // Print Info message on GUI Info Message Box
    static int printGUIInfoMsg(const char* format, ...);
    // Print Warnning message on GUI Warn Message Box
    static int printGUIWarnMsg(const char* format, ...);
    // Print Error message on GUI Error Message Box
    static int printGUIErrorMsg(const char* format, ...);

    // start to write every log message into a file
    static void startLogFile(std::string log_file);
    // close the open log file
    static void endLogFile();
  };

}

#endif // !GUI_CONSOLE_H
