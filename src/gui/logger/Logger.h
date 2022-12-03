#pragma once

#include <QTextEdit>

void redirectMessageHandle(QtMsgType, const QMessageLogContext &,
                           const QString &);

class SingletonLogger : public QWidget {
  Q_OBJECT
 public:
  static SingletonLogger &instance();
 signals:
  void log(const QString &info);

 private:
  SingletonLogger(QWidget *parent = nullptr);
  ~SingletonLogger();
};