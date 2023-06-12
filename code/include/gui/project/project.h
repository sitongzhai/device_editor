//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_PROJECT_PROJECT_H
#define GUI_PROJECT_PROJECT_H

#include <QString>

namespace eda {
  class Project {
  private:
    static Project* project_;
    QString version_;
    QString project_name_;
    QString project_path_;
    QString description_;
    QString family_;
    QString device_;
    QString package_;
    QString speed_;
    QString xdl_file_;
    QString ori_xdl_file_;
    QString edif_file_;
    QString ori_edif_file_;
    QString blif_file_;
    QString ori_blif_file_;
    QString ucf_file_;
    QString ori_ucf_file_;
    QString sdc_file_;
    QString ori_sdc_file_;
    bool copy_source_files_;
    bool is_modified_;
    // FIXME: save all options used in each command of flow, 
    // but should be initialized with the arguments in the 
    // registered command automatically. DONOT forget save into 
    // project files
    //Option command_options_; 
  private:
    Project();
    ~Project();
  public:
    static Project* create();
    static Project* project();
    static void release();
    static Project* load(const QString& project_file_name);
    static QString suffix() { return ".prj"; }
    void save();
  public:
    void resetName();
    void resetDevice();
    void resetSources();
    QString version() { return version_; }
    void set_version(QString version) { 
      version_ = version; 
      is_modified_ = true;
    }
    QString project_name() { return project_name_; }
    void set_project_name(QString project_name) { 
      project_name_ = project_name;
      is_modified_ = true;
    }
    QString project_path() { return project_path_; }
    void set_project_path(QString project_path) { 
      project_path_ = project_path;
      is_modified_ = true;
    }
    QString description() { return description_; }
    void set_description(QString description) { 
      description_ = description;
      is_modified_ = true;
    }
    QString family() { return family_; }
    void set_family(QString family) { 
      family_ = family;
      is_modified_ = true;
    }
    QString device() { return device_; }
    void set_device(QString device) { 
      device_ = device;
      is_modified_ = true;
    }
    QString package() { return package_; }
    void set_package(QString package) { 
      package_ = package;
      is_modified_ = true;
    }
    QString speed() { return speed_; }
    void set_speed(QString speed) { 
      speed_ = speed;
      is_modified_ = true;
    }
    QString xdl_file() { return xdl_file_; }
    void set_xdl_file(QString xdl_file) { 
      xdl_file_ = xdl_file;
      is_modified_ = true;
    }
    QString ori_xdl_file() { return ori_xdl_file_; }
    void set_ori_xdl_file(QString ori_xdl_file) { 
      ori_xdl_file_ = ori_xdl_file;
      is_modified_ = true;
    }
    QString edif_file() { return edif_file_; }
    void set_edif_file(QString edif_file) { 
      edif_file_ = edif_file;
      is_modified_ = true;
    }
    QString ori_edif_file() { return ori_edif_file_; }
    void set_ori_edif_file(QString ori_edif_file) { 
      ori_edif_file_ = ori_edif_file;
      is_modified_ = true;
    }
    QString blif_file() { return blif_file_; }
    void set_blif_file(QString blif_file) { 
      blif_file_ = blif_file;
      is_modified_ = true;
    }
    QString ori_blif_file() { return ori_blif_file_; }
    void set_ori_blif_file(QString ori_blif_file) { 
      ori_blif_file_ = ori_blif_file;
      is_modified_ = true;
    }
    QString ucf_file() { return ucf_file_; }
    void set_ucf_file(QString ucf_file) { 
      ucf_file_ = ucf_file;
      is_modified_ = true;
    }
    QString ori_ucf_file() { return ori_ucf_file_; }
    void set_ori_ucf_file(QString ori_ucf_file) { 
      ori_ucf_file_ = ori_ucf_file;
      is_modified_ = true;
    }
    QString sdc_file() { return sdc_file_; }
    void set_sdc_file(QString sdc_file) { 
      sdc_file_ = sdc_file;
      is_modified_ = true;
    }
    QString ori_sdc_file() { return ori_sdc_file_; }
    void set_ori_sdc_file(QString ori_sdc_file) { 
      ori_sdc_file_ = ori_sdc_file;
      is_modified_ = true;
    }
    bool copy_source_files() { return copy_source_files_; }
    void set_copy_source_files(bool copy_source_files) { 
      copy_source_files_ = copy_source_files;
      is_modified_ = true;
    }
    bool is_modified() { return is_modified_; }
    void set_is_modified(bool is_modified) { is_modified_ = is_modified; }

    bool hasXdlFile() { return xdl_file_ != ""; }
    bool hasEdifFile() { return edif_file_ != ""; }
    bool hasBlifFile() { return blif_file_ != ""; }
    bool hasUcfFile() { return ucf_file_ != ""; }
    bool hasSdcFile() { return sdc_file_ != ""; }

  };

}

#endif // !GUI_PROJECT_PROJECT_H
