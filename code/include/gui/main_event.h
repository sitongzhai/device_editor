//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#ifndef GUI_MAIN_EVENT_H
#define GUI_MAIN_EVENT_H

#include <qstring.h>
#include <qevent.h>
#include <qobject.h>

namespace eda {

  //event for console
#define CONSOLE_EVENT (QEvent::User + 1)
#define CONSOLE_END 0
#define CONSOLE_INFO 1
#define CONSOLE_WARN 2
#define CONSOLE_ERROR 3
#define CONSOLE_DEBUG 4

  const int PROGRESS_STARTED = QEvent::Type(QEvent::MaxUser - 1);
  const int PROGRESS_UPDATED = QEvent::Type(QEvent::MaxUser - 2);
  const int THREAD_STOPPED = QEvent::Type(QEvent::MaxUser - 3);
  const QEvent::Type CHIP_BUNDLE_READY = QEvent::Type(QEvent::MaxUser - 4);
  const QEvent::Type STAGE_CHANGED = QEvent::Type(QEvent::MaxUser - 5);
  const QEvent::Type NO_LICENSE = QEvent::Type(QEvent::MaxUser - 6);//FIXME
  const QEvent::Type CMD_EXECUTED = QEvent::Type(QEvent::MaxUser - 7);
  const QEvent::Type CMD_FINISHED = QEvent::Type(QEvent::MaxUser - 8);
  const QEvent::Type LOADING_COMPLETED = QEvent::Type(QEvent::MaxUser - 9);

  static const int global_event_type = QEvent::registerEventType();

  class GlobalEvent : public QEvent {
  public:
    enum EventId {
      kEventNone = 0,
      kEventOptionInit,
      kEventOptionChanged,
      kEventProjectOpened,
      kEventCommandFinish, 
      kEventChipSelected,
      kEventBusyLocked,
      kEventLabelWorkFinish
    };
  public:
    static const char* key_result;
  private:
    EventId id_;
    void* sender_;
    QMap<const char*, QVariant> attributes_;
  public:
    GlobalEvent(EventId id, void* sender, const QMap<const char*, QVariant>& attrs) : QEvent(static_cast<QEvent::Type>(global_event_type)) {
      id_ = id;
      sender_ = sender;
      attributes_ = attrs;
    }
  public:
    EventId id() { return id_; }
    void* sender() { return sender_; }
    QMap<const char*, QVariant>& attributes() { return attributes_; }
    void set_attributes(QMap<const char*, QVariant>& attributes) { attributes_ = attributes; }
  };

  class EventDispatcher : public QObject {
    Q_OBJECT
  private:
    static EventDispatcher* instance_;
    std::map<GlobalEvent::EventId, std::list<QObject*>> event_object_map_;
  public:
    EventDispatcher();
    ~EventDispatcher();
  public:
    static EventDispatcher* instance();
    void broadcastEvent(GlobalEvent::EventId event_id, QObject* from_obj = NULL, const QMap<const char*, QVariant>& attributes = QMap<const char*, QVariant>());
    void subscribeEvent(QObject* obj, GlobalEvent::EventId event_id);
    void unsubscribeEvent(QObject* obj, GlobalEvent::EventId event_id);
  public slots:
    //Note: The subscriber's QObject::destroyed signal should be connected to the cleanup slot
    void cleanup(QObject* obj);
  };


  class ConsoleEvent : public QEvent {
  private:
    int msg_type_;
    QString msg_;

  public:
    ConsoleEvent(int msg_ype, QString msg);

    int msg_type();
    QString msg();

  };

  class ProgressChangedEvent : public QEvent {
  private:
    int value_;
  public:
    ProgressChangedEvent(int value, QEvent::Type type) : QEvent(type), value_(value) {}
    int value() const { return value_; }
  };
  class ProgressEvent : public QEvent {
  private:
    int min_;
    int max_;
  public:
    ProgressEvent(int min, int max, QEvent::Type type) :QEvent(type) {
      min_ = min;
      max_ = max;
    }
    int min() const { return min_; }
    int max() const { return max_; }
  };

  class ExecutedCommandEvent : public QEvent {
  public:
    ExecutedCommandEvent(const QString& cmd, QEvent::Type type = CMD_EXECUTED) :
      QEvent(type), cmd_(cmd) {}
    QString command() const { return cmd_; }

  private:
    QString cmd_;
  };

}
#endif // !GUI_MAIN_EVENT_H
