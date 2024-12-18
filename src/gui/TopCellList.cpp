#include "TopCellList.h"

#include "event/CloseFileEvent.h"
#include "event/EventDispacher.h"
#include "event/SetCellNamesEvent.h"

TopCellList::TopCellList(QWidget *parent) : QListWidget(parent) {
  EventDispacher::instance().registComp(EventComp::CellList, this);
  connect(this, &QListWidget::itemClicked, this, &TopCellList::onItemClicked);
}

bool TopCellList::event(QEvent *e) {
  if (e->type() == SetCellNamesEvent::type) {
    this->clear();
    auto event = dynamic_cast<SetCellNamesEvent *>(e);
    for (auto &&i : event->cellNames()) {
      this->addItem(i);
    }
    e->accept();
    return true;
  } else if (e->type() == CloseFileEvent::type) {
    this->clear();
    e->accept();
    return true;
  } else {
    return QListWidget::event(e);
  }
}

void TopCellList::onItemClicked(QListWidgetItem *item) {}
