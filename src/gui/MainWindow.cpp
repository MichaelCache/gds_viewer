#include "MainWindow.h"

#include <QStyleFactory>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setStyle(QStyleFactory::create("Fusion"));

  m_main_widget = new CentralWidget();
  setCentralWidget(m_main_widget);

  m_menubar = new MenuBar();
  setMenuBar(m_menubar);

  m_kernel = new Kernel(this);

  // m_logger = new QStatusBar();
  // setStatusBar(m_logger);
  // max main window
  showMaximized();
}

MainWindow::~MainWindow() {}
