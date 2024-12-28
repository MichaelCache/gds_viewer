#include <QApplication>

#include "gui/MainWindow.h"

int main(int argc, char *argv[]) {
  // qt6 enable high dpi by default
  // QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QApplication a(argc, argv);
  // load translation
  //   QTranslator translator;
  //   const QStringList uiLanguages = QLocale::system().uiLanguages();
  //   for (const QString &locale : uiLanguages)
  //   {
  //     const QString baseName = "qpanda_gui_" + QLocale(locale).name();
  //     if (translator.load(":/i18n/" + baseName))
  //     {
  //       a.installTranslator(&translator);
  //       break;
  //     }
  //   }

  MainWindow w;

  w.show();
  return a.exec();
}
