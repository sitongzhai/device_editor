//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_MAIN_WINDOW_H
#define GUI_MAIN_WINDOW_H

#include <qobject.h>
#include <qsettings.h>
#include <qmainwindow.h>
#include <qmdiarea.h>
#include <qurl.h>

#include "gui/gui_utility.h"



namespace eda {

  class LayoutWindow;
  
  class CommandLine;
  class MainConsole;

  class ProjectWidget;
  class EDAMdiSubWindow;

  class MainWindow : public ::QMainWindow {

    enum kDesktopSize {
      kTiny = 0,
      kSmall = 1,
      kNormal = 2,
      kBig = 3,
      kHuge = 4
    };

    Q_OBJECT

  public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    MainConsole* main_console() { return main_console_; }
    CommandLine* command_line() { return command_line_; }

    void setCurrentProject(Project* project);

  protected:

    void initLayout();
    void createActions();
    void createMenuBar();
    void createStartDock();
    void createProjectWidget();
    void createProjectTabWindow();
    void createConsoleDock();

  protected slots:
    bool onOpenProject(const QString& filename = "");
    void onNewProject();
    void onSaveProject();
    void onCloseProject();
    void clearConsoleTab();
    void onLayout(bool flag);
    void onStartReport();
    void onViewConsoleLogFile();
    void onWarningChanged(int count);
    void onErrorChanged(int count);
    void onSetConsoleDockVisible(bool flag);
    void onSetProjectDockVisible(bool flag);
    void onSetMdiWindowVisible(bool flag);

  private:
    QMdiArea* mdi_central_;
    EDAMdiSubWindow* project_tabs_subwindow_;
    QMenuBar* menu_bar_;
    QListWidget* warning_list_;
    QListWidget* error_list_;
    MainConsole* main_console_;
    QTabWidget* console_tab_;
    CommandLine* command_line_;
    QDockWidget* dock_console_;

    QString init_path_;
    QTabWidget* main_tab_;
    QDockWidget* dock_start_;
    ProjectWidget* project_widget_;


    QTabWidget* project_tab_;

    //project
    QAction* act_open_project_;
    QAction* act_new_project_;
    QAction* act_save_project_;
    QAction* act_close_project_;
    QAction* act_window_device_layout_;
    QAction* act_window_main_tab_;
    //console
    QAction* act_show_dock_cmd_;
    QAction* act_show_dock_start_;
    LayoutWindow* layout_window_;
  };

}


#endif // !GUI_MAIN_WINDOW_H
