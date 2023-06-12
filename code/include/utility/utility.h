//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef UTILITY_UTILITY_H
#define UTILITY_UTILITY_H

#include <vector>
#include <string>
#include <regex>
#include <sstream>

namespace eda {
  std::vector<std::string> splitStringWithDelim(const std::string& s, char delim, bool removeEmptyString = true);
  static bool getFilesByExtension(const char* str, const char* ext_regex, std::vector<std::string>& file_paths);
}


#endif