//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <tcl.h>
#include <qrect.h>
#include <qpen.h>
#include <qpainter.h>
#include <qbrush.h>

#include "gui/gui.h"
#include "gui/main_event.h"
#include "gui/main_app.h"
#include "gui/main_window.h"
#include "gui/command_context.h"
#include "gui/console/command_executor.h"

#include "utility/log.h"

namespace eda {

  CommandExecutor::CommandExecutor(QWidget* parent) : QLabel(parent) {
    index_ = -1;
    sector_ = 0;
    state_ = State::kInit;
    setFixedSize(16, 16);
    timer_.setSingleShot(false);
    timer_.setInterval(77);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
    EventDispatcher::instance()->subscribeEvent(this, GlobalEvent::kEventOptionChanged);
    EventDispatcher::instance()->subscribeEvent(this, GlobalEvent::kEventOptionInit);
    connect(this, SIGNAL(destroyed(QObject*)), EventDispatcher::instance(), SLOT(cleanup(QObject*)));
    setAttribute(Qt::WA_TranslucentBackground);
    registerCommand(" ");
  }
  CommandExecutor::~CommandExecutor() {
    EventDispatcher::instance()->unsubscribeEvent(this, GlobalEvent::kEventOptionChanged);
    EventDispatcher::instance()->unsubscribeEvent(this, GlobalEvent::kEventOptionInit);
  }
  void CommandExecutor::setRunning() {
    timer_.start();
    state_ = State::kRunning;
    repaint();
  }
  void CommandExecutor::setClear() {
    if (state_ == State::kRunning)
      return;
    timer_.stop();
    sector_ = 0;
    state_ = State::kInit;
    repaint();
  }
  void CommandExecutor::setFinish(bool success) {
    timer_.stop();
    sector_ = 0;
    if (success) {
      state_ = State::kFinishSuccessfully;
    }
    repaint();
  }
  bool CommandExecutor::registerCommand(const QString& command) {
    new_command_ = command;
    if (isRunning()) {
      return false;
    }
    command_ = command;
    setEnabled(true);
    // remove all connections existed
    disconnect();
    connect(this, SIGNAL(sigStart()), this, SLOT(onStarted()));
    if (Gui::main_window())
      connect(this, SIGNAL(sigStart()), Gui::main_window(), SLOT(onStartReport()));
    // turn on animation here
    connect(this, SIGNAL(sigStart()), this, SLOT(run()));

    QStringList tips_list = command.split(";", QString::SkipEmptyParts);
    QString tip;
    foreach(QString s, tips_list)
      tip += " " + s + " \n";
    tip.remove(-1, 1);
    return true;
  }
  bool CommandExecutor::updateExecuterCmd(const QString& command) {
    command_ = command;
    QStringList tips_list = command.split(";", QString::SkipEmptyParts);
    QString tip;
    foreach(QString s, tips_list)
      tip += " " + s + " \n";
    tip.remove(-1, 1);
    return true;
  }
  void CommandExecutor::paintEvent(QPaintEvent* /*event*/) {
    QPainter painter(this);
    if (!isEnabled()) {
      if (state_ == State::kFinishSuccessfully) {
        painter.drawPixmap(rect(), QPixmap(Gui::resource_path() + "/success_disable.png"));
      }
      if (state_ != State::kInit) {
        return;
      }
    }
    QPen pen(Qt::green);
    QBrush brush(Qt::green, Qt::SolidPattern);
    painter.setPen(pen);
    QRect rectangle(1, 1, rect().width() - 2, rect().height() - 2);
    if (state_ == State::kRunning) {
      painter.drawEllipse(rectangle);
      painter.setBrush(brush);
      painter.drawPie(rectangle, sector_ * 16, 60 * 16);
    } else if (state_ == State::kFinishError) {
      painter.drawPixmap(rect(), QPixmap(Gui::resource_path() + "/error.png"));
    } else if (state_ == State::kFinishWarning) {
      painter.drawPixmap(rect(), QPixmap(Gui::resource_path() + "/warning.png"));
    } else if (state_ == State::kUserStopped) {
      painter.drawPixmap(rect(), QPixmap(Gui::resource_path() + "/stop.png"));
    } else if (state_ == State::kFinishSuccessfully) {
      painter.drawPixmap(rect(), QPixmap(Gui::resource_path() + "/success.png"));
    } else if (state_ == State::kInit) {
      pen.setColor(Qt::gray);
      brush.setStyle(Qt::NoBrush);
      brush.setColor(Qt::gray);
      painter.setPen(pen);
      painter.setBrush(brush);
      painter.drawEllipse(rectangle);
    }
  }
  QString CommandExecutor::generateCommand() {
    QString cmd;
    //Project* project = MainWindow::currentProject();

    //FIXME generate commands for project using Options, class Options should be redesigned

    return cmd;
  }
  void CommandExecutor::customEvent(QEvent* event) {
    GlobalEvent* global_event = dynamic_cast<GlobalEvent*>(event);
    if (!global_event) return;

    if (global_event->id() == GlobalEvent::kEventOptionChanged ||
      global_event->id() == GlobalEvent::kEventOptionInit) {
      QString event_command = generateCommand();
      registerCommand(event_command);
    }
  }
  void CommandExecutor::run() {
    std::vector<int> result_flags;
    QStringList cmd_list = command_.split(";", QString::SkipEmptyParts);
    foreach(QString s, cmd_list) {
      if (CommandContext::threadStopped()) {
        break;
      }
      int result = Gui::executeCmd(s, true);
      result_flags.push_back(result);
      if (result == TCL_ERROR) {
        break;
      }
    }
    // restore the stopped flag after executing all commands;
    if (CommandContext::threadStopped()) {
      CommandContext::setThreadStopped(false);
    }
    onEnded();
    if (std::find(result_flags.begin(), result_flags.end(), TCL_ERROR) != result_flags.end()) {
      onFinish(TCL_ERROR);
    } else {
      onFinish(TCL_OK);
    }
  }
  void CommandExecutor::runCommand() {
    if (command_.isEmpty()) {
      onFinish(TCL_ERROR);
      return;
    }
    emit sigStart();
  }
  void CommandExecutor::reRun() {
    if (command_.isEmpty()) {
      onFinish(TCL_ERROR);
      return;
    }
    if (state_ == State::kInit) {
      return;
    }
    EventDispatcher::instance()->broadcastEvent(GlobalEvent::kEventCommandFinish, this);
    emit sigStart();
  }
  void CommandExecutor::stop() {
    CommandContext::setThreadStopped(true);
    state_ = State::kUserStopped;
    sector_ = 0;
  }
  void CommandExecutor::resetLabel() {
    if (isRunning()) {
      return;
    }
    timer_.stop();
    sector_ = 0;
    state_ = State::kInit;
  }
  void CommandExecutor::onTimeout() {
    sector_ = sector_ + 60;
    if (sector_ >= 360) {
      sector_ = 0;
    }
    repaint();
  }
  void CommandExecutor::onFinish(int tcl_result) {
    if (state_ == State::kUserStopped) {
      eda_info("Operation canceled by user.\n");
      Console::done();
      tcl_result = TCL_ERROR;
    } else {
      if (tcl_result == TCL_ERROR) {
        state_ = State::kFinishError;
      } else if (tcl_result == TCL_OK) {
        state_ = State::kFinishSuccessfully;
      }
      if (new_command_ != command_) {
        registerCommand(new_command_);
      }
    }
    repaint();
    QMap<const char*, QVariant> attributes = { {GlobalEvent::key_result , tcl_result} };
    EventDispatcher::instance()->broadcastEvent(GlobalEvent::kEventLabelWorkFinish, this, attributes);
    emit sigFinish(tcl_result);
    return;
  }
  void CommandExecutor::onStarted() {
    Gui::main_app()->setBusy(true);
    setEnabled(true);
    Gui::main_app()->addExceptObject(this);
    timer_.start();
    state_ = State::kRunning;
    repaint();
  }
  void CommandExecutor::onEnded() {
    Gui::main_app()->setBusy(false);
    Gui::main_app()->removeExceptObject(this);
    timer_.stop();
  }

}