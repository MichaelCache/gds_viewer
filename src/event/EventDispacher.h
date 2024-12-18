#pragma once

#include <QHash>
#include <QObject>
#include <QString>

enum class EventComp : int {
  LayoutCanvas,
  CellList,
};

/**
 * @brief use event instead of signal and slot
 *
 */
class EventDispacher {
 public:
  static EventDispacher& instance();

  void registComp(EventComp, QObject* comp);
  QObject* getComp(EventComp);

 private:
  EventDispacher() = default;
  ~EventDispacher() = default;

  QHash<EventComp, QObject*> m_registed_comp;
};
