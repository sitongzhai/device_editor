//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <qstandardpaths.h>
#include <qsettings.h>
#include <qfiledialog.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qmessagebox.h>

#include "gui/project/project.h"
#include "gui/project/new_project_wizard.h"
#include "gui/project/source_file_selector.h"
#include "device/device_manager.h"
#include "utility/log.h"

namespace eda {

  NewProjectWizard::NewProjectWizard(QWidget* parent) : QWizard(parent) {
    setWindowTitle("New Project Wizard");
    setFixedSize(640, 520);
    name_page_ = NULL;
    device_page_ = NULL;
    source_page_ = NULL;
    conclusion_page_ = NULL;
    setWizardStyle(QWizard::ClassicStyle);
    setOption(QWizard::IndependentPages, false);
    setOption(QWizard::NoBackButtonOnStartPage, true);
    setOption(QWizard::HaveNextButtonOnLastPage, false);
    setOption(QWizard::NoCancelButton, true);
    button(QWizard::NextButton)->setObjectName("NEXT_BTN");
    button(QWizard::BackButton)->setObjectName("BACK_BTN");
    button(QWizard::FinishButton)->setObjectName("FINISH_BTN");

    project_ = Project::create();
    name_page_ = new NamePage(this, project_);
    device_page_ = new DevicePage(this, project_);
    source_page_ = new SourcePage(this, project_);
    conclusion_page_ = new ConclusionPage(this, project_);
    conclusion_page_->setCommitPage(true);

    setPage(0, name_page_);
    setPage(1, device_page_);
    setPage(2, source_page_);
    setPage(3, conclusion_page_);
  }
  NewProjectWizard::~NewProjectWizard() {
    
  }
  NewProjectWizard::NamePage::NamePage(QWidget* parent, Project* project) : QWizardPage(parent) {
    project_ = project;

    QVBoxLayout* vbox_layout = new QVBoxLayout();
    vbox_layout->setSpacing(2);
    vbox_layout->setContentsMargins(9, 0, 9, 9);
    setTitle("<b>Create New Project</b>");
    QGroupBox* group = new QGroupBox(this);
    group->setCheckable(false);
    QGridLayout* grid = new QGridLayout();
    grid->setContentsMargins(9, 0, 9, 9);
    grid->setSpacing(4);
    QLabel* project_name_label = new QLabel("Name:", this);
    QLabel* location_label = new QLabel("Location:", this);
    QLabel* description_label = new QLabel("Description:", this);
    name_edit_ = new QLineEdit(this);
    name_edit_->setPlaceholderText("Enter project name ...");
    QRegExp rx("[a-zA-Z_][a-zA-Z0-9_]*");
    QValidator* validator = new QRegExpValidator(rx, this);
    name_edit_->setValidator(validator);

    connect(name_edit_, SIGNAL(textChanged(const QString&)), this, SIGNAL(completeChanged()));
    connect(name_edit_, SIGNAL(textChanged(const QString&)), this, SLOT(onNameEditChanged(const QString&)));

    path_edit_ = new QLineEdit(this);

    QString default_loc = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    default_loc += "/Project";
    default_loc = QDir::fromNativeSeparators(default_loc);

    QSettings* setting = Gui::setting();
    setting->beginGroup("Project");
    QString last_loc = setting->value("last_location", default_loc).toString();
    setting->endGroup();

    path_edit_->setText(last_loc);
    connect(path_edit_, SIGNAL(textChanged(const QString&)), this, SIGNAL(completeChanged()));
    connect(path_edit_, SIGNAL(textEdited(const QString&)), this, SLOT(onProjectPathEdited(const QString&)));

    path_str_ = last_loc;
    QPushButton* location_dir_btn = new QPushButton("...", this);
    location_dir_btn->setFixedWidth(30);

    description_edit_ = new QTextEdit(this);
    description_edit_->setFixedHeight(255);

    QHBoxLayout* hbox_layout = new QHBoxLayout;
    hbox_layout->addWidget(path_edit_);
    hbox_layout->addWidget(location_dir_btn);

    connect(location_dir_btn, SIGNAL(clicked()), this, SLOT(onClickDirectionButton()));

    grid->addWidget(project_name_label, 0, 0);
    grid->addWidget(location_label, 1, 0);
    grid->addWidget(description_label, 2, 0);

    grid->addWidget(name_edit_, 0, 1);
    grid->addLayout(hbox_layout, 1, 1);
    grid->addWidget(description_edit_, 2, 1);
    group->setLayout(grid);

    vbox_layout->addWidget(group);
    setLayout(vbox_layout);
    
    project_->resetName();
  }
  bool NewProjectWizard::NamePage::isComplete() const {
    QFileInfo info(path_str_);
    // in linux and windows, the maximum length of name of a file is 255 including the suffix
    return info.exists() && info.isDir() && info.isWritable() &&
      !name_edit_->displayText().isEmpty() && name_edit_->displayText().length() < 255; 
  }
  void NewProjectWizard::NamePage::onClickDirectionButton() {
    QFileDialog::Options dialog_options = QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks;
    QString pre_path = Gui::getExistingDirectory(this, tr("Open Directory"), path_str_, dialog_options);
    pre_path = QDir::fromNativeSeparators(pre_path);
    if (!pre_path.isEmpty()) {
      path_str_ = pre_path;
      path_edit_->setText(path_str_);
    }
  }
  void NewProjectWizard::NamePage::onNameEditChanged(const QString& name) {
    while (true) {
      if (path_str_.endsWith('/')) {
        path_str_.chop(1);
      } else {
        break;
      }
    }
    QString new_path_str = path_str_ + "/" + name;
    path_edit_->setText(new_path_str);
  }
  void NewProjectWizard::NamePage::onProjectPathEdited(const QString& name) {
    path_str_ = QDir::fromNativeSeparators(name);
    path_str_.replace('\\', '/');
    path_edit_->setText(path_str_);
  }
  bool NewProjectWizard::NamePage::validatePage() {
    QString project_path = path_edit_->text();
    QString project_name = name_edit_->text();
    QString description = description_edit_->toPlainText();

    bool overwrite_flag = false;
    bool readonly_flag = false;

    QDir project_dir(project_path);
    if (project_dir.exists()) {
      overwrite_flag = Gui::confirmMessageBox(this, "Folder already exists, continue?");
    } else {
      overwrite_flag = true;
    }

    if (!QFileInfo(project_path).isWritable()) {
      readonly_flag = false;
      Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", "The project path is read-only.");
    } else {
      readonly_flag = true;
    }

    project_->set_project_name(project_name);
    project_->set_project_path(project_path);
    project_->set_description(description);

    return readonly_flag && overwrite_flag;
  }
  NewProjectWizard::DevicePage::DevicePage(QWidget* parent, Project* project) : QWizardPage(parent) {
    project_ = project;

    setTitle("<b>Project Settings</b>");
    QGroupBox* group = new QGroupBox("Select device for the project", this);
    group->setObjectName("DEVICE_GROUP");
    QGridLayout* grid = new QGridLayout;
    grid->addWidget(new QLabel("Family", this), 1, 0);
    grid->addWidget(new QLabel("Device", this), 2, 0);
    grid->addWidget(new QLabel("Package", this), 3, 0);
    grid->addWidget(new QLabel("Speed", this), 4, 0);

    family_combo_ = new QComboBox(this);
    family_combo_->setObjectName("FAMILY_COMBO");
    family_combo_->setCurrentIndex(0);
    std::vector<std::string> families = DeviceManager::manager()->families();
    for (size_t f = 0; f < families.size(); f++) {
      family_combo_->addItem(families[f].c_str());
    }

    device_combo_ = new QComboBox(this);
    device_combo_->setObjectName("DEVICE_COMBO");
    device_combo_->setCurrentIndex(1);
    std::vector<DeviceManager::DeviceDef> devices = DeviceManager::manager()->devices()[0];
    for (size_t d = 0; d < devices.size(); d++) {
      device_combo_->addItem(devices[d].name.c_str());
    }

    package_combo_ = new QComboBox(this);
    package_combo_->setObjectName("PACKAGE_COMBO");
    package_combo_->setCurrentIndex(2);
    DeviceManager::DeviceDef device = DeviceManager::manager()->devices()[0][0];
    for (size_t p = 0; p < device.packages.size(); p++) {
      package_combo_->addItem(device.packages[p].c_str());
    }

    speed_combo_ = new QComboBox(this);
    speed_combo_->setObjectName("SPEED_COMBO");
    speed_combo_->setCurrentIndex(3);
    for (size_t s = 0; s < device.speeds.size(); s++) {
      speed_combo_->addItem(device.speeds[s].c_str());
    }

    grid->addWidget(family_combo_, 1, 1);
    grid->addWidget(device_combo_, 2, 1);
    grid->addWidget(package_combo_, 3, 1);
    grid->addWidget(speed_combo_, 4, 1);
    group->setLayout(grid);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(9, 0, 9, 9);
    setLayout(layout);
    layout->addWidget(group);

    connect(family_combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(onDeviceFamilyChanged(int)));
    connect(device_combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(onDeviceNameChanged(int)));

    connect(family_combo_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(completeChanged()));
    connect(device_combo_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(completeChanged()));
    connect(package_combo_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(completeChanged()));
    connect(speed_combo_, SIGNAL(currentIndexChanged(int)), this, SIGNAL(completeChanged()));
    
    project_->resetDevice();
  }
  bool NewProjectWizard::DevicePage::isComplete() const {
    QString family = family_combo_->currentText();
    if (family == "") {
      return false;
    }
    QString device = device_combo_->currentText();
    if (device == "") {
      return false;
    }
    QString package = package_combo_->currentText();
    if (package == "") {
      return false;
    }
    QString speed = speed_combo_->currentText();
    if (speed == "") {
      return false;
    }
    return true;
  }
  void NewProjectWizard::DevicePage::onDeviceFamilyChanged(int index) {
    device_combo_->clear();
    std::vector<std::vector<DeviceManager::DeviceDef>> devices = DeviceManager::manager()->devices();
    if (index >= 0 && index < (int)devices.size()) {
      for (size_t d = 0; d < devices[index].size(); d++) {
        device_combo_->addItem(devices[index][d].name.c_str());
      }
    }
  }
  void NewProjectWizard::DevicePage::onDeviceNameChanged(int index) {
    package_combo_->clear();
    speed_combo_->clear();
    int family_index = family_combo_->currentIndex();
    if (family_index < 0) return;
    std::vector<DeviceManager::DeviceDef> devices = DeviceManager::manager()->devices()[family_index];
    if (index >= 0 && index < (int)devices.size()) {
      for (size_t p = 0; p < devices[index].packages.size(); p++) {
        package_combo_->addItem(devices[index].packages[p].c_str());
      }
      for (size_t s = 0; s < devices[index].speeds.size(); s++) {
        speed_combo_->addItem(devices[index].speeds[s].c_str());
      }
    }
  }
  bool NewProjectWizard::DevicePage::validatePage() {
    QString family = family_combo_->currentText();
    QString device = device_combo_->currentText();
    QString package = package_combo_->currentText();
    QString speed = speed_combo_->currentText();

    project_->set_family(family);
    project_->set_device(device);
    project_->set_package(package);
    project_->set_speed(speed);

    return true;
  }
  void NewProjectWizard::DevicePage::cleanupPage() {
    project_->resetName();
  }
  NewProjectWizard::SourcePage::SourcePage(QWidget* parent, Project* project) : QWizardPage(parent) {
    project_ = project;

    setTitle("Assign source file");
    setSubTitle("Support XDL, BLIF and EDIF files for netlists.\nSupport SDC and UCF files for constraints.");

    file_selector_ = new SourceFileSelector(this);
    file_selector_->setObjectName("FILE_LIST_SELECTOR");
    file_selector_->set_pwd(".");
    QString file_filter = "Source (*.xdl *.blif *.edf *.edif);;Constraint (*.ucf *.sdc)";
    file_selector_->set_filter(file_filter);
    connect(file_selector_, SIGNAL(listChanged(const QStringList&)), this, SIGNAL(completeChanged()));

    copy_file_box_ = new QCheckBox(tr("Copy files into project"), this);
    copy_file_box_->setChecked(true);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(9, 0, 9, 9);
    layout->setSpacing(4);
    layout->addWidget(new QLabel("Select source File"));
    layout->addWidget(file_selector_);
    layout->addWidget(copy_file_box_);

    setLayout(layout);

    project_->resetSources();
  }
  bool NewProjectWizard::SourcePage::validatePage() {
    QString project_path = project_->project_path();
    bool copy_source_files = copy_file_box_->isChecked();

    bool validate = true;
    int netlist_file_count = 0;
    QStringList files = file_selector_->files();
    foreach(QString file, files) {
      if (file.endsWith(".xdl")) {
        if (project_->hasXdlFile()) {
          Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", "Only one xdl file can be specified!");
          validate = false;
          break;
        } else {
          netlist_file_count++;
          project_->set_ori_xdl_file(file);
          if (copy_source_files) {
            QString new_xdl = project_path + '/' + QFileInfo(file).fileName();
            project_->set_xdl_file(new_xdl);
          } else {
            project_->set_xdl_file(file);
          }
        }
      } else if (file.endsWith(".edif") || file.endsWith(".edf")) {
        if (project_->hasEdifFile()) {
          Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", "Only one edif file can be specified!");
          validate = false;
          break;
        } else {
          netlist_file_count++;
          project_->set_ori_edif_file(file);
          if (copy_source_files) {
            QString new_edif = project_path + '/' + QFileInfo(file).fileName();
            project_->set_edif_file(new_edif);
          } else {
            project_->set_edif_file(file);
          }
        }
      } else if (file.endsWith(".blif")) {
        if (project_->hasBlifFile()) {
          Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", "Only one blif file can be specified!");
          validate = false;
          break;
        } else {
          netlist_file_count++;
          project_->set_ori_blif_file(file);
          if (copy_source_files) {
            QString new_blif = project_path + '/' + QFileInfo(file).fileName();
            project_->set_blif_file(new_blif);
          } else {
            project_->set_blif_file(file);
          }
        }
      } else if (file.endsWith(".ucf")) {
        project_->set_ori_ucf_file(file);
        if (copy_source_files) {
          QString new_ucf = project_path + '/' + QFileInfo(file).fileName();
          project_->set_ucf_file(new_ucf);
        } else {
          project_->set_ucf_file(file);
        }
      } else if (file.endsWith(".sdc")) {
        project_->set_ori_sdc_file(file);
        if (copy_source_files) {
          QString new_sdc = project_path + '/' + QFileInfo(file).fileName();
          project_->set_sdc_file(new_sdc);
        } else {
          project_->set_sdc_file(file);
        }
      }
    }
    if (netlist_file_count > 1) {
      validate = false;
      Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", "Only one netlist file can be specified!");
    }
    if (!validate) {
      project_->resetSources();
      return false;
    } else {
      project_->set_copy_source_files(copy_source_files);
      return true;
    }
  }
  void NewProjectWizard::SourcePage::cleanupPage() {
    project_->resetDevice();
  }
  NewProjectWizard::ConclusionPage::ConclusionPage(QWidget* parent, Project* project) : QWizardPage(parent) {
    project_ = project;

    setTitle("<b>Project Summary</b>");
    setSubTitle("Project Navigator will create a new project with the following settings:");

    QVBoxLayout* vbox_layout = new QVBoxLayout();
    vbox_layout->setContentsMargins(9, 0, 9, 9);
    setLayout(vbox_layout);
    conclusion_text_ = new QTextEdit(this);
    conclusion_text_->setObjectName("CONCLUSION_EDIT");
    conclusion_text_->setReadOnly(true);
    vbox_layout->addWidget(conclusion_text_);
  }
  NewProjectWizard::ConclusionPage::~ConclusionPage() {
    if (project_) Project::release();
    project_ = NULL;
  }
  bool NewProjectWizard::ConclusionPage::validatePage() {

    QString project_path = project_->project_path();

    bool overwrite = false;
    bool writable = false;
    QDir project_dir(project_path);
    if (project_dir.exists()) {
      overwrite = Gui::confirmMessageBox(this, "Folder already exists, continue?");
    } else {
      project_dir.mkpath(project_path);
      overwrite = true;
    }

    if (!overwrite) {
      return false;
    }

    if (!QFileInfo(project_path).isWritable()) {
      writable = false;
      Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", "The project path is read-only.");
    } else {
      writable = true;
    }
    
    if (!writable) {
      return false;
    }

    if (project_->copy_source_files()) {
      if (project_->hasXdlFile()) {
        QString ori_xdl_file = project_->ori_xdl_file();
        QString xdl_file = project_->xdl_file();
        if (ori_xdl_file != xdl_file) {
          if (QFile::exists(xdl_file))
            QFile::remove(xdl_file);
          if (!QFile(ori_xdl_file).copy(xdl_file)) {
            QString error_message = QString("Failed to copy file '%1' to file '%2'!").arg(ori_xdl_file).arg(xdl_file);
            Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", error_message);
            return false;
          }
        }
      }
      if (project_->hasEdifFile()) {
        QString ori_edif_file = project_->ori_edif_file();
        QString edif_file = project_->edif_file();
        if (ori_edif_file != edif_file) {
          if (QFile::exists(edif_file))
            QFile::remove(edif_file);
          if (!QFile::copy(ori_edif_file, edif_file)) {
            QString error_message = QString("Failed to copy file '%1' to file '%2'!").arg(ori_edif_file).arg(edif_file);
            Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", error_message);
            return false;
          }
        }
      }
      if (project_->hasBlifFile()) {
        QString ori_blif_file = project_->ori_blif_file();
        QString blif_file = project_->blif_file();
        if (ori_blif_file != blif_file) {
          if (QFile::exists(blif_file))
            QFile::remove(blif_file);
          if (!QFile::copy(ori_blif_file, blif_file)) {
            QString error_message = QString("Failed to copy file '%1' to file '%2'!").arg(ori_blif_file).arg(blif_file);
            Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", error_message);
            return false;
          }
        }
      }
      if (project_->hasUcfFile()) {
        QString ori_ucf_file = project_->ori_ucf_file();
        QString ucf_file = project_->ucf_file();
        if (ori_ucf_file != ucf_file) {
          if (QFile::exists(ucf_file))
            QFile::remove(ucf_file);
          if (!QFile::copy(ori_ucf_file, ucf_file)) {
            QString error_message = QString("Failed to copy file '%1' to file '%2'!").arg(ori_ucf_file).arg(ucf_file);
            Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", error_message);
            return false;
          }
        }
      }
      if (project_->hasSdcFile()) {
        QString ori_sdc_file = project_->ori_sdc_file();
        QString sdc_file = project_->sdc_file();
        if (ori_sdc_file != sdc_file) {
          if (QFile::exists(sdc_file)) 
            QFile::remove(sdc_file);
          if (!QFile::copy(ori_sdc_file, sdc_file)) {
            QString error_message = QString("Failed to copy file '%1' to file '%2'!").arg(ori_sdc_file).arg(sdc_file);
            Gui::messageBox(this, QMessageBox::Icon::Critical, "ERROR", error_message);
            return false;
          }
        }
      }
    }
    project_->set_version(EDAVersion.c_str());
    project_->save();
    
    project_ = NULL;

    return true;
  }
  void NewProjectWizard::ConclusionPage::initializePage() {

    const QString break_line = "<br />";
    QString project_summary;
    project_summary += "<dl>";
    project_summary += "<dt><font size=\"4\"><b>Project Info:</font></b></dt></ br>";
    project_summary += "<dd><b>Project Name:</font></b>  " + project_->project_name() + "</dd>";
    project_summary += "<dd><b>Project File Location:</b>  " + project_->project_path() + "</dd>";
    project_summary += "<dd><b>Project Description:</b>  " + project_->description() + "</dd>";

    project_summary += break_line;
    project_summary += "<dt><font size=\"4\"><b>Device Info:</b><dt></ br>";
    project_summary += "<dd><b>Family:</b>  " + project_->family() + "</dd>";
    project_summary += "<dd><b>Device:</b>  " + project_->device() + "</dd>";
    project_summary += "<dd><b>Package:</b>  " + project_->package() + "</dd>";
    project_summary += "<dd><b>Speed:</b>  " + project_->speed() + "</dd>";

    project_summary += break_line;
    project_summary += "<dt><font size=\"4\"><b>File(s) Info:</b></dt></ br>";
    if (project_->hasXdlFile()) {
      project_summary += "<dd><b>XDL file:</b>  " + project_->xdl_file() + "</dd>";
    }
    if (project_->hasEdifFile()) {
      project_summary += "<dd><b>EDIF file:</b>  " + project_->edif_file() + "</dd>";
    }
    if (project_->hasBlifFile()) {
      project_summary += "<dd><b>BLIF file:</b>  " + project_->blif_file() + "</dd>";
    }
    if (project_->hasUcfFile()) {
      project_summary += "<dd><b>UCF file:</b>  " + project_->ucf_file() + "</dd>";
    }
    if (project_->hasSdcFile()) {
      project_summary += "<dd><b>SDC file:</b>  " + project_->sdc_file() + "</dd>";
    }

    project_summary += break_line;
    if (project_->copy_source_files()) {
      project_summary += "<dt><font size=\"4\"><b>Original File(s) Info:</b></dt></ br>";
      if (project_->hasXdlFile() && project_->ori_xdl_file() != project_->xdl_file()) {
        project_summary += "<dd><b>XDL file:</b>  " + project_->ori_xdl_file() + "</dd>";
      }
      if (project_->hasEdifFile() && project_->ori_edif_file() != project_->edif_file()) {
        project_summary += "<dd><b>EDIF file:</b>  " + project_->ori_edif_file() + "</dd>";
      }
      if (project_->hasBlifFile() && project_->ori_blif_file() != project_->blif_file()) {
        project_summary += "<dd><b>BLIF file:</b>  " + project_->ori_blif_file() + "</dd>";
      }
      if (project_->hasUcfFile() && project_->ori_ucf_file() != project_->ucf_file()) {
        project_summary += "<dd><b>UCF file:</b>  " + project_->ori_ucf_file() + "</dd>";
      }
      if (project_->hasSdcFile() && project_->ori_sdc_file() != project_->sdc_file()) {
        project_summary += "<dd><b>SDC file:</b>  " + project_->ori_sdc_file() + "</dd>";
      }
    }

    project_summary += "</dl>";
    project_summary += "<br />";

    conclusion_text_->setHtml(project_summary);
  }
  void NewProjectWizard::ConclusionPage::cleanupPage() {
    project_->resetSources();
  }
}