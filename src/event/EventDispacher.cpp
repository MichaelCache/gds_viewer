#include "EventDispacher.h"

EventDispacher& EventDispacher::instance() {
  static EventDispacher dispscher;
  return dispscher;
}

void EventDispacher::registComp(EventComp name, QObject* comp) {
  Q_ASSERT(!m_registed_comp.contains(name));
  m_registed_comp.insert(name, comp);
}

QObject* EventDispacher::getComp(EventComp name) {
  return m_registed_comp.value(name);
}