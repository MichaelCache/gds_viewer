#pragma once

#include <QEvent>
#include <QString>

#include "EventDispacher.h"


class OpenFileEvent : public QEvent {
 public:
  OpenFileEvent(const QString& fn)
      : QEvent(QEvent::Type(OpenFileEvent::type)), m_openfn(fn) {}
  ~OpenFileEvent() {}

  QString data() { return m_openfn; }

  static const QEvent::Type type;

 private:
  QString m_openfn;
};
