//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//******************************************************************************

#include <qapplication.h>

#include "gui/main_event.h"

namespace eda {
  EventDispatcher* EventDispatcher::instance_ = NULL;
  const char* GlobalEvent::key_result = "result";

  EventDispatcher::EventDispatcher() {

  }
  EventDispatcher::~EventDispatcher() {

  }
  EventDispatcher* EventDispatcher::instance() {
    if (instance_ == NULL) {
      instance_ = new EventDispatcher();
    }
    return instance_;
  }
  void EventDispatcher::broadcastEvent(GlobalEvent::EventId event_id, QObject* from_obj, const QMap<const char*, QVariant>& attributes) {
    auto iter = event_object_map_.find(event_id);
    if (iter != event_object_map_.end()) {
      auto obj_list = iter->second;
      for (auto list_iter = obj_list.begin(); list_iter != obj_list.end(); list_iter++) {
        if (*list_iter && (static_cast<void*>(*list_iter) != from_obj)) {
          QApplication::postEvent(*list_iter, new GlobalEvent(event_id, from_obj, attributes));
        }
      }
    }
  }
  void EventDispatcher::subscribeEvent(QObject* obj, GlobalEvent::EventId event_id) {
    event_object_map_[event_id].push_back(obj);
  }
  void EventDispatcher::unsubscribeEvent(QObject* obj, GlobalEvent::EventId event_id) {
    auto iter = event_object_map_.find(event_id);
    if (iter != event_object_map_.end()) {
      iter->second.remove(obj);
    }
  }
  void EventDispatcher::cleanup(QObject* obj) {
    for (auto iter = event_object_map_.begin(); iter != event_object_map_.end(); iter++) {
      iter->second.remove(obj);
    }
  }

  ConsoleEvent::ConsoleEvent(int msg_type, QString msg) : QEvent((QEvent::Type)CONSOLE_EVENT) {
    msg_type_ = msg_type;
    msg_ = msg;
  }

  int ConsoleEvent::msg_type() { 
    return msg_type_; 
  }

  QString ConsoleEvent::msg() {
    return msg_; 
  }

}