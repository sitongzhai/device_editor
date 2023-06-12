//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#include <qicon.h>
#include <qaction.h>
#include <qmdiarea.h>
#include <qpainter.h>
#include <qwidget.h>
#include <qcompleter.h>
#include <qlineedit.h>
#include <qstringlistmodel.h>
#include <qboxlayout.h>
#include <qmessagebox.h>
#include <qdialogbuttonbox.h>
#include <qdesktopservices.h>
#include <qclipboard.h>
#include <qheaderview.h>

#include "gui/gui.h"
#include "gui/gui_utility.h"
#include "gui/main_app.h"
#include "gui/main_event.h"
#include "gui/main_window.h"
#include "utility/log.h"

namespace eda {

  EDATableWidget::EDATableWidget(QWidget* parent) : QTableWidget(parent) {
    setSelectionBehavior(QTableWidget::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
  }
  EDATableWidget::~EDATableWidget() {

  }




  EDAComboBox::EDAComboBox(QWidget* parent) : QComboBox(parent) {
  }
  void EDAComboBox::addItem(const QString& text, const QVariant& user_data) {
    QComboBox::addItem(text, user_data);
    setEnabled(count() > 1); // only enable if more than one item
  }
  void EDAComboBox::setCurrentText(const QString& text) {
    int index = findText(text);
    if (index != -1) {
      setCurrentIndex(index);
    }
  }

}