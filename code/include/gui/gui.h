//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_GUI_H
#define GUI_GUI_H

#include <tcl.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <qmessagebox.h>

class QSettings;

namespace eda {

  class MainApp;
  class MainWindow;

  class Gui {
  private:
    static MainWindow* main_window_;

    static MainApp* main_app_;
    static Tcl_Interp* interp_;

    static bool gui_mode_;
    static QSettings* setting_;

    static QString resource_path_;
  public:
    Gui() {}
    ~Gui() {}

    static MainApp* main_app() { return main_app_; }
    static MainWindow* main_window() { return main_window_; }


    static void set_interp(Tcl_Interp* interp) { interp_ = interp; }
    static Tcl_Interp* interp() { return interp_; }
    static int executeCmd(QString cmd, const bool print_relsult = false);
    static QSettings* setting() { return setting_; }


    static bool is_gui_mode() { return gui_mode_; }

    static QString resource_path() {
      return resource_path_;
    }
  public:
    static void init(int argc, char* argv[]);

    static void initGui(int& argc, char* argv[]);

    // The same function to TclInitProc, but enter QT loop first
    static int tclGuiInitProc(Tcl_Interp* interp);
  
  
  public:
    static bool confirmMessageBox(QWidget* caller, const QString& message);
    static QMessageBox::StandardButton messageBox(QWidget* parent, QMessageBox::Icon icon,
      const QString& title, const QString& text,
      QMessageBox::StandardButtons buttons = QMessageBox::StandardButton::Ok,
      QMessageBox::StandardButton defaultButton = QMessageBox::StandardButton::NoButton);
    static QString getExistingDirectory(QWidget* parent, const QString& caption = QString(), 
      const QString& dir = QString(), QFileDialog::Options options = QFileDialog::ShowDirsOnly);
    static QString getOpenFileName(QWidget* parent, const QString& caption = QString(), 
      const QString& dir = QString(), const QString& filter = QString(), QString* selectedFilter = NULL, QFileDialog::Options options = 0);
    static QStringList getOpenFileNames(QWidget* parent, const QString& caption = QString(), 
      const QString& dir = QString(), const QString& filter = QString(), QString* selectedFilter = NULL, QFileDialog::Options options = 0);
    static QString getSaveFileName(QWidget* parent, const QString& caption = QString(), 
      const QString& dir = QString(), const QString& filter = QString(), QString* selectedFilter = NULL, QFileDialog::Options options = 0);
  };

}

#endif // !GUI_GUI_H
