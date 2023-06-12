//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <qevent.h>

#include "gui/project/mdi_subwindow.h"

namespace eda {

  void EDAMdiSubWindow::closeEvent(QCloseEvent* event) {
    event->ignore();
    showNormal();
    setVisible(false);
    emit visibilityChanged(false);
  }
  void EDAMdiSubWindow::showEvent(QShowEvent* event) {
    emit visibilityChanged(true);
  }
  void EDAMdiSubWindow::hideEvent(QHideEvent* event) {
    emit visibilityChanged(false);
  }

}
