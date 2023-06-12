//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************
#ifndef GUI_CONSOLE_COMMAND_LINE
#define GUI_CONSOLE_COMMAND_LINE

#include <qobject.h>
#include <qtextbrowser.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qstring.h>
#include <qevent.h>

namespace eda {

  class MainConsole;

  class CommandLine : public QLineEdit {
    Q_OBJECT

  private:
    MainConsole* main_console_;
    QList<QString> command_list_;
    QList<QString>::iterator current_iter_;

  public:
    CommandLine(MainConsole* main_console, QWidget* parent);
    ~CommandLine();

    void initHistory();
    void clearHistory();
    void append(QString command);
    QString lastCommand();
    QString currentCommand();
    QString nextCommand();

  protected:
    void keyPressEvent(QKeyEvent* event);
    void keyUp();
    void keyDown();
    void keyEnter();

  signals:
    void sigTclCommand(const QString& cmd);
  };

}

#endif // !GUI_CONSOLE_COMMAND_LINE
