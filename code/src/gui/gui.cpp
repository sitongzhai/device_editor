//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#include <stdlib.h>
#include <qsettings.h>
#include <qstyle.h>
#include <qdialogbuttonbox.h>
#include <qdir.h>


#include "gui/gui.h"
#include "gui/main_app.h"
#include "gui/main_window.h"
#include "gui/gui_utility.h"
#include "gui/console/command_line.h"
#include "gui/console/main_console.h"
#include "tcl/commands.h"
#include "utility/app.h"
#include "utility/log.h"
#include "utility/data_var.h"

namespace eda {

  MainApp* Gui::main_app_ = NULL;
  MainWindow* Gui::main_window_ = NULL;

  Tcl_Interp* Gui::interp_ = NULL;

  bool Gui::gui_mode_ = false;

  QSettings* Gui::setting_ = NULL;

  QString Gui::resource_path_ = ":";

  extern int registerAllCmds(Tcl_Interp* pInterp);
  void EDAReadLineLoop(Tcl_Interp* interp);

  void Gui::init(int argc, char* argv[]) {
    if (main_window() != NULL) return;
    if (argc < 1) return;

    Tcl_FindExecutable(argv[0]);

    initGui(argc, argv);

    Tcl_SetServiceMode(TCL_SERVICE_ALL);

    gui_mode_ = true;
    Tcl_Main(argc, argv, tclGuiInitProc);
  }
  void Gui::initGui(int& argc, char* argv[]) {
    resource_path_ = QDir(QString(App::getDBPath().c_str()) + QString("/icon/")).path();
    main_app_ = new MainApp(argc, argv);
    QString eda_app_name(QString("VTR")/* + QChar(0x2122)*/);
    main_app_->setApplicationName(eda_app_name);
    main_app_->setOrganizationName("Vaughn Betz's Group");
    QString eda_version = QString::fromStdString(EDAVersion);
    main_app_->setApplicationVersion(eda_version);

    setting_ = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Vaughn Betz's Group", "FPGAEDA");
    setting_->setValue("eda_gui_setting", "Program Config");
    setting_->sync();

    main_app_->setAttribute(Qt::AA_DontShowIconsInMenus, false);
    main_app_->setWindowIcon(QIcon(Gui::resource_path() + "/applogo.png"));

    main_app_->setWheelScrollLines(4);
    main_app_->setStyle("fusion");
    main_app_->setPalette(QApplication::style()->standardPalette());

    //others may be later
    return;
  }


  int Gui::tclGuiInitProc(Tcl_Interp* interp) {
    if (interp == NULL) {
      return TCL_ERROR;
    }

    if (TCL_ERROR == Tcl_Init(interp)) {
      return TCL_ERROR;
    }

#ifdef WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07);
    Tcl_Channel tcl_stdout, tcl_stdin, tcl_stderr;
    tcl_stdin = Tcl_OpenFileChannel(interp, "nul", "r", 0);
    tcl_stdout = Tcl_OpenFileChannel(interp, "nul", "w", 0);
    tcl_stderr = Tcl_OpenFileChannel(interp, "nul", "w", 0);
    Tcl_RegisterChannel(interp, tcl_stdout);
    Tcl_RegisterChannel(interp, tcl_stdin);
    Tcl_RegisterChannel(interp, tcl_stderr);
    Tcl_SetStdChannel(tcl_stdout, TCL_STDOUT);
    Tcl_SetStdChannel(tcl_stderr, TCL_STDERR);
    Tcl_SetStdChannel(tcl_stdin, TCL_STDIN);
#endif

    Commands::set_interp(interp);

    Gui::set_interp(interp);

    eda_info("FPGA GUI is running.\n");

    registerAllCmds(interp);

    main_window_ = new MainWindow();
    main_window_->show();

    if (main_app_->exec()) {
      EDAReadLineLoop(interp);
    } else {
      exit(0);
    }

    return TCL_OK;
  }



  int Gui::executeCmd(QString cmd, const bool print_result) {
    if (interp_ == NULL) {
      eda_error("No Tcl Interpret specified.\n");
      return -1;
    }
    if (cmd == "exit") {
      Console::done();
      return TCL_OK;
    }
    QStringList tokens = cmd.split(" ", QString::SkipEmptyParts);
    if (tokens.size()) {
      QString head = tokens.at(0);
      if (head.compare("cd", Qt::CaseInsensitive) == 0 ||
        head.compare("chdir", Qt::CaseInsensitive) == 0) {
        eda_info("Changing current directory is not allowed.\n");
        Console::done();
        return TCL_OK;
      }
    }
    if (Gui::main_window()->main_console()) {
      Gui::main_window()->command_line()->append(cmd);
    }
    const char* c_string_cmd = cmd.toLatin1().data();
    try {
      // send out notification before executing the command.
      if (Gui::main_app() && Gui::main_window()) {
        Gui::main_app()->postEvent(Gui::main_window(), new ExecutedCommandEvent(c_string_cmd));
      }
      int ret = Tcl_Eval(interp_, c_string_cmd);
      // send out notification after executing the command.
      if (Gui::main_app() && Gui::main_window()) {
        Gui::main_app()->postEvent(Gui::main_window(), new ExecutedCommandEvent(c_string_cmd, CMD_FINISHED));
      }
      const char* result = Tcl_GetStringResult(interp_);
      if (print_result && strlen(result) > 0) {
        if (TCL_OK != ret)
          eda_error("ERROR: TCL:%s\n", result);
        else
          eda_info("%s\n", result);
      }
      Tcl_ResetResult(interp_);
      // finish executeCmd, (prevent fake Gui terminal busy)
      Console::done();
      return ret;
    } catch (...) {
      eda_error("Command execution failed.\n");
      Console::done();
      return TCL_ERROR;
    }
  }






