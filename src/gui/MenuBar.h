#pragma once
#include <QAction>
#include <QMenu>
#include <QMenuBar>

class MenuBar : public QMenuBar {
  Q_OBJECT

 public:
  MenuBar(QWidget* parent = nullptr);
  ~MenuBar() = default;

  void setup();

 private slots:
  void openfile(bool checked = false);
  void closefile(bool checked = false);

 private:
  QMenu* m_file_menu;
  QAction* m_open;
  QAction* m_close;
};
