#include <QDrag>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QPainter>
#include <QDebug>

#include "PictureArea.h"
#include "ComponentLabel.h"
#include "../../../Rtv/Deploy/Include/rtv_initdll.h"

PictureArea::PictureArea(QWidget *parent)
  : QFrame(parent)
{
  setMinimumSize(50, 50);
  setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
  setAcceptDrops(true);
}

PictureArea::~PictureArea()
{

}

void PictureArea::dragEnterEvent(QDragEnterEvent *event)
{
  qDebug() << "dragEnterEvent";

  if (event->mimeData()->hasFormat("Component")) 
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  } 
  else 
  {
    event->ignore();
  }
}

void PictureArea::dragMoveEvent(QDragMoveEvent *event)
{
  if (event->mimeData()->hasFormat("Component")) 
  {
    event->setDropAction(Qt::MoveAction);
    event->accept();
  } 
  else 
  {
    event->ignore();
  }
}

void PictureArea::dropEvent(QDropEvent *event)
{
  if (event->mimeData()->hasFormat("Component")) 
  {
    QByteArray itemData = event->mimeData()->data("Component");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);

    QString text;
    QPoint offset;
    dataStream >> text >> offset;

    ComponentLabel *newIcon = new ComponentLabel(text, event->pos() - offset, this);
    if (event->source() != this)
    {
      newIcon->move(event->pos() - QPoint(newIcon->width()/2, newIcon->height()/2));
    }

    if (event->source() != this)
    {
     Model_AddComponent(Env_GetCurrentComponentName(), text.toLocal8Bit());
     qDebug() << "The model root contains: " << Model_GetComponentsNameList("");
    }
  } 
  else 
  {
    event->ignore();
  }
}

void PictureArea::mousePressEvent(QMouseEvent *event)
{
  ComponentLabel *child = static_cast<ComponentLabel*>(childAt(event->pos()));
  if (!child)
    return;

  QPoint hotSpot = event->pos() - child->pos();
    
  QByteArray itemData;
  QDataStream dataStream(&itemData, QIODevice::WriteOnly);
  dataStream << child->LabelText() << QPoint(hotSpot);

  QMimeData *mimeData = new QMimeData;
  mimeData->setData("Component", itemData);

  QDrag *drag = new QDrag(this);

  drag->setMimeData(mimeData);
  drag->setHotSpot(hotSpot);

  drag->setPixmap(*child->pixmap());
  
  child->close();

  // we only can move items from here
  drag->exec(Qt::MoveAction, Qt::MoveAction);
  
}

QSize PictureArea::sizeHint() const
{
  return QSize(500, 500);
}
