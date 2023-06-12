//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <stdlib.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "tcl/commands.h"
#include "utility/log.h"
#include "utility/assert.h"
#include "utility/data_var.h"

namespace eda {

  extern int tclInitProc(Tcl_Interp* interp);

  Tcl_Interp* Commands::interp_ = NULL;

  void Commands::init(int argc, char* argv[]) {
    //register all commands before Tclsh is invoked
    //To register a command, please modify RegisterAllCmds()
    Tcl_Main(argc, argv, tclInitProc);
  }
  void Commands::register_cmd(Tcl_Interp* interp, const std::string cmd_name, const std::string cmd_options, CommandFunction cmd_func) {
    commands_.push_back(cmd_name);
    options_.push_back(cmd_options);
    cmd_funcs_.push_back(cmd_func);

    //register options
    std::string arguments;
    std::vector<Option> options;
    splitString(cmd_options, arguments, options);
    cmd_opts_.insert(std::pair<std::string, std::vector<Option>>(cmd_name, options));
    cmd_args_.insert(std::pair<std::string, std::string>(cmd_name, arguments));

    //register cmd_name and cmd_func with tclsh
    Tcl_CreateObjCommand(interp, cmd_name.c_str(), cmd_func, (ClientData)NULL, (Tcl_CmdDeleteProc*)NULL);
  }
  int Commands::executeCmd(const char* cmd, const bool print_result) {
    if (interp_ == NULL) {
      eda_error("No Tcl Interpret specified.\n");
      return TCL_ERROR;
    }
    int ret = Tcl_Eval(interp_, cmd);
    const char* result = Tcl_GetStringResult(interp_);

    if (print_result && strlen(result) > 0) {
      eda_info(result);
    }
    return ret;
  }

  int Commands::getObjectIndex(const char* options, const char* obj_name) {
    const char* s = strstr(options, obj_name);
    if (s == NULL) {
      return -1;
    }
    unsigned int idx = 0;
    while (s > options) {
      if (*s == ' ')
        idx++;
      s--;
    }
    return idx;
  }
  char* Commands::getObject(Tcl_Obj* const objv[], const char* obj_name) {
    const std::string command_name = std::string(Tcl_GetString(objv[0]));
    for (int i = 0; i < numCmds(); i++) {
      if (commands(i) == command_name) {
        const int obj_idx = getObjectIndex(options(i).c_str(), obj_name);
        if (obj_idx >= 0) {
          return Tcl_GetString(objv[obj_idx + 1]);
        } else {
          return NULL;
        }
      }
    }
    return NULL;
  }
  int Commands::getOptionIndex(const int objc, Tcl_Obj* const objv[], const char* optionName) {
    for (int i = 1; i < objc; i++) {
      if (strcmp(optionName, Tcl_GetString(objv[i])) == 0) {
        return i;
      }
    }
    return -1;
  }
  bool Commands::isOptionUsed(const int objc, Tcl_Obj* const objv[], const char* optionName) {
    if (getOptionIndex(objc, objv, optionName) > -1)
      return true;
    else
      return false;
  }
  bool Commands::getBoolOption(const int objc, Tcl_Obj* const objv[], const char* option_name, bool& value) {
    int i = getOptionIndex(objc, objv, option_name);
    if (i > -1) {
      if (i == objc - 1) {
        eda_error("A boolean vaule is expected for option %s\n",option_name);
        return false;
      }else {
        const char* v = Tcl_GetString(objv[i + 1]);
        if (strcasecmp(v, "true") == 0) {
          value = true;
          return true;
        } else if (strcasecmp(v, "false") == 0) {
          value = false;
          return true;
        } else {
          eda_error("A boolean vaule is expected for option %s\n", option_name);
          return false;
        }
      }
    }
    return false;
  }
  bool Commands::getIntOption(const int objc, Tcl_Obj* const objv[], const char* option_name, int& value) {
    int i = getOptionIndex(objc, objv, option_name);
    if (i > -1) {
      if (i == objc - 1) {
        eda_error("An integer vaule is expected for option %s\n", option_name);
      } else {
        if (Tcl_GetInt(NULL, Tcl_GetString(objv[i + 1]), &value) == TCL_OK)
          return true;
        eda_error("An integer vaule is expected for option %s\n", option_name);
      }
    }
    return false;
  }
  bool Commands::getFloatOption(const int objc, Tcl_Obj* const objv[], const char* option_name, float& value) {
    int i = getOptionIndex(objc, objv, option_name);
    if (i > -1) {
      if (i == objc - 1) {
        eda_error("A float vaule is expected for option %s\n", option_name);
      } else {
        double tmp_double;
        if (Tcl_GetDouble(NULL, Tcl_GetString(objv[i + 1]), &tmp_double) == TCL_OK) {
          value = static_cast<float>(tmp_double);
          return true;
        }
        eda_error("A float vaule is expected for option %s\n", option_name);
      }
    }
    return false;
  }
  bool Commands::getDoubleOption(const int objc, Tcl_Obj* const objv[], const char* option_name, double& value) {
    int i = getOptionIndex(objc, objv, option_name);
    if (i > -1) {
      if (i == objc - 1) {
        eda_error("A double vaule is expected for option %s\n", option_name);
      } else {
        if (Tcl_GetDouble(NULL, Tcl_GetString(objv[i + 1]), &value) == TCL_OK)
          return true;
        eda_error("A double vaule is expected for option %s\n", option_name);
      }
    }
    return false;
  }
  bool isSeparator(const char* name) {
    return name[0] == '-' && strlen(name) > 1 && !isdigit(name[1]);
  }

