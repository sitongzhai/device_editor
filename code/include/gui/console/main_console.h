//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_MAIN_CONSOLE_H
#define GUI_MAIN_CONSOLE_H

#include <qobject.h>
#include <qlineedit.h>
#include <qtextbrowser.h>

#include <qstringlist.h>
#include <qlist.h>
#include <qstring.h>

#include "gui/main_event.h"

namespace eda {

  class CommandExecutor;

  class MainConsole : public QTextBrowser {
    Q_OBJECT
  public:
    static QColor fontColor;
    static QColor warnColor;
    static QColor errColor;
    static QColor debugColor;

  private:
    CommandExecutor* executor_;
    QAction* copy_text_;
    QAction* view_log_;
    QAction* clear_;
    QStringList warning_list_;
    QStringList error_list_;

  public:
    MainConsole(QWidget* parent = NULL);
    virtual ~MainConsole();

  public:
    QStringList warnings() { return warning_list_; }
    QStringList errors() { return error_list_; }

    void appendOutput(const QString& message, const int output_type = CONSOLE_INFO);
    void setCursorEnd();
    void run(const QString& cmd);
  protected:
    virtual void customEvent(QEvent* event);

  signals:
    void warningsCountChanged(int);
    void errorsCountChanged(int);

    void sigCommandStart(const QString& cmd);
    void sigCommandFinish(const QString& cmd);
    void sigViewConsoleLog();

  protected slots:
    void onCopy();
    void onClear();

  };

}

#endif // !GUI_MAIN_CONSOLE_H
