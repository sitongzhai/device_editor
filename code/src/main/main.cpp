//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <stdlib.h>
#include <string>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <fstream>
#ifndef WIN32
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#else
#include <Windows.h>
#include <Psapi.h>
#endif

#include "utility/log.h"
#include "utility/data_var.h"
#include "utility/utility.h"
#include "tcl/commands.h"
#include "gui/gui.h"
#include "device/device_manager.h"

namespace eda {


  void StartTime() {
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[256];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 256, "%Y-%m-%d %H:%M:%S", timeinfo);
    eda_print("Start Time: %s\n", buffer);
  }
  void BuildTime() {
    int year, day;
    char month[4];
    memset(month, 0, 3);
    uint8_t i_month = 0;
    sscanf(__DATE__, "%s %d %d", month, &day, &year);
    static const char *op_month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    for (int i = 0; i < 12; i++) {
      if (memcmp(month, op_month[i], 3) == 0) {
        i_month = (uint8_t)(i + 1);
        break;
      }
    }
    char buf[12] = { 0 };
    sprintf(buf, "%d-%02d-%02d", year, i_month, day);
    eda_print("Build Time: %s %s\n", buf, __TIME__);
  }
  void Version() {
    eda_print("FPGAEDA %s\n", EDAVersion.c_str());
#ifdef GIT_REVISION
    const char* reversion = GIT_REVISION;
    if (reversion != NULL) {
      eda_print("Git Version: %s\n", reversion);
    }
#endif
  }

  extern char* init_tcl_file;
}

void* MemoryTraceFunc(void*);

#ifdef WIN32
// hidden win32 dos-style window 
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
long __stdcall CrashCallback(_EXCEPTION_POINTERS* excp) {
  eda_info("Error address: %p", excp->ExceptionRecord->ExceptionAddress);

  return EXCEPTION_EXECUTE_HANDLER;
}
#else
void SigRoutine(int signo) {
  switch (signo) {
  case SIGSEGV:
  case SIGBUS:
    eda_info("We encountered some problems while executing current command.\nIt may be caused by the environment, the input data, or other unexpected conditions.\nFatal error! Abnormal exit!\n");
    eda::dump_stack();
    exit(-1);
    break;
  }
}
#endif

void releaseAll() {
  eda::DeviceManager::release();



}

int main(int argc, char** argv) {

  std::string logfile_name = default_logfile_name;
  bool gui_mode = false;
  for (int i = 1; i < argc; i++) {
    std::string cmd = argv[i];
    if (cmd.compare("-sh") == 0) {
      gui_mode = false;
    } else if (cmd.compare("-gui") == 0) {
      gui_mode = true;
    } else if (cmd.compare("-log") == 0) {
      logfile_name = std::string(argv[i + 1]);
    } else if (argv[i][0] == '-') {
      printf("Error : unknown option : %s\n", argv[i]);
      return 0;
    }
  }
  eda::Console::startLogFile(logfile_name);

#ifdef WIN32
  SetUnhandledExceptionFilter(CrashCallback);
  AllocConsole();
  freopen("CONIN$", "r", stdin);
  freopen("CONOUT$", "w", stdout);
  freopen("CONOUT$", "w", stderr);
#else
  struct sigaction act, oldact;
  act.sa_handler = SigRoutine;
  act.sa_flags = SA_NODEFER;
  sigaction(SIGSEGV, &act, &oldact);
  sigaction(SIGBUS, &act, &oldact);
#endif

  eda::Version();
  eda::BuildTime();
  eda::StartTime();

  eda::init_tcl_file = NULL;
  if (argc > 1 && argv[1][0] != '-') {
    gui_mode = false;
    eda::init_tcl_file = argv[1];
  }

  pthread_t thread_mem_trace;
  pthread_create(&thread_mem_trace, NULL, MemoryTraceFunc, NULL);

  eda::DeviceManager::load();

  if (gui_mode) {
    eda::Gui::init(argc, argv);
  } else {
    eda::Commands::init(argc, argv);
  }

  releaseAll();

  eda::Console::endLogFile();

  exit(0);
}

void* MemoryTraceFunc(void*) {
#ifdef WIN32
  while (true) {
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info));
    eda::DataVar::eda_peak_used_memory_ = static_cast<int>(info.PeakWorkingSetSize) / 1024 / 1024;
    eda::DataVar::current_used_memory_ = static_cast<int>(info.WorkingSetSize) / 1024 / 1024;

    if (!eda::DataVar::cmd_started_)
      eda::DataVar::cmd_peak_used_memory_ = eda::DataVar::current_used_memory_;
    else if (eda::DataVar::cmd_peak_used_memory_ < eda::DataVar::current_used_memory_)
      eda::DataVar::cmd_peak_used_memory_ = eda::DataVar::current_used_memory_;
    Sleep(1);
  }
#else
  while (true) {
    rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    int ru_maxrss = static_cast<int>(usage.ru_maxrss / 1024); //MB
    eda::DataVar::eda_peak_used_memory_ = ru_maxrss; // MB

    const int MAX_SIZE = 512;
    pid_t pid = getpid();
    char proc_status_file_name[MAX_SIZE];
    sprintf(proc_status_file_name, "/proc/%d/status", pid);
    std::fstream proc_status_stream(proc_status_file_name, std::ios::in);
    if (!proc_status_stream.good()) {
      rusage usage;
      getrusage(RUSAGE_SELF, &usage);
      eda::DataVar::current_used_memory_ = static_cast<int>((usage.ru_maxrss) / 1024);
    } else {
      char buf[MAX_SIZE];
      while (proc_status_stream.getline(buf, MAX_SIZE)) {
        if (!strncmp(buf, "VmRSS", 5)) {
          std::vector<std::string> split_strings = eda::splitStringWithDelim(buf, ' ');
          eda_assert(split_strings.size() > 1);
          eda::DataVar::current_used_memory_ = atoi(split_strings[1].c_str()) / 1024;
          break;
        }
      }
    }
    proc_status_stream.close();

    if (!eda::DataVar::cmd_started_)
      eda::DataVar::cmd_peak_used_memory_ = eda::DataVar::current_used_memory_;
    else if (eda::DataVar::cmd_peak_used_memory_ < eda::DataVar::current_used_memory_)
      eda::DataVar::cmd_peak_used_memory_ = eda::DataVar::current_used_memory_;
    usleep(1000);
  }
#endif
  return NULL;
}


