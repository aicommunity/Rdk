#include <QDebug>

#include <QApplication>
#include <QByteArray>
#include <QDataStream>
#include <QDrag>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QLabel>
#include <QMimeData>
#include <QModelIndex>
#include <QMouseEvent>
#include <QPainter>

#include "ComponentList.h"

ComponentList::ComponentList(QWidget *parent)
  : QTableView(parent)
{
  setMinimumSize(50, 50);
}

ComponentList::ComponentList(QStringList& components, QWidget *parent)
{
  setMinimumSize(50, 50);
  horizontalHeader()->setStretchLastSection(true);
  
  model = new QStringListModel(components, this);
  setModel(model);

  setDragEnabled(true);

}

ComponentList::~ComponentList()
{

}

void ComponentList::mousePressEvent(QMouseEvent *event)
{
  // сначала выделить нужную €чейку
  QPoint pos = event->pos();
  QPersistentModelIndex index = indexAt(pos);

  if (!index.isValid())
    return;

  setCurrentIndex(index);

  // check how far the mouse cursor has moved
  // and if it's far enough start the DnD operation
  // about dnd distance: http://qt-project.org/doc/qt-5.0/qtgui/qstylehints.html#startDragDistance

  QByteArray itemData;
  QDataStream dataStream(&itemData, QIODevice::WriteOnly);
  dataStream << index.data().toString() << 0;

  QMimeData *mimeData = new QMimeData();
  mimeData->setData("Component", itemData);

  QDrag *drag = new QDrag(this);
  drag->setMimeData(mimeData);

  // we can only copy data from the list and the list remains as it is
  drag->exec(Qt::CopyAction, Qt::CopyAction);
}