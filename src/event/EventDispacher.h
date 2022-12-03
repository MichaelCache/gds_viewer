#pragma once

#include <QHash>
#include <QObject>
#include <QString>

class EventDispacher {
 public:
  static EventDispacher& instance();

  void registComp(const QString& name, QObject* comp);
  QObject* getComp(const QString& name);

 private:
  EventDispacher();
  ~EventDispacher();

  QHash<QString, QObject*> m_registed_comp;
};
