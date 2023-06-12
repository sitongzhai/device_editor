//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#ifndef TCL_COMMAND_H
#define TCL_COMMAND_H

#include <string>
#include <vector>
#include <map>
#include <tcl.h>
#include "utility/time.h"


namespace eda {

  typedef int(*CommandFunction)(ClientData data, Tcl_Interp* interp, int objc, Tcl_Obj* const objv[]);

  class Commands {
  public:
    struct Option {
      std::string option;
      std::string regex;
    };

    Commands() {
      memory_begin_ = 0;
      verbose_ = true;
    }
    ~Commands() {}

    int numCmds() const { return static_cast<int>(commands_.size()); }
    int numOpts() const { return static_cast<int>(options_.size()); }
    const std::string commands(const int idx) const {
      if (idx < numCmds())
        return commands_[idx];
      else
        return "";
    }
    const std::string options(const int idx) const {
      if (idx < numOpts())
        return options_[idx];
      else
        return "";
    }
    std::vector<Option> command_options(const std::string& cmd) {
      std::map<std::string, std::vector<Option>>::iterator iter = cmd_opts_.find(cmd);
      if (iter != cmd_opts_.end()) {
        return iter->second;
      }
      else {
        return std::vector<Option>();
      }
    }
    void set_verbose(const bool verbose) { verbose_ = verbose; }
    CommandFunction getCmdFunction(const std::string& cmd_name) {
      std::map<std::string, CommandFunction>::iterator iter = cmds_map_.find(cmd_name);
      if (iter != cmds_map_.end()) {
        return iter->second;
      }
      return NULL;
    }
    static void init(int argc, char* argv[]);
    static Tcl_Interp* interp() { return interp_; }
    static void set_interp(Tcl_Interp* interp) { interp_ = interp; }
    static int executeCmd(const char* cmd, const bool print_result = true);
    void register_cmd(Tcl_Interp* interp, const std::string cmd_name, const std::string cmd_options, CommandFunction cmd_func);
    static int getObjectIndex(const char* options, const char* obj_name);
    char* getObject(Tcl_Obj* const kObjv[], const char* obj_name);
    static int getOptionIndex(const int objc, Tcl_Obj* const objv[], const char* optionName);
    static bool isOptionUsed(const int objc, Tcl_Obj* const objv[], const char* optionName);
    static bool getBoolOption(const int objc, Tcl_Obj* const objv[], const char* option_name, bool& value);
    static bool getIntOption(const int objc, Tcl_Obj* const objv[], const char* option_name, int& value);
    static bool getFloatOption(const int objc, Tcl_Obj* const objv[], const char* option_name, float& value);
    static bool getDoubleOption(const int objc, Tcl_Obj* const objv[], const char* option_name, double& value);
    static bool getStringOption(const int objc, Tcl_Obj* const objv[], const char* option_name, std::string& value);
    void printHelp(Tcl_Obj* const kObjv[]) const;
    void printHelp(const std::string cmd_name = "", const std::string opt_name = "") const;
    static void getFullName(int objc, Tcl_Obj* const objv[], std::string& full_name) {
      full_name.clear();
      for (int i = 0; i < objc; i++) {
        std::string cmd_name = Tcl_GetString(objv[i]);
        if (cmd_name.find(' ') != std::string::npos) {
          full_name += "{";
          full_name += cmd_name + "}";
        }
        else {
          full_name += cmd_name;
        }
        if (i < objc - 1)
          full_name += " ";
      }
    }
    bool preRun(int objc, Tcl_Obj* const objv[]);
    bool postRun(int objc, Tcl_Obj* const objv[]);

    bool checkOptions(int objc, Tcl_Obj* const objv[]);
    bool checkArguments(int objc, Tcl_Obj* const objv[]);
    std::string visibleOption(const std::string cmd_name, const std::string opt_name = "") const;
  private:
    int splitString(const std::string& str, std::string& arguments, std::vector<Option>& ret, std::string sep = " ");
    int splitString(const std::string& str, std::vector<std::string>& ret, std::string sep = " ");
    bool matchRegex(const std::string& para, const std::string& regex);
    std::string trimString(const std::string& str, const std::string& trim_b = " ", const std::string& trim_e = " ");
    bool isValidInt(std::string opt);
    bool isValidDouble(std::string opt);
    bool isValidString(std::string opt);
    
  private:
    std::vector<std::string> commands_;
    std::vector<std::string> options_;
    std::vector<CommandFunction> cmd_funcs_;
    std::map<std::string, std::string> cmd_args_;
    std::map<std::string, std::vector<Option>> cmd_opts_;
    std::map<std::string, CommandFunction> cmds_map_;
    static Tcl_Interp* interp_;
    struct timespec tp_begin_, tp_end_;
#ifdef WIN32
    struct timeval time_begin_, time_end_;
#else
    timeval time_begin_, time_end_;
#endif
    int memory_begin_;
    bool verbose_;
  };

  extern Commands gCommands;

}

#endif // TCL_COMMAND_H