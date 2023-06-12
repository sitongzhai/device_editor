//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef UTILITY_DATA_VAR_H
#define UTILITY_DATA_VAR_H

#include <vector>
#include <list>
#include <string>

#include "utility/time.h"

namespace eda {

  class DataVarObserver;

  class DataVar {
  public:

  public:
    static int current_used_memory_;
    static int cmd_peak_used_memory_;
    static int eda_peak_used_memory_;

    static bool cmd_started_;
    static Time time_;

    static void insertObserver(DataVarObserver* observer, int index = -1) {
      auto iter = observers_.begin();
      if (index == -1) {
        iter = observers_.end();
      } else {
        std::advance(iter, index);
      }
      observers_.insert(iter, observer);
    }
    static void removeObserver(DataVarObserver* observer) { observers_.remove(observer); }

  private:
    static std::list<DataVarObserver*> observers_;
  };

  class DataVarObserver {
  public:
    DataVarObserver(int index = -1) {
      DataVar::insertObserver(this, index);
    }

    virtual ~DataVarObserver() {
      DataVar::removeObserver(this);
    }

  };

}

#endif 
