//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include "gui/main_app.h"
#include "gui/main_event.h"

namespace eda {

  MainApp::MainApp(int& argc, char** argv) : QApplication(argc, argv), data_lock_(QReadWriteLock::NonRecursive) {
    setObjectName("top");
  }
  void MainApp::addExceptObject(QObject* o) {
    except_objects_.insert(o);
  }
  void MainApp::removeExceptObject(QObject* o) {
    except_objects_.remove(o);
  }
  void MainApp::addWaitObject(QObject* o) {
    wait_objects_.insert(o);
  }
  void MainApp::removeWaitObject(QObject* o) {
    wait_objects_.remove(o);
  }
  void MainApp::addWaitTreeItem(QTreeWidgetItem* o) {
    wait_tree_items_.insert(o);
  }
  void MainApp::removeWaitTreeItem(QTreeWidgetItem* o) {
    wait_tree_items_.remove(o);
  }
  void MainApp::setBusy(bool busy, bool for_read) {
    if (busy) {
      lock(for_read);
      setOverrideCursor(Qt::BusyCursor);
      EventDispatcher::instance()->broadcastEvent(GlobalEvent::kEventBusyLocked);
    }
    setObjectsEnabled(!busy);
    if (!busy) {
      restoreOverrideCursor();
      EventDispatcher::instance()->broadcastEvent(GlobalEvent::kEventCommandFinish);
      unlock();
    }
  }
  bool MainApp::isBusyWriting() {
    if (data_lock_.tryLockForRead()) {
      data_lock_.unlock();
      return false;
    } else {
      return true;
    }
  }
  bool MainApp::isBusyReading() {
    if (data_lock_.tryLockForWrite()) {
      data_lock_.unlock();
      return false;
    } else {
      return true;
    }
  }
  QString MainApp::productName() {
    //EDA(TM)
    return QString(QString("EDA")/* + QChar(0x2122)*/);
  }
  void MainApp::lock(bool for_read) {
    if (for_read) {
      data_lock_.lockForRead();
    } else {
      data_lock_.lockForWrite();
    }
  }
  void MainApp::unlock() {
    data_lock_.unlock();
  }
  void MainApp::setObjectsEnabled(bool flag) {
    for (auto iter = wait_objects_.begin(); iter != wait_objects_.end(); ++iter) {
      if ((*iter)->inherits("QAction")) {
        QAction* a = static_cast<QAction*>(*iter);
        a->setEnabled(flag);
      } else if ((*iter)->inherits("QWidget")) {
        QWidget* a = static_cast<QWidget*>(*iter);
        a->setEnabled(flag);
        a->update();
      }
    }
    for (auto iter = wait_tree_items_.begin(); iter != wait_tree_items_.end(); iter++) {
      (*iter)->setDisabled(!flag);
    }
  }

}