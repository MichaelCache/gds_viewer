#include "MenuBar.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QSizePolicy>

#include "event/CloseFileEvent.h"
#include "event/EventDispacher.h"
#include "event/OpenFileEvent.h"
#include "gui/logger/Logger.h"

MenuBar::MenuBar(QWidget* parent) : QMenuBar(parent) {
  m_file_menu = new QMenu("File");
  m_open = new QAction("Open");
  m_close = new QAction("Close");
  m_file_menu->addAction(m_open);
  m_file_menu->addAction(m_close);
  addMenu(m_file_menu);

  connect(m_open, SIGNAL(triggered(bool)), this, SLOT(openfile(bool)));
  connect(m_close, SIGNAL(triggered(bool)), this, SLOT(closefile(bool)));
}

void MenuBar::setup() {
  auto& dispacher = EventDispacher::instance();
  // dispacher.
}

void MenuBar::openfile(bool checked) {
  QString fn = QFileDialog::getOpenFileName(
      nullptr, tr("Open gds"), QDir::homePath(), "gds file (*.gds)");
  if (!fn.isEmpty()) {
    // send file open event to LayoutCanvas
    qDebug() << "load " << fn;
    QApplication::postEvent(
        EventDispacher::instance().getComp(EventComp::LayoutCanvas),
        new OpenFileEvent(fn));
  }
}

void MenuBar::closefile(bool checked) {
  qDebug() << "close file";
  QApplication::postEvent(
      EventDispacher::instance().getComp(EventComp::LayoutCanvas),
      new CloseFileEvent());
  QApplication::postEvent(
      EventDispacher::instance().getComp(EventComp::CellList),
      new CloseFileEvent());
}
