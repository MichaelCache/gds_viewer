#include "CentralWidget.h"

#include <QDateTime>
// #include <QDesktopWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>

#include "logger/Logger.h"

CentralWidget::CentralWidget(QWidget *parent) : QWidget(parent) {
  createComponent();
  initLayout();
}

CentralWidget::~CentralWidget() {}

void CentralWidget::createComponent() {
  m_layout_canvas = new LayoutCanvas(this);

  m_top_cell_list = new TopCellList(this);
  // m_circuit_scence = new CircuitScene(this);
  m_logger = new QTextEdit();
  m_logger->setReadOnly(true);

  // get redirected debug output, write to textedit
  connect(&SingletonLogger::instance(), SIGNAL(log(const QString &)), m_logger,
          SLOT(append(const QString &)));

  // m_circute_view->setScene(m_circuit_scence);
  // m_circute_view->setCacheMode(QGraphicsView::CacheBackground);

  // // redirect qdebug to SingletonLogger
  qInstallMessageHandler(redirectMessageHandle);
}

void CentralWidget::initLayout() {
  QVBoxLayout *main_layout = new QVBoxLayout();

  QHBoxLayout *top_layout = new QHBoxLayout();
  top_layout->addWidget(m_top_cell_list, 1);
  top_layout->addWidget(m_layout_canvas, 9);

  main_layout->addLayout(top_layout, 9);
  // main_layout->addStretch(40);
  main_layout->addWidget(m_logger, 1);
  setLayout(main_layout);
}