  bool Commands::getStringOption(const int objc, Tcl_Obj* const objv[], const char* option_name, std::string& value) {
    int i = getOptionIndex(objc, objv, option_name);
    if (i > -1) {
      if (i == objc - 1) {
        eda_error("A string vaule is expected for option %s\n", option_name);
      } else {
        bool is_valid = true;
        do {
          std::string tmp = Tcl_GetString(objv[++i]);
          if (isSeparator(tmp.c_str()) || tmp[0] == '$')
            break;
          if (value.length() > 0) {
            value += " ";
          }
          value += tmp;
          is_valid = true;
        } while (i < objc - 1);
        if (is_valid) {
          return true;
        }
        eda_info("A string vaule is expected for option %s\n", option_name);
      }
    }
    return false;
  }
  void Commands::printHelp(Tcl_Obj* const objv[]) const {
    const std::string cmd_name = Tcl_GetString(objv[0]);
    printHelp(cmd_name);
    return;
  }
  void Commands::printHelp(const std::string cmd_name, const std::string opt_name) const {
    eda_info("  HelpInfo::\n");
    for (int i = 0; i < numCmds(); ++i) {
      if (cmd_name == "") {
        if (commands_[i].substr(0, 4) == "test") {
          continue;
        }
        std::string argument;
        std::map<std::string, std::string>::const_iterator it = cmd_args_.find(commands_[i]);
        if (it != cmd_args_.end()) {
          argument = it->second;
        }
        std::string option = visibleOption(commands_[i], opt_name);
        eda_info("  Command name : %s\n", commands_[i].c_str());
        eda_info("  Option: %s %s\n", argument.c_str(), option.c_str());
      } else if (commands_[i] == cmd_name) {
        std::string argument;
        std::map<std::string, std::string>::const_iterator it = cmd_args_.find(cmd_name);
        if (it != cmd_args_.end()) {
          argument = it->second;
        }
        std::string option = visibleOption(cmd_name, opt_name);
        eda_info("  Command name : %s\n", commands_[i].c_str());
        eda_info("  Option: %s %s\n", argument.c_str(), option.c_str());
        return;
      }
    }
  }
  std::string Commands::visibleOption(const std::string cmd_name,
    const std::string opt_name) const {
    if (cmd_name.length() == 0) {
      return "";
    }
    std::map<std::string, std::vector<Option> >::const_iterator it = cmd_opts_.find(cmd_name);
    std::vector<Option> options;
    if (it != cmd_opts_.end()) {
      options = it->second;
    }
    std::string option_str;
    int option_cnt = (int)options.size();
    for (int i = 0; i < option_cnt; ++i) {
      if (opt_name != "") {
        if (options[i].option == opt_name) {
          option_str += options[i].option;
          option_str += " ";
          option_str += options[i].regex;
          break;
        }
      } else {
        if (options[i].option[1] != '!') {
          if (i > 0 && i % 5 == 0)
            option_str += "\n          ";
          option_str += options[i].option;
          if (!options[i].regex.empty()) {
            option_str += options[i].regex;
          }
          option_str += " ";
        }
      }
    }
    option_str += "-help -h";
    return option_str;
  }

