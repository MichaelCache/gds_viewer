#pragma once

#include <QMainWindow>
#include <QStatusBar>

#include "CentralWidget.h"
#include "MenuBar.h"
#include "kernel/Kernel.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 private:
  CentralWidget *m_main_widget;
  MenuBar *m_menubar;
  QStatusBar *m_logger;
  Kernel* m_kernel;
};