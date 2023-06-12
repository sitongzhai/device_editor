//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#include <qfile.h>
#include <qfileinfo.h>
#include <qsettings.h>
#include "gui/project/project.h"
#include "utility/log.h"

namespace eda {
  Project* Project::project_ = NULL;

  Project::Project() {
    version_ = "";
    project_name_ = "";
    project_path_ = "";
    description_ = "";
    family_ = "";
    device_ = "";
    package_ = "";
    speed_ = "";
    xdl_file_ = "";
    ori_xdl_file_ = "";
    edif_file_ = "";
    ori_edif_file_ = "";
    blif_file_ = "";
    ori_blif_file_ = "";
    ucf_file_ = "";
    ori_ucf_file_ = "";
    sdc_file_ = "";
    ori_sdc_file_ = "";
    copy_source_files_ = false;
    is_modified_ = false;
  }
  Project::~Project() {

  }
  void Project::resetName() {
    project_name_ = "";
    project_path_ = "";
    description_ = "";
  }
  void Project::resetDevice() {
    family_ = "";
    device_ = "";
    package_ = "";
    speed_ = "";
  }
  void Project::resetSources() {
    xdl_file_ = "";
    ori_xdl_file_ = "";
    edif_file_ = "";
    ori_edif_file_ = "";
    blif_file_ = "";
    ori_blif_file_ = "";
    ucf_file_ = "";
    ori_ucf_file_ = "";
    sdc_file_ = "";
    ori_sdc_file_ = "";
    copy_source_files_ = false;
  }
  Project* Project::create() {
    if (project_ != NULL) delete project_;
    project_ = new Project();
    return project_;
  }
  Project* Project::project() {
    return project_;
  }
  void Project::release() {
    if (project_ != NULL) {
      delete project_;
      project_ = NULL;
    }
  }
  Project* Project::load(const QString& project_file_name) {
    if (!QFile::exists(project_file_name)) {
      return NULL;
    }

    QSettings project_setting(project_file_name, QSettings::IniFormat, NULL);
    project_setting.beginGroup("EDATool");
    QString version = project_setting.value("Version").toString();
    project_setting.endGroup();

    project_setting.beginGroup("Project");
    QString project_name = project_setting.value("Project_Name").toString();
    QString project_path = project_setting.value("Project_Path").toString();
    QString description = project_setting.value("Description", "").toString();
    project_setting.endGroup();

    project_setting.beginGroup("Device");
    QString family = project_setting.value("Family").toString();
    QString device = project_setting.value("Device").toString();
    QString package = project_setting.value("Package").toString();
    QString speed = project_setting.value("Speed").toString();
    project_setting.endGroup();

    project_setting.beginGroup("Source_Files");
    bool copy_source_files = project_setting.value("Copy_Source").toBool();
    QString xdl_file = project_setting.value("XDL").toString();
    QString ori_xdl_file = project_setting.value("Ori_XDL").toString();
    QString edif_file = project_setting.value("EDIF").toString();
    QString ori_edif_file = project_setting.value("Ori_EDIF").toString();
    QString blif_file = project_setting.value("BLIF").toString();
    QString ori_blif_file = project_setting.value("Ori_BLIF").toString();
    QString ucf_file = project_setting.value("UCF").toString();
    QString ori_ucf_file = project_setting.value("Ori_UCF").toString();
    QString sdc_file = project_setting.value("SDC").toString();
    QString ori_sdc_file = project_setting.value("Ori_SDC").toString();
    project_setting.endGroup();

    int netlist_file_count = 0;
    if (xdl_file != "") {
      netlist_file_count++;
    }
    if (edif_file != "") {
      netlist_file_count++;
    }
    if (blif_file != "") {
      netlist_file_count++;
    }
    if (netlist_file_count > 1) {
      eda_error("Illegal project!\n");
      return NULL;
    }

    create();
    project_->set_version(version);
    project_->set_project_name(project_name);
    project_->set_project_path(project_path);
    project_->set_description(description);
    project_->set_family(family);
    project_->set_device(device);
    project_->set_package(package);
    project_->set_speed(speed);
    project_->set_copy_source_files(copy_source_files);
    project_->set_xdl_file(xdl_file);
    project_->set_ori_xdl_file(ori_xdl_file);
    project_->set_edif_file(edif_file);
    project_->set_ori_edif_file(ori_edif_file);
    project_->set_blif_file(blif_file);
    project_->set_ori_blif_file(ori_blif_file);
    project_->set_ucf_file(ucf_file);
    project_->set_ori_ucf_file(ori_ucf_file);
    project_->set_sdc_file(sdc_file);
    project_->set_ori_sdc_file(ori_sdc_file);
    project_->set_is_modified(false);

    return project_;
  }
  void Project::save() {
    QString project_file_name = project_path_ + "/" + project_name_ + suffix();
    QFileInfo project_file_info(project_file_name);
    if (project_file_info.exists() && project_file_info.isWritable()) {
      QFile project_file(project_file_name);
      project_file.open(QFile::ReadWrite);
      project_file.resize(0);
      project_file.close();
    }
    QSettings project_setting(project_file_name, QSettings::IniFormat, NULL);

    project_setting.beginGroup("EDATool");
    project_setting.setValue("Version", version_);
    project_setting.endGroup();

    project_setting.beginGroup("Project");
    project_setting.setValue("Project_Name", project_name_);
    project_setting.setValue("Project_Path", project_path_);
    project_setting.setValue("Description", description_);
    project_setting.endGroup();

    project_setting.beginGroup("Device");
    project_setting.setValue("Family", family_);
    project_setting.setValue("Device", device_);
    project_setting.setValue("Package", package_);
    project_setting.setValue("Speed", speed_);
    project_setting.endGroup();

    project_setting.beginGroup("Source_Files");
    project_setting.setValue("Copy_Source", copy_source_files_);
    project_setting.setValue("XDL", xdl_file_);
    project_setting.setValue("Ori_XDL", ori_xdl_file_);
    project_setting.setValue("EDIF", edif_file_);
    project_setting.setValue("Ori_EDIF", ori_edif_file_);
    project_setting.setValue("BLIF", blif_file_);
    project_setting.setValue("Ori_BLIF", ori_blif_file_);
    project_setting.setValue("UCF", ucf_file_);
    project_setting.setValue("Ori_UCF", ori_ucf_file_);
    project_setting.setValue("SDC", sdc_file_);
    project_setting.setValue("Ori_SDC", ori_sdc_file_);
    project_setting.endGroup();

    //FIXME: save options later

    project_setting.sync();
  }
}