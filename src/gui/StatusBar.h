#pragma once
#include <QLabel>
#include <QStatusBar>

class StatusBar : public QStatusBar {
  Q_OBJECT
 public:
  StatusBar(QWidget* parent = nullptr);
  ~StatusBar() = default;

  bool event(QEvent *) override;

  QLabel* m_fps_label;
};
