#pragma once
#include <QEvent>

class CloseFileEvent : public QEvent {
 public:
  CloseFileEvent();
  ~CloseFileEvent() = default;

  static const QEvent::Type type;
};
