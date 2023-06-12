//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#ifndef WIN32
#include <execinfo.h>
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#else
#include <Windows.h>
#include <Psapi.h>
#endif

#include <qapplication.h>
#include <qmessagebox.h>

#include "utility/assert.h"
#include "utility/file.h"
#include "utility/app.h"
#include "gui/gui.h"
#include "gui/main_app.h"
#include "gui/main_event.h"
#include "gui/console/console.h"
#include "gui/console/main_console.h"

namespace eda {
#define LOG_BUF_SIZE 4096

  MainConsole* Console::console_ = NULL;
  FILE* Console::fp_ = NULL;
  std::string Console::logfile_name_ = "";

  FILE* Console::fp() { 
    return fp_; 
  }

  void Console::set_fp(FILE* fp) {
    if (fp_ != NULL)
      fclose(fp_);
    fp_ = fp;
  }

  std::string Console::logfile_name() {
    return logfile_name_;
  }

  void Console::set_logfile_name(std::string logfile_name) {
    logfile_name_ = logfile_name;
  }

  MainConsole* Console::console_window() { 
    return console_; 
  }

  void Console::set_console(MainConsole* console) {
    if (console_ != NULL)
      delete console_;
    console_ = console;
  }

  void Console::done() {
    if (console_ != NULL) {
      QApplication::postEvent(console_, new ConsoleEvent(CONSOLE_END, ""));
    }
  }

  void Console::print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[LOG_BUF_SIZE] = { 0 };
    vsnprintf(buffer, LOG_BUF_SIZE, format, args);
    va_end(args);

    if (fp_ != NULL) {
      fprintf(fp_, "%s", buffer);
#ifdef WIN32
      fflush(fp_);
#endif
    }
    printf("%s", buffer);
    fflush(stdout);
  }

  void Console::printInfoMsg(const char* format, ...) {
    bool output = false;

    va_list args;
    va_start(args, format);
    char info_format[LOG_BUF_SIZE] = "*INFO*: ";
    strcat(info_format, format);
    char buffer[LOG_BUF_SIZE] = { 0 };
    vsnprintf(buffer, LOG_BUF_SIZE, info_format, args);
    va_end(args);

    if (console_ != NULL) {
      ConsoleEvent* event = new ConsoleEvent(CONSOLE_INFO, buffer);
      QApplication::postEvent(console_, event);
      output = true;
    }

    if (fp_ != NULL) {
      fprintf(fp_, "%s", buffer);
#ifdef WIN32
      fflush(fp_);
#endif
    }
    if (!output) {
      printf("%s", buffer);
      fflush(stdout);
    }
  }

  void Console::printWarnMsg(const char* format, ...) {
    bool output = false;

    va_list args;
    va_start(args, format);
    char warn_format[LOG_BUF_SIZE] = "*WARN*: ";
    strcat(warn_format, format);
    char buffer[LOG_BUF_SIZE] = { 0 };
    vsnprintf(buffer, LOG_BUF_SIZE, warn_format, args);
    va_end(args);

    if (console_ != NULL) {
      ConsoleEvent* event = new ConsoleEvent(CONSOLE_WARN, buffer);
      QApplication::postEvent(console_, event);
      output = true;
    }

    if (fp_ != NULL) {
      fprintf(fp_, "%s", buffer);
#ifdef WIN32
      fflush(fp_);
#endif
    }

    if (!output) {
#ifdef WIN32
      //#define COLOR_YELLOW 6
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x06);
      printf("%s", buffer);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
#else
      printf("%s", buffer);
#endif
      fflush(stdout);
    }
  }

  void Console::printErrorMsg(const char* format, ...) {
    bool output = false;

    va_list args;
    va_start(args, format);
    char error_format[LOG_BUF_SIZE] = "*ERROR*: ";
    strcat(error_format, format);
    char buffer[LOG_BUF_SIZE] = { 0 };
    vsnprintf(buffer, LOG_BUF_SIZE, error_format, args);
    va_end(args);

    if (console_ != NULL) {
      ConsoleEvent* event = new ConsoleEvent(CONSOLE_ERROR, buffer);
      QApplication::postEvent(console_, event);
      output = true;
    }

    if (fp_ != NULL) {
      fprintf(fp_, "%s", buffer);
#ifdef WIN32
      fflush(fp_);
#endif
    }

    if (!output) {
#ifdef WIN32
      //#define COLOR_RED 4
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x04);
      printf("%s", buffer);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
#else
      printf("%s", buffer);
#endif
      fflush(stdout);
    }
  }

  void Console::printDebugMsg(const char* format, ...) {
#ifdef DEBUG
    bool output = false;
    va_list args;
    va_start(args, format);
    char debug_format[LOG_BUF_SIZE] = "*DEBUG*: ";
    strcat(debug_format, format);
    char buffer[LOG_BUF_SIZE] = { 0 };
    vsnprintf(buffer, LOG_BUF_SIZE, debug_format, args);
    va_end(args);

    if (console_ != NULL) {
      ConsoleEvent* event = new ConsoleEvent(CONSOLE_DEBUG, buffer);
      QApplication::postEvent(console_, event);
      output = true;
    }

    if (fp_ != NULL) {
      fprintf(fp_, "%s", buffer);
#ifdef WIN32
      fflush(fp_);
#endif
    }
    if (!output) {
#ifdef WIN32
      //#define COLOR_GREEN 4
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x02);
      printf("%s", buffer);
      SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
#else
      printf("%s", buffer);
#endif
      fflush(stdout);
    }
#endif // DEBUG
  }

  int Console::printGUIInfoMsg(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[LOG_BUF_SIZE] = { 0 };
    vsnprintf(buffer, LOG_BUF_SIZE, format, args);
    va_end(args);

    QMessageBox::information(Gui::main_app()->activeWindow(), "Info", buffer);

    return 0;
  }

  int Console::printGUIWarnMsg(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[LOG_BUF_SIZE] = { 0 };
    vsnprintf(buffer, LOG_BUF_SIZE, format, args);
    va_end(args);

    QMessageBox::warning(Gui::main_app()->activeWindow(), "Warning", buffer);

    return 0;
  }

  int Console::printGUIErrorMsg(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[LOG_BUF_SIZE] = { 0 };
    vsnprintf(buffer, LOG_BUF_SIZE, format, args);
    va_end(args);

    QMessageBox::critical(Gui::main_app()->activeWindow(), "Error", buffer);

    return 0;
  }

  void Console::startLogFile(std::string log_file) {
    if (!App::avoidOverwrite(log_file)) {
      printErrorMsg("Failed to rename existing log file %s, it has to been overwriten!\n", log_file.c_str());
    }
    size_t path_index = log_file.find_last_of('/');
    std::string log_dir = ".";
    if (path_index != -1) {
      log_dir = log_file.substr(0, path_index + 1);
    }
    //check if log_dir can be accessed
    if (File::access(log_dir.c_str(), 0) == -1) {
      File::mkdir(log_dir.c_str());
    }
    //check if log_dir can be written
    if (File::access(log_dir.c_str(), 2) != -1) {
      FILE* fp = fopen(log_file.c_str(), "w");
      if (ferror(fp)) eda_assert(0);
      
      Console::set_fp(fp);
      Console::set_logfile_name(log_file);
#ifdef WIN32
      //setvbuf(fp, NULL, _IOLBF, 256); // no line buffer on windows
#else
      setlinebuf(fp);
#endif
    }
    return;
  }

  void Console::endLogFile() {
    if (fp_ != NULL)
      fclose(fp_);
    fp_ = NULL;
    logfile_name_ = "";
  }
}
