//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <tcl.h>
#include <qcompleter.h>
#include <qfile.h>
#include <qchar.h>
#include <qstringlist.h>

#include "gui/gui.h"
#include "gui/console/command_line.h"
#include "gui/console/main_console.h"

#include "utility/log.h"

namespace eda {

  CommandLine::CommandLine(MainConsole* main_console, QWidget* parent) : QLineEdit(parent) {
    main_console_ = main_console;
    QStringList all_commands;
    Tcl_Interp* interp = Gui::interp();
    int res = Tcl_Eval(interp, qPrintable(QString("info commands")));
    if (!res) {
      //Get the string result of the executed command
      QString result = Tcl_GetString(Tcl_GetObjResult(interp));
      if (!result.isEmpty()) {
        all_commands = result.split(" ");
      }
    }
    QCompleter* completer = new QCompleter(all_commands, this);
    this->setCompleter(completer);
  }
  CommandLine::~CommandLine() {
  
  }
  void CommandLine::initHistory() {
    QFile file(default_history_file.c_str());
    if (!file.open(QFile::ReadOnly))
      return;
    QString content = file.readAll();
    QStringList lines = content.split("\n", QString::SkipEmptyParts);
    foreach(QString line, lines) {
      line = line.trimmed();
      command_list_.append(line);
    }
    current_iter_ = command_list_.end();
  }
  void CommandLine::clearHistory() {
    command_list_.clear();
    current_iter_ = command_list_.end();
  }
  void CommandLine::append(QString command) {
    command_list_.append(command);
    current_iter_ = command_list_.end();
  }
  QString CommandLine::lastCommand() {
    if (command_list_.empty()) return QString();
    if (current_iter_ != command_list_.begin()) {
      --current_iter_;
    }
    return (*current_iter_);
  }
  QString CommandLine::currentCommand() {
    if (command_list_.empty()) return QString();
    if (current_iter_ == command_list_.end()) return QString();
    return (*current_iter_);
  }
  QString CommandLine::nextCommand() {
    if (command_list_.empty()) return QString();
    if (current_iter_ == command_list_.end()) return QString();
    ++current_iter_;
    if (current_iter_ == command_list_.end()) return QString();
    return (*current_iter_);
  }
  void CommandLine::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
      case Qt::Key_Up:
        keyUp();
        break;
      case Qt::Key_Down:
        keyDown();
        break;
      case Qt::Key_Enter:
      case Qt::Key_Return:
        keyEnter();
        break;
      case Qt::Key_Tab:
      default:
        QLineEdit::keyPressEvent(event);
    }
  }
  void CommandLine::keyUp() {
    QString last = lastCommand();
    setText(last);
  }
  void CommandLine::keyDown() {
    QString next = nextCommand();
    setText(next);
  }
  void CommandLine::keyEnter() {
    QString command = this->text();
    command = command.trimmed();
    command.replace('\\', '/');
    if (!command.isEmpty()) {
      QFile history_file(default_history_file.c_str());
      if (history_file.open(QFile::WriteOnly | QFile::Append)) {
        history_file.write((command + "\n").toLatin1().data());
        history_file.close();
      }
    }
    QStringList commands = command.split(QChar::ParagraphSeparator);
    if (command != "") {
      foreach(QString cmd, commands) {
        if (cmd != "") {
          this->append(cmd);
          emit sigTclCommand(cmd);
        }
      }
      command.replace(QChar::ParagraphSeparator, ';');
      main_console_->run(command);
    } else {
      setText("");
    }
  }

}