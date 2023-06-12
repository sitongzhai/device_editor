//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#ifndef UTILITY_APP_H
#define UTILITY_APP_H

#include <string>
#include <vector>
namespace eda {
  class App {
  public:
    static bool canonicalFilePath(std::string &file_path, std::string &canonical_path);
    static std::string getDBPath();
    static bool avoidOverwrite(const std::string & file_name);
  };
}

#endif
