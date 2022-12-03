#include "Kernel.h"

#include <QDebug>

#include "../event/OpenGdsEvent.h"

Kernel::Kernel(QObject* parent) : QObject(parent) {
  EventDispacher::instance().registComp("Kernel", this);
}

Kernel::~Kernel() {}

bool Kernel::event(QEvent* e) {
  if (e->type() == OpenGdsEvent::type) {
    QString fn = dynamic_cast<OpenGdsEvent*>(e)->data();
    qDebug() << fn;
    gdstk::ErrorCode error_code;
    gdstk::Set<gdstk::Tag> tags = {0};
    m_gds_lib =
        gdstk::read_gds(fn.toLocal8Bit().data(), 0, 0, &tags, &error_code);

    qDebug() << QString(m_gds_lib.name);
    e->accept();
    return true;
  } else {
    return QObject::event(e);
  }
}