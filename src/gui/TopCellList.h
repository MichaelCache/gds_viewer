#pragma once

#include <QListWidget>

class TopCellList : public QListWidget {
  Q_OBJECT
 public:
  explicit TopCellList(QWidget *parent = nullptr);
  ~TopCellList() = default;

 protected:
  bool event(QEvent *e) override;

 private slots:
  void onItemClicked(QListWidgetItem *item);
};