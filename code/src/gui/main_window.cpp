//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//* class for storing context information for the running command and notify
//* the current progress of the running command. The running command is supposed
//* to check the status of the thread it resides regularly using the function
//* threadStopped before doing the next step, start the progress indicator using
//* the function startProgressIndicator and update the progress using setProgress.
//******************************************************************************
#include <qdesktopwidget.h>
#include <qapplication.h>
#include <qaction.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qboxlayout.h>
#include <qpainter.h>

#include "gui/gui.h"
#include "gui/main_app.h"
#include "gui/main_event.h"
#include "gui/main_window.h"
#include "gui/layout/layout_window.h"
#include "gui/console/command_line.h"
#include "gui/console/main_console.h"
#include "gui/console/command_executor.h"
#include "gui/project/new_project_wizard.h"
#include "gui/project/project_widget.h"
#include "gui/project/mdi_subwindow.h"
#include "utility//log.h"


namespace eda {

  MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    init_path_ = QDir::currentPath();
    layout_window_ = NULL;
    setObjectName("EDA_MAINWINDOW");
    initLayout();
    EventDispatcher::instance()->subscribeEvent(this, GlobalEvent::kEventOptionChanged);
    EventDispatcher::instance()->subscribeEvent(this, GlobalEvent::kEventLabelWorkFinish);
    EventDispatcher::instance()->subscribeEvent(this, GlobalEvent::kEventProjectOpened);
    EventDispatcher::instance()->subscribeEvent(this, GlobalEvent::kEventCommandFinish);
    connect(this, SIGNAL(destroyed(QObject*)), EventDispatcher::instance(), SLOT(cleanup(QObject*)));
  }

  MainWindow::~MainWindow() {

  }

  void MainWindow::setCurrentProject(Project* project) {
    //FIXME:unfinished
    Gui::main_app()->setBusy(true);
    Gui::main_app()->setBusy(false);
    EventDispatcher::instance()->broadcastEvent(GlobalEvent::kEventOptionInit);
    QString project_path = init_path_;
    if (project != NULL) {
      project_path = project->project_path();
    }
    if (!QDir::setCurrent(project_path)) {
      return;
    }
    //FIXME: set_device, pack, place, route.... do one by one

    project_widget_->setProject(project);

  }
  void MainWindow::createMenuBar() {
    menu_bar_ = new QMenuBar(this);
    setMenuBar(menu_bar_);

    QMenu* project = menu_bar_->addMenu("&Project");
    project->addAction(act_open_project_);
    project->addAction(act_new_project_);
    project->addAction(act_save_project_);
    project->addAction(act_close_project_);

    QMenu* tools = menu_bar_->addMenu("&Tools");
    tools->addAction(act_window_device_layout_);
    tools->addAction(act_window_main_tab_);


    QMenu* window = menu_bar_->addMenu("&Window");
    window->addAction(act_show_dock_start_);
    window->addAction(act_show_dock_cmd_);

  }
  
  void MainWindow::initLayout() {

    mdi_central_ = new QMdiArea(this);
    mdi_central_->setOption(QMdiArea::DontMaximizeSubWindowOnActivation);
    setCentralWidget(mdi_central_);

    createActions();
    createMenuBar();

    createProjectTabWindow();

    createConsoleDock();
    createStartDock();

  }

  void MainWindow::onSetMdiWindowVisible(bool flag) {
    if (flag) {
      project_tabs_subwindow_->showMaximized();
    } else {
      project_tabs_subwindow_->close();
    }
    return;
  }
  void MainWindow::createStartDock() {
    createProjectWidget();

    dock_start_ = new QDockWidget("Welcome to VTR", this);
    dock_start_->setObjectName("EDA_START_DOCKWIDGET");
    connect(dock_start_, SIGNAL(visibilityChanged(bool)), act_show_dock_start_, SLOT(setChecked(bool)));
    dock_start_->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dock_start_->setWindowFlags(Qt::WindowType::WindowTitleHint);
    dock_start_->setContextMenuPolicy(Qt::NoContextMenu);
    dock_start_->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable);
    dock_start_->setStyleSheet("QDockWidget {color:black; font-size:12px; border:2px solid gray; height:10px;}" + QString("QDockWidget::title {text-align:center}"));
    main_tab_ = new QTabWidget(this);
    main_tab_->setObjectName("EDA_START_TABWIDGET");
    main_tab_->setTabPosition(QTabWidget::South);
    main_tab_->addTab(project_widget_, "Project");
    main_tab_->setDisabled(true);
    dock_start_->setWidget(main_tab_);
    addDockWidget(Qt::LeftDockWidgetArea, dock_start_, Qt::Vertical);
  }
  void MainWindow::createProjectWidget() {
    project_widget_ = new ProjectWidget(this);

    command_line_->initHistory();
    
  }


  void MainWindow::createActions() {

    act_open_project_ = new QAction(QIcon(Gui::resource_path() + "/project.png"), "&Open Project...", this);
    act_open_project_->setStatusTip(tr("Opening an existing project"));

    act_new_project_ = new QAction(QIcon(Gui::resource_path() + "/current_project.png"), "&New Project...", this);
    act_new_project_->setStatusTip(tr("Create a new project"));

    act_save_project_ = new QAction(QIcon(Gui::resource_path() + "/save.png"), "&Save Project", this);
    Gui::main_app()->addWaitObject(act_save_project_);
    act_save_project_->setStatusTip(tr("Save the active project"));
    act_save_project_->setEnabled(false);

    act_close_project_ = new QAction(QIcon(Gui::resource_path() + "/menu_exit.png"), "&Close Project", this);
    Gui::main_app()->addWaitObject(act_close_project_);
    act_close_project_->setStatusTip(tr("Close the active project"));
    act_close_project_->setEnabled(false);

    act_show_dock_start_ = new QAction("Dock Start", this);
    act_show_dock_start_->setCheckable(true);
    act_show_dock_start_->setChecked(true);
    act_show_dock_start_->setStatusTip(tr("Dock the start window"));

    connect(act_open_project_, SIGNAL(triggered()), this, SLOT(onOpenProject()));
    connect(act_new_project_, SIGNAL(triggered()), this, SLOT(onNewProject()));
    connect(act_save_project_, SIGNAL(triggered()), this, SLOT(onSaveProject()));
    connect(act_close_project_, SIGNAL(triggered()), this, SLOT(onCloseProject()));
    connect(act_show_dock_start_, SIGNAL(triggered(bool)), this, SLOT(onSetProjectDockVisible(bool)));


    act_window_device_layout_ = new QAction(QIcon(), "Device Layout...", this);
    act_window_device_layout_->setCheckable(true);
    act_window_device_layout_->setChecked(false);
    act_window_device_layout_->setStatusTip("Show device layout");

    act_window_main_tab_ = new QAction(QIcon(), "Project Tabs...", this);
    act_window_main_tab_->setCheckable(true);
    act_window_main_tab_->setChecked(true);
    act_window_main_tab_->setStatusTip("Show project tab");


    connect(act_window_device_layout_, SIGNAL(triggered(bool)), this, SLOT(onLayout(bool)));
    connect(act_window_main_tab_, SIGNAL(triggered(bool)), this, SLOT(onSetMdiWindowVisible(bool)));//FIXME change name to main tab

    act_show_dock_cmd_ = new QAction("Dock Console", this);
    act_show_dock_cmd_->setCheckable(true);
    act_show_dock_cmd_->setChecked(true);
    act_show_dock_cmd_->setStatusTip(tr("Dock the console"));

    connect(act_show_dock_cmd_, SIGNAL(triggered(bool)), this, SLOT(onSetConsoleDockVisible(bool)));

  }

  void MainWindow::createProjectTabWindow() {
    project_tab_ = new QTabWidget(this);
    project_tab_->setTabsClosable(true);
    project_tab_->setMovable(true);
    project_tab_->setTabPosition(QTabWidget::South);
    project_tabs_subwindow_ = new EDAMdiSubWindow(this);
    project_tabs_subwindow_->setWindowFlags(Qt::WindowType::CustomizeWindowHint | Qt::WindowType::WindowMinMaxButtonsHint);
    project_tabs_subwindow_->setWindowTitle("Project Tab");
    project_tabs_subwindow_->setWidget(project_tab_);
    connect(project_tabs_subwindow_, SIGNAL(visibilityChanged(bool)), act_window_main_tab_, SLOT(setChecked(bool)));
    mdi_central_->addSubWindow(project_tabs_subwindow_);

    project_tabs_subwindow_->showMaximized();

  }

  void MainWindow::createConsoleDock() {
    main_console_ = new MainConsole(this);
    connect(main_console_, SIGNAL(sigViewConsoleLog()), this, SLOT(onViewConsoleLogFile()));

    warning_list_ = new QListWidget(this);
    error_list_ = new QListWidget(this);
    Console::set_console(main_console_);

    connect(main_console_, SIGNAL(warningsCountChanged(int)), this, SLOT(onWarningChanged(int)));
    connect(main_console_, SIGNAL(errorsCountChanged(int)), this, SLOT(onErrorChanged(int)));

    dock_console_ = new QDockWidget("TCL Console", this);
    dock_console_->setAllowedAreas(Qt::DockWidgetArea::BottomDockWidgetArea);
    dock_console_->setWindowFlags(Qt::WindowType::WindowTitleHint);
    dock_console_->setContextMenuPolicy(Qt::NoContextMenu);
    dock_console_->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetClosable);
    dock_console_->setStyleSheet("QDockWidget {color:black; font-size:12px; border:2px solid gray; height:10px;}" + QString("QDockWidget::title {text-align:center}"));
    connect(dock_console_, SIGNAL(visibilityChanged(bool)), act_show_dock_cmd_, SLOT(setChecked(bool)));

    QWidget* console_widget = new QWidget(this);
    QLabel* command_label = new QLabel(prompt_prefix.c_str(), console_widget);
    command_line_ = new CommandLine(main_console_, this);
    Gui::main_app()->addWaitObject(command_line_);
    command_line_->setEnabled(true);

    QHBoxLayout* command_line_edit_layout = new QHBoxLayout();
    command_line_edit_layout->addWidget(command_label);
    command_line_edit_layout->addWidget(command_line_);
    QVBoxLayout* command_console_layout = new QVBoxLayout();
    command_console_layout->addWidget(main_console_);
    command_console_layout->addLayout(command_line_edit_layout);
    console_widget->setLayout(command_console_layout);

    console_tab_ = new QTabWidget(this);
    console_tab_->setTabPosition(QTabWidget::TabPosition::South);
    //keep the tab order "console error warning" for functions onErrorChanged and onWarningChanged 
    console_tab_->addTab(console_widget, QIcon(Gui::resource_path() + "/tab_terminal.png"), "Console");
    console_tab_->addTab(error_list_, QIcon(Gui::resource_path() + "/error.png"), "No Errors");
    console_tab_->addTab(warning_list_, QIcon(Gui::resource_path() + "/warning.png"), "No Warnings");
    dock_console_->setWidget(console_tab_);

    addDockWidget(Qt::BottomDockWidgetArea, dock_console_, Qt::Horizontal);
  }

  void MainWindow::onNewProject() {
    main_tab_->setDisabled(true);
    NewProjectWizard project_wizard(this);
    project_wizard.setObjectName("NEW_PROJECT_DLG");
    const int result_code = project_wizard.exec();
    if (!result_code) return;
    setCurrentProject(Project::project());
    main_tab_->setEnabled(true);
  }

  bool MainWindow::onOpenProject(const QString& filename) {
    if (!act_close_project_->isEnabled() && Project::project()) {
      return false;
    }

    QString project_file_name;
    if (filename == "") {
      project_file_name = Gui::getOpenFileName(this, "Open Project File", ".", "Project File (*" + Project::suffix() + ")");
    } else {
      project_file_name = filename;
    }

    if (project_file_name.isEmpty()) {
      return false;
    }

    if (Project::project() != NULL &&
      QMessageBox::No == Gui::messageBox(this, QMessageBox::Icon::Question, "EDA", "Close Current Project\nAre you sure?", 
                                         QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
      return false;
    }

    onCloseProject();

    Project::load(project_file_name);
    setCurrentProject(Project::project());
    main_tab_->setEnabled(true);
    return true;
  }
  void MainWindow::onCloseProject() {
    Project* current_project = Project::project();
    if (current_project == NULL) {
      return;
    }

    if (current_project->is_modified()) {
      QMessageBox save_box(this);
      save_box.setWindowTitle("Save Project");
      save_box.setText("Project is changed, do you want to save it?");
      QPushButton* save_project = save_box.addButton("&Save", QMessageBox::YesRole);
      save_box.addButton("&Discard", QMessageBox::NoRole);
      save_box.setDefaultButton(save_project);
      save_box.exec();
      if (save_box.clickedButton() == save_project) {
        onSaveProject();
      }
    }

    Gui::main_app()->setBusy(true);
    Project::release();
    Gui::main_app()->setBusy(false);

    QDir::setCurrent(init_path_);
    setCurrentProject(NULL);

    main_tab_->setDisabled(true);

    clearConsoleTab();

    command_line_->clearHistory();
    main_console_->run("clear");

    project_tabs_subwindow_->hide();
    if (layout_window_) {
      delete layout_window_;
      layout_window_ = NULL;
    }
  }
  void MainWindow::clearConsoleTab() {
    warning_list_->clear();
    error_list_->clear();
    onWarningChanged(0);
    onErrorChanged(0);
    main_console_->clear();
    console_tab_->setCurrentIndex(0);
  }

  void MainWindow::onLayout(bool flag) {
    if (Project::project() == NULL) {
      return;
    }

    if (layout_window_ == NULL) {
      layout_window_ = new LayoutWindow(this);
      project_tab_->addTab(layout_window_, QIcon(Gui::resource_path() + "sigma.png"), "Device Layout");
    }
    if (flag) {
      project_tab_->show();

      int index = project_tab_->indexOf(layout_window_);

      project_tab_->setCurrentIndex(index);
      project_tab_->currentWidget()->setFocus();
      project_tabs_subwindow_->showMaximized();
      mdi_central_->setActiveSubWindow(project_tabs_subwindow_);
    } else {
      project_tab_->hide();
    }
  }

  void MainWindow::onSaveProject() {
    Project* current_project = Project::project();
    if (current_project == NULL)
      return;

    current_project->save();

    current_project->set_is_modified(false);
  }

  void MainWindow::onWarningChanged(int count) {
    if (count == 0) {
      console_tab_->setTabText(2, "No warning");
    } else {
      console_tab_->setTabText(2, "Warnings (" + QString::number(count) + ")");
    }
    QStringList lists = main_console()->warnings();
    warning_list_->clear();
    foreach(QString msg, lists) {
      warning_list_->addItem(msg.trimmed());
    }
  }
  void MainWindow::onErrorChanged(int count) {
    if (count == 0) {
      console_tab_->setTabText(1, "No error");
    } else {
      console_tab_->setTabText(1, "Errors (" + QString::number(count) + ")");
    }
    QStringList lists = main_console()->errors();
    error_list_->clear();
    foreach(QString msg, lists) {
      error_list_->addItem(msg.trimmed());
    }
  }
  void MainWindow::onViewConsoleLogFile() {
    //FIXME
    return;
  }

  void MainWindow::onSetConsoleDockVisible(bool flag) {
    dock_console_->setVisible(flag);
    return;
  }
  void MainWindow::onSetProjectDockVisible(bool flag) {
    dock_start_->setVisible(flag);
    return;
  }
  void MainWindow::onStartReport() {
    CommandExecutor* executor = qobject_cast<CommandExecutor*>(sender());
    if (executor == NULL) return;
    //FIXME: report place, route, set_device ....
  }
}

