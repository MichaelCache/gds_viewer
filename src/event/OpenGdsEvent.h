#pragma once

#include <QEvent>
#include <QString>

#include "EventDispacher.h"


class OpenGdsEvent : public QEvent {
 public:
  OpenGdsEvent(const QString& fn)
      : QEvent(QEvent::Type(OpenGdsEvent::type)), m_openfn(fn) {}
  ~OpenGdsEvent() {}

  QString data() { return m_openfn; }

  static const QEvent::Type type;

 private:
  QString m_openfn;
};
