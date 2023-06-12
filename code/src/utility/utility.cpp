//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include "utility/utility.h"

namespace eda {
  std::vector<std::string> splitStringWithDelim(const std::string& s, char delim, bool removeEmptyString) {
    std::string sTmp(s);
    size_t stringLocation;
    while ((stringLocation = sTmp.find("\r")) != std::string::npos)
      sTmp.replace(stringLocation, 1, "");
    while ((stringLocation = sTmp.find("\t")) != std::string::npos)
      sTmp.replace(stringLocation, 1, " ");
    while ((stringLocation = sTmp.find("\n")) != std::string::npos)
      sTmp.replace(stringLocation, 1, " ");
    if (delim != ' ') {
      while ((stringLocation = sTmp.find(" ")) != std::string::npos)
        sTmp.replace(stringLocation, 1, "");
    }
    std::stringstream ss(sTmp);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
      if (removeEmptyString) {
        if (item.find_first_not_of(" ") == std::string::npos)
          continue;
      }
      elems.push_back(item);
    }
    return elems;
  }


  bool getFilesByExtension(const char* str, const char* ext_regex, std::vector<std::string>& file_paths) {
    std::string s(str);
    const std::string c_delem = " ";
    s = s.substr(s.find_first_not_of(" "), s.find_last_not_of(" ") - s.find_first_not_of(" ") + 1);
    std::vector<std::string> res;
    std::string::size_type pos1 = 0, pos2 = 0;
    pos2 = s.find(c_delem);
    while (pos2 != std::string::npos) {
      std::string s_res = s.substr(pos1, pos2 - pos1);
      if (!s_res.empty()) {
        res.push_back(s_res);
      }
      pos1 = pos2 + c_delem.size();
      pos2 = s.find(c_delem, pos1);
    }
    if (pos1 != std::string::npos) {
      res.push_back(s.substr(pos1));
    }
    for (size_t i = 0; i < res.size(); ++i) {
      std::string s_data = res.at(i);
      std::string regex = ".+\\.";
      regex += ext_regex;
      regex += "$";
      std::regex e(regex, std::regex_constants::icase);
      std::regex_iterator<std::string::iterator> rit(s_data.begin(), s_data.end(), e);
      std::regex_iterator<std::string::iterator> rend;
      if (rit == rend) {
        return false;
      }
      while (rit != rend) {
        std::string s_res = (*rit).str();
        file_paths.push_back(s_res);
        rit++;
      }
    }
    return true;
  }
}