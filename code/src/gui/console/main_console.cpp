//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <qfile.h>
#include <qcompleter.h>
#include <qevent.h>
#include <qfont.h>
#include <qcolor.h>
#include <qaction.h>
#include <qicon.h>
#include <qscrollbar.h>
#include <qfontdialog.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qframe.h>

#include <tcl.h>
#include "gui/gui.h"
#include "gui/gui_utility.h"
#include "gui/command_context.h"
#include "gui/console/console.h"
#include "gui/console/main_console.h"
#include "gui/console/command_executor.h"
#include "utility/log.h"


namespace eda {

  QColor MainConsole::fontColor = QColor(Qt::black);
  QColor MainConsole::warnColor = QColor(10, 200, 150);
  QColor MainConsole::errColor = QColor(255, 0, 0);
  QColor MainConsole::debugColor = QColor(255, 0, 255);

  MainConsole::MainConsole(QWidget* parent) : QTextBrowser(parent) {
    setFrameStyle(QFrame::NoFrame | QFrame::Plain);
    setOpenLinks(false);
    setMinimumHeight(5);
    setContextMenuPolicy(Qt::ActionsContextMenu);
    setReadOnly(true);
    copy_text_ = new QAction(QIcon(Gui::resource_path() + "/save.png"), "Copy", this);
    view_log_ = new QAction(QIcon(Gui::resource_path() + "/files.png"), "View log file", this);
    clear_ = new QAction("clear", this);
    addAction(copy_text_);
    addAction(view_log_);
    addAction(clear_);
    connect(copy_text_, SIGNAL(triggered()), this, SLOT(onCopy()));
    connect(view_log_, SIGNAL(triggered()), this, SIGNAL(sigViewConsoleLog()));
    connect(clear_, SIGNAL(triggered()), this, SLOT(onClear()));
    executor_ = new CommandExecutor(this);
    executor_->setObjectName("EXECUTOR_COMMANDLABEL");
    executor_->setVisible(false);
  }
  MainConsole::~MainConsole() {
    if (executor_) delete executor_;
  }
  void MainConsole::appendOutput(const QString& message, const int output_type) {
    setCursorEnd();
    switch (output_type) {
      case CONSOLE_WARN:
        setTextColor(warnColor);
        warning_list_.append(message);
        emit warningsCountChanged(warning_list_.count());
        break;
      case CONSOLE_ERROR:
        setTextColor(errColor);
        error_list_.append(message);
        emit errorsCountChanged(error_list_.count());
        break;
      case CONSOLE_DEBUG:
        setTextColor(debugColor);
        break;
      default:
        setTextColor(fontColor);
        break;
    }
    textCursor().insertText(message);
    verticalScrollBar()->setValue(verticalScrollBar()->maximumHeight());
  }
  void MainConsole::setCursorEnd() {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
    setTextCursor(cursor);
  }
  void MainConsole::run(const QString& cmd) {
    if (cmd == "clear") {
      onClear();
      return;
    }
    executor_->registerCommand(cmd);
    CommandContext::init(NULL);
    executor_->runCommand();
  }
  void MainConsole::customEvent(QEvent* event) {
    if (event->type() == CONSOLE_EVENT) {
      ConsoleEvent* e = (ConsoleEvent*)event;
      QString msg = e->msg();
      if (e->msg_type() == CONSOLE_END) {
        //do nothing
      } else {
        appendOutput(msg, e->msg_type());
      }
    }
  }
  void MainConsole::onCopy() {
    if (textCursor().hasSelection()) {
      QClipboard* clipboard = QApplication::clipboard();
      clipboard->setText(textCursor().selectedText());
    }
  }
  void MainConsole::onClear() {
    clear();
    warning_list_.clear();
    error_list_.clear();
    if (executor_) {
      executor_->setClear();
    }
    emit warningsCountChanged(0);
    emit errorsCountChanged(0);
    Console::done();
  }
  
  
}