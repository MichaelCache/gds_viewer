#pragma once

#include <gdstk.h>

#include <QObject>

class Kernel : public QObject {
  Q_OBJECT
 protected:
  bool event(QEvent* e) override;

 public:
  Kernel(QObject* parent = nullptr);
  ~Kernel();
  gdstk::Library m_gds_lib;
};
