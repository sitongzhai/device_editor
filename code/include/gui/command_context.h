//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//* class for storing context information for the running command and notify
//* the current progress of the running command. The running command is supposed
//* to check the status of the thread it resides regularly using the function
//* threadStopped before doing the next step, start the progress indicator using
//* the function startProgressIndicator and update the progress using setProgress.
//******************************************************************************


#ifndef GUI_COMMAND_CONTEXT_H
#define GUI_COMMAND_CONTEXT_H

#include <qobject.h>
#include <qreadwritelock.h>

namespace eda {


  class CommandContext {
  private:
    static bool stopped_;
    static bool inited_;
    static int min_;
    static int max_;
    static int current_;
    static double factor_;
    static QObject* receiver_;
    static QReadWriteLock* lock_;
    static QReadWriteLock session_lock_;

  public:
    static void init(QObject*);
    static void reset();
    static bool threadStopped();
    static void setThreadStopped(bool value);
    static void startProgressIndicator(int min = 0, int max = 100);
    static int min();
    static int max();
    static int range();
    static void setProgress(int);
    static int currentProgress();
    static void setProgressFactor(double);
    static double progressFactor();
    static void increaseProgress(int);
    static bool inited();
    static void setReceiver(QObject* receiver);

  };

}

#endif // !GUI_COMMAND_CONTEXT_H
