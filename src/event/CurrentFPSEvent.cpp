#include "CurrentFPSEvent.h"

const QEvent::Type CurrentFPSEvent::type =
    (QEvent::Type)QEvent::registerEventType();

CurrentFPSEvent::CurrentFPSEvent(int fps)
    : QEvent(QEvent::Type(CurrentFPSEvent::type)), m_fps(fps) {}