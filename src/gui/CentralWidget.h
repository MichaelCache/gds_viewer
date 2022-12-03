#pragma once

#include <QTextEdit>
#include <QWidget>

#include "layout_canvas/LayoutCanvas.h"

class CentralWidget : public QWidget {
  Q_OBJECT

 public:
  explicit CentralWidget(QWidget *parent = nullptr);
  ~CentralWidget();

 private:
  void createComponent();
  void initLayout();

  LayoutCanvas *m_layout_canvas;
  // CircuitScene *m_circuit_scence;
  QTextEdit *m_logger;
};