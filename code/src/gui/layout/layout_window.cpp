
#include <qstatusbar.h>
#include <qlabel.h>
#include <qtoolbar.h>
#include <qlayout.h>
#include <qsplitter.h>
//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//* This is only a demo used for how to design a QT window used in the Multiple
//* Document Interface of this desktop software framework.
//******************************************************************************

#include "gui/layout/layout_window.h"

QLabel* g_status_label = NULL;

namespace eda {


  LayoutWindow::LayoutWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Device Browser");
    initializeSideBar();
    QLabel* status_label = new QLabel("Status Bar");
    QStatusBar* status_bar = new QStatusBar();
    status_bar->addWidget(status_label);
    setStatusBar(status_bar);
    g_status_label = status_label;
    resize(1200, 800);
  }

  void LayoutWindow::initializeSideBar() {

    QToolBar* toolBar = new QToolBar();
    toolBar->setAllowedAreas(Qt::LeftToolBarArea);
    QAction* action = toolBar->addAction("<<");
    connect(action, SIGNAL(triggered()), this, SLOT(changeDockVisible()));
    addToolBar(Qt::LeftToolBarArea, toolBar);
  }




}