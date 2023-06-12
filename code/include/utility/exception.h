//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#ifndef UTILITY_EXCEPTION_H
#define UTILITY_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace eda {

  class Exception : public std::runtime_error {
  public:
    Exception() throw() : runtime_error(std::string()) {
#ifdef _DEBUG
      assert(0);
#endif
    }
    explicit Exception(const char* what_arg) throw () : runtime_error(what_arg) {
#ifdef _DEBUG
      assert(0);
#endif
    }
    explicit Exception(const std::string& what_arg) throw () : runtime_error(what_arg) {
#ifdef _DEBUG
      assert(0);
#endif
    }
  };

  class IOException : public Exception {
  public:
    IOException() throw () : Exception() {

    }
    explicit IOException(const std::string& what_arg) throw () :
      Exception(what_arg) {

    }
  };

}

#endif
