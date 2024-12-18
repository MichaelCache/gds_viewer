#include "CloseFileEvent.h"

const QEvent::Type CloseFileEvent::type =
    (QEvent::Type)QEvent::registerEventType();

CloseFileEvent::CloseFileEvent() : QEvent(QEvent::Type(CloseFileEvent::type)) {}