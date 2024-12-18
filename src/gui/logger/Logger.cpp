#include "Logger.h"

#include <QDateTime>
#include <QMouseEvent>
#include <QMutex>

SingletonLogger::SingletonLogger(QWidget *parent) : QWidget(parent) {}

SingletonLogger::~SingletonLogger() {}

SingletonLogger &SingletonLogger::instance() {
  static SingletonLogger logger;
  logger.setMouseTracking(true);
  return logger;
}

void redirectMessageHandle(QtMsgType type, const QMessageLogContext &context,
                           const QString &str) {
  SingletonLogger &logger = SingletonLogger::instance();
  QString message =
      QString("%1: %2")
          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
          .arg(str);

  emit logger.log(message);
}
