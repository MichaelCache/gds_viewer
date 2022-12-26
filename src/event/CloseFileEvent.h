#pragma once
#include <QEvent>

#include "EventDispacher.h"

class CloseFileEvent : public QEvent {
 public:
  CloseFileEvent() : QEvent(QEvent::Type(CloseFileEvent::type)) {}
  ~CloseFileEvent() {}

  static const QEvent::Type type;
};
