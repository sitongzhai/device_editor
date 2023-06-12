//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#ifndef GUI_CONSOLE_COMMAND_EXECUTOR_H
#define GUI_CONSOLE_COMMAND_EXECUTOR_H

#include <qobject.h>
#include <qstring.h>
#include <qlabel.h>
#include <qtimer.h>

namespace eda {

  class CommandExecutor : public QLabel {
    Q_OBJECT
  public:
    enum class State {
      kRunning,            // running clock
      kInit,               // empty circle
      kFinishSuccessfully, // green
      kFinishWarning,      // yellow
      kFinishError,        // red
      kUserStopped
    };
  private:
    int index_;
    int sector_;
    State state_;
    QTimer timer_;
    QString command_;
    QString new_command_;

  public:
    CommandExecutor(QWidget* parent = NULL);
    ~CommandExecutor();

    bool isRunning() { return state_ == State::kRunning; }
    bool isInit() { return state_ == State::kInit; }
    bool isFinishSuccessfully() { return state_ == State::kFinishSuccessfully; }
    bool isFinishWarning() { return state_ == State::kFinishWarning; }
    bool isFinishError() { return state_ == State::kFinishError; }
    bool isUserStopped() { return state_ == State::kUserStopped; }
    bool isFinish() {
      return state_ == State::kFinishSuccessfully ||
        state_ == State::kFinishWarning ||
        state_ == State::kFinishError;
    }
    int index() { return index_; }
    void setIndex(int i) { index_ = i; }
    State state() const { return state_; }
    QString command() { return command_; }

  public:
    void setFinishError() {
      state_ = State::kFinishError;
    }
    void setRunning();
    void setClear();
    void setFinish(bool success);

    bool registerCommand(const QString& command);
    bool updateExecuterCmd(const QString& command);

  protected:
    virtual void paintEvent(QPaintEvent* /*event*/);
    virtual QString generateCommand();
    virtual void customEvent(QEvent* event);
    
  public slots:
    void run();
    void runCommand();
    void reRun();
    void stop();

  protected slots:
    void resetLabel();
    void onTimeout();
    void onFinish(int tcl_result);
    void onStarted();
    void onEnded();

  signals:
    void sigStart();
    void sigFinish(int);

  };

}

#endif // !GUI_CONSOLE_COMMAND_LABEL_H


