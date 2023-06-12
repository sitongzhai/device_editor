//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_EDA_MDI_SUB_WINDOW_H
#define GUI_EDA_MDI_SUB_WINDOW_H

#include <qtimer.h>
#include <qobject.h>
#include <qlabel.h>
#include <qevent.h>
#include <qprocess.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <qtablewidget.h>
#include <qtreewidget.h>
#include <qdockwidget.h>
#include <qmdisubwindow.h>
#include <qreadwritelock.h>
#include <qthread.h>
#include <qlistwidget.h>
#include <qstyleoption.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qtextedit.h>
#include <qwizard.h>
#include <qfiledialog.h>
#include <qicon.h>
#include <qmessagebox.h>

#include "gui/project/project.h"
#include "utility/data_var.h"

namespace eda {

  class EDAComboBox : public QComboBox {
  public:
    EDAComboBox(QWidget* parent = NULL);
    ~EDAComboBox() {}
    void addItem(const QString& text, const QVariant& user_data = QVariant());
    void setCurrentText(const QString& text);
  };
  class EDATableWidget : public QTableWidget {
    Q_OBJECT
  public:
    EDATableWidget(QWidget* parent = NULL);
    ~EDATableWidget();
  };

}


#endif // !GUI_EDA_MDI_SUB_WINDOW+H
