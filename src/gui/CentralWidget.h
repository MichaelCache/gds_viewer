#pragma once

#include <QTextEdit>
#include <QWidget>

#include "TopCellList.h"
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
  TopCellList *m_top_cell_list;
  // CircuitScene *m_circuit_scence;
  QTextEdit *m_logger;
};