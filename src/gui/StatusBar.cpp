#include "StatusBar.h"

#include "event/CurrentFPSEvent.h"
#include "event/EventDispacher.h"

StatusBar::StatusBar(QWidget *parent) : QStatusBar(parent) {
  EventDispacher::instance().registComp(EventComp::MainStatusBar, this);
  m_fps_label = new QLabel(QString("FPS: %1").arg(0, 3, 10, QChar(' ')));
  addPermanentWidget(m_fps_label);
}

bool StatusBar::event(QEvent *e) {
  if (e->type() == CurrentFPSEvent::type) {
    auto event = dynamic_cast<CurrentFPSEvent *>(e);
    m_fps_label->setText(
        QString("FPS: %1").arg(event->m_fps, 3, 10, QChar(' ')));
    e->accept();
    return true;
  } else {
    return QStatusBar::event(e);
  }
}
