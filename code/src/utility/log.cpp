//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#include "utility/log.h"
#include "utility/assert.h"


namespace eda {

  extern FILE* fp_;

  void eda_print_cmd_profile(const char* cmd_name, const timeval time_begin, const timeval time_end, const timespec tp_begin, const timespec tp_end) {
    double elapsed_time = static_cast<double>(time_end.tv_sec - time_begin.tv_sec);
    elapsed_time += static_cast<double>(time_end.tv_usec - time_begin.tv_usec) / 1000000.0;

    double cpu_time = static_cast<double>(tp_end.tv_sec - tp_begin.tv_sec);
    cpu_time += static_cast<double>(tp_end.tv_nsec - tp_begin.tv_nsec) / 1000000000.0;

    if (elapsed_time > 0.01) {
      eda_info("*INFO*: %s elapsed time %.2f seconds, cpu_time %.2f seconds\n", cmd_name, elapsed_time, cpu_time);
    }

  }


  void dump_stack() {
#ifdef WIN32
    eda_info("*INFO*: Dumping stack is not supported in windows.\n");
#else
    void* buffer[1024];
    int n = backtrace(buffer, 1024);
    char** infos = backtrace_symbols(buffer, n);
    //backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO);
    char exec_path[1024] = { 0 };
    if (readlink("/proc/self/exe", exec_path, sizeof(exec_path)) <= 0)
      eda_info("Could not get the execute binary's path!\n");
    char cmd[2048];
    sprintf(cmd, "addr2line -ifC -e %s", exec_path);
    size_t len = strlen(cmd);
    char* p = cmd + len;
    size_t s = sizeof(cmd) - len;
    for (int i = 0; i < n; i++) {
      if (s > 0) {
        len = snprintf(p, s, " %p", buffer[i]);
        p += len;
        s -= len;
      }
    }
    eda_info("=================\nFrame info:\n");
    FILE* fp = NULL;
    if ((fp = popen(cmd, "r")) != NULL) {
      char buf[128];
      while (fgets(buf, sizeof(buf), fp))
        eda_info("%s\n", buf);
      pclose(fp);
    }
    eda_info("=================\n");
    free(infos);
#endif // WIN32
  }


  

}