#include "SetCellNamesEvent.h"

const QEvent::Type SetCellNamesEvent::type =
    (QEvent::Type)QEvent::registerEventType();

SetCellNamesEvent::SetCellNamesEvent(const QVector<QString>& cell_names)
    : QEvent(type), m_cell_names(cell_names) {}

const QVector<QString> SetCellNamesEvent::cellNames() {
  return m_cell_names;
  ;
}