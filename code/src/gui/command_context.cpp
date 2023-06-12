//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#include <qevent.h>

#include "gui/gui.h"
#include "gui/command_context.h"
#include "gui/main_app.h"
#include "gui/main_event.h"

namespace eda {

  bool CommandContext::stopped_ = false;
  QReadWriteLock* CommandContext::lock_ = NULL;
  bool CommandContext::inited_ = false;
  int CommandContext::min_ = -1;
  int CommandContext::max_ = -1;
  int CommandContext::current_ = -1;
  double CommandContext::factor_ = 1.;
  QObject* CommandContext::receiver_ = NULL;
  QReadWriteLock CommandContext::session_lock_;

  void CommandContext::init(QObject* receiver) {
    session_lock_.lockForWrite();
    receiver_ = receiver;
    lock_ = new QReadWriteLock();
    inited_ = true;
    min_ = max_ = current_ = 0;
    factor_ = 1.0;
    setThreadStopped(false);
  }
  void CommandContext::reset() {
    receiver_ = NULL;
    delete lock_;
    lock_ = NULL;
    inited_ = false;
    min_ = max_ = current_ = -1;
    factor_ = 1.;
    session_lock_.unlock();
  }
  bool CommandContext::threadStopped() {
    if (lock_) {
      lock_->lockForRead();
    }
    bool value = stopped_;
    if (lock_) {
      lock_->unlock();
    }
    return value;
  }
  int CommandContext::min() {
    if (!inited_ && Gui::main_app()) {
      return 0;
    }
    return min_;
  }
  int CommandContext::max() {
    if (!inited_ && Gui::main_app()) {
      return 0;
    }
    return max_;
  }
  int CommandContext::range() {
    if (!inited_ && Gui::main_app()) {
      return 0;
    }
    return max_ - min_;
  }
  int CommandContext::currentProgress() {
    if (!inited_ && Gui::main_app()) {
      return 0;
    }
    return current_;
  }
  void CommandContext::setThreadStopped(bool value) {
    if (!lock_) {
      stopped_ = value;
      return;
    }
    lock_->lockForWrite();
    stopped_ = value;
    lock_->unlock();
    if (value) {
      QEvent* event = new QEvent((QEvent::Type)THREAD_STOPPED);
      Gui::main_app()->postEvent(receiver_, event);
    }
  }
  void CommandContext::startProgressIndicator(int min, int max) {
    if (!receiver_) {
      return;
    }
    min_ = min;
    max_ = max;
    QEvent* event = new ProgressEvent(min, max, QEvent::Type(PROGRESS_STARTED));
    Gui::main_app()->postEvent(receiver_, event);
    setProgress(min_);
  }
  void CommandContext::setProgress(int value) {
    if (!receiver_) {
      return;
    }
    if (threadStopped()) {
      return;
    }
    current_ = value;
    QEvent* event = new ProgressChangedEvent(value, QEvent::Type(PROGRESS_UPDATED));
    Gui::main_app()->postEvent(receiver_, event);
  }
  void CommandContext::setProgressFactor(double value) {
    factor_ = value;
  }
  double CommandContext::progressFactor() {
    return factor_;
  }
  void CommandContext::increaseProgress(int delta) {
    setProgress(currentProgress() + delta);
  }
  bool CommandContext::inited() { 
    return inited_; 
  }
  void CommandContext::setReceiver(QObject* receiver) { 
    receiver_ = receiver; 
  }

}