#pragma once
#include <QEvent>

class CurrentFPSEvent : public QEvent {
 public:
  CurrentFPSEvent(int fps);
  ~CurrentFPSEvent() = default;

  static const QEvent::Type type;
  int m_fps;
};
