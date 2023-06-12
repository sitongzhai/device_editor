//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_MAIN_APP_H
#define GUI_MAIN_APP_H

#include <qset.h>
#include <qaction.h>
#include <qapplication.h>
#include <qtreewidget.h>
#include <qreadwritelock.h>

namespace eda {
  class MainApp : public QApplication {
  private:
    QSet<QObject*> wait_objects_;
    QSet<QObject*> except_objects_;
    QSet<QTreeWidgetItem*> wait_tree_items_;
    QReadWriteLock data_lock_;
  public:
    MainApp(int& argc, char** argv);

    void addExceptObject(QObject* o);
    void removeExceptObject(QObject* o);
    void addWaitObject(QObject* o);
    void removeWaitObject(QObject* o);
    void addWaitTreeItem(QTreeWidgetItem* o);
    void removeWaitTreeItem(QTreeWidgetItem* o);
    void setBusy(bool busy, bool for_read = false);
    bool isBusyWriting();
    bool isBusyReading();
    static QString productName();

  protected:
    void lock(bool for_read);
    void unlock();
    void setObjectsEnabled(bool flag);

  };

}

#endif // !GUI_MAIN_APP_H
