#include "OpenFileEvent.h"

const QEvent::Type OpenFileEvent::type =
    (QEvent::Type)QEvent::registerEventType();

OpenFileEvent::OpenFileEvent(const QString& fn)
    : QEvent(QEvent::Type(OpenFileEvent::type)), m_openfn(fn) {}

const QString& OpenFileEvent::fileName() { return m_openfn; }