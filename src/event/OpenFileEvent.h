#pragma once

#include <QEvent>
#include <QString>

class OpenFileEvent : public QEvent {
 public:
  OpenFileEvent(const QString& fn);
  ~OpenFileEvent() = default;

  const QString& fileName();

  static const QEvent::Type type;

 private:
  QString m_openfn;
};