  bool Commands::preRun(int objc, Tcl_Obj* const objv[]) {
    if (Commands::isOptionUsed(objc, objv, "-help") || Commands::isOptionUsed(objc, objv, "-h")) {
      gCommands.printHelp(objv);
      return false;
    }
    DataVar::cmd_started_ = true;
#ifndef WIN32
    usleep(2000);
#endif
    if (!checkArguments(objc, objv) || !checkOptions(objc, objv)) {
      return false;
    }
    //FIXME
    //check stages
    if (verbose_) {
      DataVar::time_.getProcessCpuTime(&tp_begin_);
      DataVar::time_.getElapsedTime(&time_begin_);
      memory_begin_ = DataVar::current_used_memory_;
    }
    return true;
  }
  bool Commands::postRun(int objc, Tcl_Obj* const objv[]) {
    std::string cmd_full_name;
    Commands::getFullName(objc, objv, cmd_full_name);
    if (verbose_) {
      DataVar::time_.getElapsedTime(&time_end_);
      DataVar::time_.getProcessCpuTime(&tp_end_);
      char* cmd_name = Tcl_GetString(objv[0]);
#ifndef WIN32
      usleep(6000);
#endif
      eda_print_cmd_profile(cmd_name, time_begin_, time_end_, tp_begin_, tp_end_);
      int cmd_used_mem = DataVar::cmd_peak_used_memory_ - memory_begin_;
      eda_info("*INFO*: %s used memory %dMb, firrtlsyn used peak memory %dMb, current used memory %dMb.\n", cmd_name, cmd_used_mem, DataVar::eda_peak_used_memory_, DataVar::current_used_memory_);
    }
    DataVar::cmd_started_ = false;
#ifndef WIN32
    usleep(2000);
#endif // !WIN32
    return true;
  }
  bool Commands::checkArguments(int objc, Tcl_Obj* const objv[]) {
    char* cmd_name = Tcl_GetString(objv[0]);
    std::map<std::string, std::string>::iterator it = cmd_args_.find(cmd_name);
    std::string argument;
    if (it != cmd_args_.end()) {
      argument = it->second;
    }
    std::vector<std::string> regex;
    splitString(argument, regex);
    int required_args_num = static_cast<int> (regex.size());
    int given_args_num = 0;
    std::string para;
    for (int i = 1; i < objc; ++i) {
      char* opt_name = Tcl_GetString(objv[i]);
      if (!isSeparator(opt_name)) {
        given_args_num++;
        para += opt_name[0];
        para += " ";
      }
    }
    if (given_args_num < required_args_num) {
      eda_error("  Invalid option arguments: %s\n", para.c_str());
      printHelp(cmd_name);
      return false;
    }
    return true;
  }
  bool Commands::matchRegex(const std::string& para, const std::string& regex) {
    std::string str = trimString(para);
    std::string rgx = trimString(regex);
    if (rgx.length() == 0) {
      if (str.length() == 0) {
        return true;
      } else {
        return false;
      }
    }
    if (rgx[0] == '{') {
      if (rgx[rgx.length() - 1] != '}') {
        return false;
      }
      str = trimString(str, "{", "}");
      rgx = trimString(rgx, "{", "}");
    }
    std::vector<std::string> str_v;
    splitString(str, str_v, " ");
    std::vector<std::string> rgx_v;
    splitString(rgx, rgx_v, " ");
    int str_v_size = static_cast<int> (str_v.size());
    int rgx_v_size = static_cast<int> (rgx_v.size());
    if (rgx[0] == '<') {
      if (rgx_v_size == 1) {
        std::string tmp = trimString(rgx_v[0], "<", ">");
        if (tmp == "string") {
          if (isValidString(str)) {
            return true;
          }
        }
      }
      if (str_v_size != rgx_v_size) {
        return false;
      }
    }
    for (int i = 0; i < rgx_v_size; ++i) {
      if (rgx_v[i][0] == '<') {
        std::string tmp = trimString(rgx_v[i], "<", ">");
        if (tmp == "int") {
          if (!isValidInt(str_v[i])) {
            return false;
          }
        } else if (tmp == "double" || tmp == "float") {
          if (!isValidDouble(str_v[i])) {
            return false;
          }
        } else if (tmp == "string") {
          if (!isValidString(str_v[i])) {
            return false;
          }
        } else {
          eda_warning("Unsupport data type %s\n", tmp.c_str());
        }
      } else {
        eda_assert(rgx_v_size == 1);
        std::vector<std::string> tmp;
        splitString(rgx_v[i], tmp, "|");
        int tmp_size = static_cast<int> (tmp.size());
        for (int j = 0; j < str_v_size; ++j) {
          bool isValid = false;
          for (int k = 0; k < tmp_size; ++k) {
            if (str_v[j] == tmp[k]) {
              isValid = true;
            }
          }
          if (!isValid) {
            return false;
          }
        }
      }
    }
    return true;
  }
  bool Commands::checkOptions(int objc, Tcl_Obj* const objv[]) {
    char* cmd_name = Tcl_GetString(objv[0]);
    std::map<std::string, std::vector<Option> >::iterator it = cmd_opts_.find(cmd_name);
    std::vector<Option> options;
    if (it != cmd_opts_.end()) {
      options = it->second;
    }
    for (int i = 1; i < objc; ++i) {
      char* opt_name = Tcl_GetString(objv[i]);
      if (!isSeparator(opt_name))
        continue;
      std::string para = "";
      while (++i < objc) {
        char* para_name = Tcl_GetString(objv[i]);
        if (isSeparator(para_name)) {
          --i;
          break;
        }
        para += " ";
        para += para_name;
      }
      std::string opt(opt_name);
      std::vector<Option>::iterator iter = options.begin();
      for (iter = options.begin(); iter != options.end(); ++iter) {
        if (iter->option == opt) {
          if (matchRegex(para, iter->regex)) {
            break;
          } else {
            eda_error("  Invalid option arguments: %s\n", para.c_str());
            printHelp(cmd_name, opt_name);
            return false;
          }
        }
      }
      if (iter == options.end()) {
        eda_error("  Invalid option name: %s\n", opt_name);
        printHelp(cmd_name);
        return false;
      }
    }
    return true;
  }
  int Commands::splitString(const std::string& str, std::vector<std::string>& ret, std::string sep) {
    if (str.empty())
      return 0;

    std::string tmp;
    std::string::size_type pos_begin = str.find_first_not_of(sep);
    std::string::size_type comma_pos = 0;
    while (pos_begin != std::string::npos) {
      comma_pos = str.find(sep, pos_begin);
      if (comma_pos != std::string::npos) {
        tmp = str.substr(pos_begin, comma_pos - pos_begin);
        pos_begin = comma_pos + sep.length();
      } else {
        tmp = str.substr(pos_begin);
        pos_begin = comma_pos;
      }
      if (!tmp.empty()) {
        ret.push_back(tmp);
        tmp.clear();
      }
    }
    return 0;
  }
  int Commands::splitString(const std::string& str, std::string& arguments, std::vector<Option>& ret, std::string sep) {
    if (str.empty()) {
      return 0;
    }
    std::vector<std::string> opts;
    splitString(str, opts, sep);
    int i_opts_num = static_cast<int>(opts.size());
    for (int i = 0; i < i_opts_num; i++) {
      while (i < i_opts_num && !isSeparator(opts[i].c_str())) {
        if (!arguments.empty())
          arguments += " ";
        arguments += opts[i];
        i++;
      }
      if (i == i_opts_num)
        break;
      Option o_opt;
      o_opt.option = opts[i];
      if (opts[i] == "-" && (i + 1) < i_opts_num) {
        o_opt.option = opts[++i];
      }
      std::string regex = "";
      while (++i < i_opts_num) {
        if (isSeparator(opts[i].c_str())) {
          --i;
          break;
        }
        regex += " ";
        regex += opts[i];
      }
      o_opt.regex = regex;
      ret.push_back(o_opt);
    }
    return 0;
  }
  std::string Commands::trimString(const std::string& str, const std::string& trim_b, const std::string& trim_e) {
    std::string::size_type pos = str.find_first_not_of(trim_b);
    if (pos == std::string::npos) {
      return str;
    }
    std::string::size_type pos2 = str.find_last_not_of(trim_e);
    if (pos2 != std::string::npos) {
      return str.substr(pos, pos2 - pos + 1);
    }
    return str.substr(pos);
  }
  bool Commands::isValidInt(std::string opt) {
    int len = static_cast<int> (opt.length());
    if (len == 0) {
      return false;
    }
    int i = 0;
    if (opt[0] == '-')
      ++i;
    for (; i < len; ++i) {
      if (!isdigit(opt[i])) {
        return false;
      }
    }
    return true;
  }
  bool Commands::isValidDouble(std::string opt) {
    int len = static_cast<int> (opt.length());
    if (len == 0) {
      return false;
    }
    int i = 0;
    if (opt[0] == '-')
      ++i;
    for (; i < len; ++i) {
      if (!isdigit(opt[i]) && (opt[i] != '.')) {
        return false;
      }
    }
    return true;
  }
  bool Commands::isValidString(std::string opt) {
    if (opt.length() == 0) {
      return false;
    }
    return true;
  }
}