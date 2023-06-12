//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#ifndef UTILITY_TIME_H
#define UTILITY_TIME_H

#include <stdio.h>
#include <time.h>
#ifdef WIN32
#include "utility/win32.h"
#else
#include <sys/time.h>
#endif // WIN32




namespace eda {
  class Time {
  public:
    Time() {
      tp_prev.tv_sec = 0;
      tp_prev.tv_nsec = 0;
    }

    int getProcessCpuTime(timespec *tp);
    int getThreadCpuTime(timespec *tp);
    int getElapsedTime(timeval *tp);
  private:
    timespec tp_prev;
  };
}


#endif
