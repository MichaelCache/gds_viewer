#pragma once

#include <QEvent>
#include <QString>
#include <QVector>

class SetCellNamesEvent : public QEvent {
 public:
  SetCellNamesEvent(const QVector<QString>& cell_names);
  ~SetCellNamesEvent() {}

  const QVector<QString> cellNames();

  static const QEvent::Type type;

 private:
  QVector<QString> m_cell_names;
};
