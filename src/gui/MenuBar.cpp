#include "MenuBar.h"

#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QApplication>

#include "event/OpenFileEvent.h"
#include "logger/Logger.h"

MenuBar::MenuBar(QWidget* parent) : QMenuBar(parent) {
  m_file_menu = new QMenu("File");
  m_open = new QAction("Open");
  m_file_menu->addAction(m_open);
  addMenu(m_file_menu);

  connect(m_open, SIGNAL(triggered(bool)), this, SLOT(openfile(bool)));
}

MenuBar::~MenuBar() {}

void MenuBar::setup() {
  auto& dispacher = EventDispacher::instance();
  // dispacher.
}

void MenuBar::openfile(bool checked) {
  QString fn = QFileDialog::getOpenFileName(
      nullptr, tr("Open gds"), QDir::homePath(), "gds file (*.gds)");
  qDebug() << fn;
  if (!fn.isEmpty()) {
    QApplication::postEvent(EventDispacher::instance().getComp("LayoutCanvas"), new OpenFileEvent(fn));
  }
}