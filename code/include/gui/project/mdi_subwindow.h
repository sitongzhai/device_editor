//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_PROJECT_MDI_SUBWINDOW_H
#define GUI_PROJECT_MDI_SUBWINDOW_H

#include <qobject.h>
#include <qmdisubwindow.h>

namespace eda {

  class EDAMdiSubWindow : public QMdiSubWindow {
    Q_OBJECT
  public:
    EDAMdiSubWindow(QWidget* parent) : QMdiSubWindow(parent) {}
    ~EDAMdiSubWindow() {}
  signals:
    void visibilityChanged(bool);
  protected:
    void closeEvent(QCloseEvent* event);
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);
  };

}

#endif // !GUI_PROJECT_MDI_SUBWINDOW_H