  bool Gui::confirmMessageBox(QWidget* caller, const QString& message) {
    QMessageBox confirm_box(caller);
    confirm_box.setObjectName("MSG");
    confirm_box.setWindowTitle("Are you sure?");
    confirm_box.setText(message);
    confirm_box.setIcon(QMessageBox::Question);
    confirm_box.addButton(QMessageBox::No);
    QPushButton* yes = confirm_box.addButton(QMessageBox::Yes);
    confirm_box.setDefaultButton(yes);
    confirm_box.exec();
    return confirm_box.clickedButton() == yes;
  }
  QMessageBox::StandardButton Gui::messageBox(QWidget* parent, QMessageBox::Icon icon,
    const QString& title, const QString& text,
    QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton) {
    QMessageBox message_box(icon, title, text, QMessageBox::NoButton, parent);
    message_box.setObjectName("MSG_BOX");
    QDialogButtonBox* button_box = message_box.findChild<QDialogButtonBox*>();
    Q_ASSERT(button_box != NULL);

    uint mask = QMessageBox::FirstButton;
    while (mask <= QMessageBox::LastButton) {
      uint standard_button = buttons & mask;
      mask <<= 1;
      if (!standard_button) continue;
      QPushButton* push_button = message_box.addButton((QMessageBox::StandardButton)standard_button);
      // Choose the first accept role as the default
      if (message_box.defaultButton())
        continue;
      if ((defaultButton == QMessageBox::NoButton && button_box->buttonRole(push_button) == QDialogButtonBox::AcceptRole) ||
        (defaultButton != QMessageBox::NoButton && standard_button == uint(defaultButton)))
        message_box.setDefaultButton(push_button);
    }
    if (message_box.exec() == -1)
      return QMessageBox::Cancel;
    return message_box.standardButton(message_box.clickedButton());
  }
  QString Gui::getExistingDirectory(QWidget* parent, const QString& caption, const QString& dir, QFileDialog::Options options) {
    QFileDialog file_selector(parent);
    file_selector.setWindowTitle(caption);
    file_selector.setDirectory(dir);
    file_selector.setOptions(options);
    file_selector.setOption(QFileDialog::DontUseNativeDialog);
    file_selector.setFileMode(QFileDialog::Directory);
    if (file_selector.exec() == QDialog::Accepted) {
      return file_selector.selectedFiles().front();
    }
    return QString();
  }
  QString Gui::getOpenFileName(QWidget* parent, const QString& caption, const QString& dir, const QString& filter, QString* selectedFilter, QFileDialog::Options options) {
    QFileDialog file_selector(parent);
    file_selector.setWindowTitle(caption);
    file_selector.setDirectory(dir);
    file_selector.setOptions(options);
    file_selector.setOption(QFileDialog::DontUseNativeDialog);
    file_selector.setNameFilter(filter);
    file_selector.setFileMode(QFileDialog::ExistingFile);
    if (selectedFilter) {
      file_selector.selectNameFilter(*selectedFilter);
    }
    file_selector.setAcceptMode(QFileDialog::AcceptOpen);
    if (file_selector.exec() == QDialog::Accepted) {
      return file_selector.selectedFiles().front();
    }
    return QString();
  }
  QStringList Gui::getOpenFileNames(QWidget* parent, const QString& caption, const QString& dir, const QString& filter, QString* selectedFilter, QFileDialog::Options options) {
    QFileDialog file_selector(parent);
    file_selector.setWindowTitle(caption);
    file_selector.setDirectory(dir);
    file_selector.setOptions(options);
    file_selector.setOption(QFileDialog::DontUseNativeDialog);
    file_selector.setNameFilter(filter);
    file_selector.setFileMode(QFileDialog::ExistingFiles);
    if (selectedFilter) {
      file_selector.selectNameFilter(*selectedFilter);
    }
    file_selector.setAcceptMode(QFileDialog::AcceptOpen);
    if (file_selector.exec() == QDialog::Accepted) {
      return file_selector.selectedFiles();
    }
    return QStringList();
  }
  QString Gui::getSaveFileName(QWidget* parent, const QString& caption, const QString& dir, const QString& filter, QString* selectedFilter, QFileDialog::Options options) {
    QFileDialog file_selector(parent);
    file_selector.setWindowTitle(caption);
    file_selector.setDirectory(dir);
    file_selector.setOptions(options);
    file_selector.setOption(QFileDialog::DontUseNativeDialog);
    file_selector.setNameFilter(filter);
    file_selector.setFileMode(QFileDialog::AnyFile);
    file_selector.setAcceptMode(QFileDialog::AcceptSave);
    if (selectedFilter && !selectedFilter->isEmpty())
      file_selector.selectNameFilter(*selectedFilter);
    if (file_selector.exec() == QDialog::Accepted) {
      if (selectedFilter)
        *selectedFilter = file_selector.selectedNameFilter();
      return file_selector.selectedFiles().front();
    }
    return QString();
  }
}