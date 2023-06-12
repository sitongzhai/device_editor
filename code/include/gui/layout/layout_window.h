
//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//* This is only a demo used for how to design a QT window used in the Multiple
//* Document Interface of this desktop software framework.
//******************************************************************************

#ifndef GUI_LAYOUT_LAYOUT_WINDOW_H
#define GUI_LAYOUT_LAYOUT_WINDOW_H

#include <qobject.h>
#include <qlabel.h>
#include <qtreewidget.h>
#include <qgraphicsscene.h>
#include <qmainwindow.h>

namespace eda {

  class LayoutWindow : public QMainWindow {
  Q_OBJECT
  public:
    LayoutWindow(QWidget* parent = NULL);

  protected:
    void initializeSideBar();

  private:
    QLabel* statusLabel_;
  };

}

#endif // !GUI_LAYOUT_LAYOUT_WINDOW_H
