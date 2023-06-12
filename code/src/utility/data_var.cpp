//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <vector>
#include "utility/log.h"
#include "utility/data_var.h"
#include <tcl.h>

namespace eda {

  int DataVar::current_used_memory_ = 0;
  int DataVar::cmd_peak_used_memory_ = 0;
  int DataVar::eda_peak_used_memory_ = 0;

  bool DataVar::cmd_started_ = 0;
  Time DataVar::time_;
  std::list<DataVarObserver*> DataVar::observers_;

}