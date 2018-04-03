#include "USingleImagePainter.h"


USingleImagePainter::USingleImagePainter(QWidget *parent):QWidget(parent), pen(Qt::green, 3)
{
  dispImage = NULL;
  loaderMutex = NULL;
  drawMode = false;
  drawable = false;
  selectedZone = -1;
  pointCaptured = false;
  movingPoint = NULL;
}

void USingleImagePainter::setLoaderMutex(QMutex *mutex)
{
  loaderMutex = mutex;
}

bool USingleImagePainter::getDrawMode() const
{
  return drawMode;
}

void USingleImagePainter::setDrawable(bool value)
{
  drawable = value;
  drawMode = false;
  if(value) selectedZone = -1;
}

bool USingleImagePainter::isDrawable() const
{
  return drawable;
}

void USingleImagePainter::setZones(QList<UDrawablePolygon> polygons)
{
  figures = polygons;
}

void USingleImagePainter::selectZone(int id)
{
  selectedZone = id;
}

QPen USingleImagePainter::getPen() const
{
  return pen;
}

void USingleImagePainter::setPen(const QPen &value)
{
  pen = value;
}

void USingleImagePainter::paintEvent(QPaintEvent *)
{
  if(!dispImage || !loaderMutex) return;
  QPainter painter(this);

  loaderMutex->lock();
  // image position calculating
  QSize widgetSize = size();
  QSize imgSize = dispImage->size();
  int dx = (widgetSize.width() > imgSize.width()) ?
        widgetSize.width()/2 - imgSize.width()/2 : 0;
  int dy = (widgetSize.height() > imgSize.height()) ?
        widgetSize.height()/2 - imgSize.height()/2 : 0;
  QRect imgRect = dispImage->rect();
  imgRect.moveTo(dx, dy);

  // draw image
  painter.drawImage(imgRect, *dispImage);
  loaderMutex->unlock();


  // draw all rects
  for(QList<UDrawablePolygon>::iterator i = figures.begin(); i != figures.end(); ++i)
  {
    UDrawablePolygon drawablePolygon = *i;

    for(QPolygonF::iterator pointIterator = drawablePolygon.polygon.begin();
        pointIterator != drawablePolygon.polygon.end(); ++pointIterator)
    {
      pointIterator->setX(pointIterator->x() * imgSize.width() + static_cast<double>(dx));
      pointIterator->setY(pointIterator->y() * imgSize.height() + static_cast<double>(dy));
    }

    if( selectedZone == drawablePolygon.id)
    {
      painter.setPen(QPen(Qt::yellow, 5));
      if(drawablePolygon.polygon.size() < 1) return;

      for(QPolygonF::iterator pointIterator = drawablePolygon.polygon.begin();
          pointIterator != drawablePolygon.polygon.end(); ++pointIterator)
      {
        painter.drawEllipse(*pointIterator, 3, 3);
      }

      zoneSelected = true;
    }
    else
    {
      painter.setPen(drawablePolygon.pen);
    }

    painter.drawPolygon(drawablePolygon.polygon);
  }
}

void USingleImagePainter::mousePressEvent(QMouseEvent *event)
{
  if(drawable && dispImage)
  {
    if(event->button() == Qt::LeftButton)
    {
      QSize imgSize = dispImage->size();
      QSize widgetSize = size();
      QPointF point = event->localPos();

      if(widgetSize.width() > imgSize.width())
        point.setX((point.x() - (widgetSize.width() - imgSize.width()) / 2) / imgSize.width());
      else
        point.setX(point.x() / imgSize.width());

      if(widgetSize.height() > imgSize.height())
        point.setY((point.y() - (widgetSize.height() - imgSize.height()) / 2) / imgSize.height());
      else
        point.setY(point.y() / imgSize.height());

      if(drawMode)
      {
        figures.last().polygon << point;
      }
      else
      {
        figures.push_back(UDrawablePolygon(-1, QPolygonF() << point, pen));
        drawMode = true;
      }
    }
    else
    {
      if(event->button() == Qt::RightButton)
      {
        if(drawMode)
        {
          drawMode = false;
          emit zoneFinished(figures.last().polygon, dispImage->size());
          return;
        }
      }
    }
  }

  if(zoneSelected)
  {

  }

  QWidget::mousePressEvent(event);
}

void USingleImagePainter::mouseReleaseEvent(QMouseEvent *event)
{
  if(pointCaptured && movingPoint)
  {
    pointCaptured = false;
    movingPoint = NULL;
  }

  QWidget::mouseReleaseEvent(event);
}

void USingleImagePainter::mouseMoveEvent(QMouseEvent *event)
{
  if(dispImage)
  {
    QSize widgetSize = size();
    QSize imgSize = dispImage->size();
    int dx = (widgetSize.width() > imgSize.width()) ?
          widgetSize.width()/2 - imgSize.width()/2 : 0;
    int dy = (widgetSize.height() > imgSize.height()) ?
          widgetSize.height()/2 - imgSize.height()/2 : 0;

    if(zoneSelected && !pointCaptured)
    {
      for(QList<UDrawablePolygon>::iterator i = figures.begin(); i != figures.end(); ++i)
      {
        if( selectedZone == i->id)
        {
          for(QPolygonF::iterator pointIterator = i->polygon.begin();
              pointIterator != i->polygon.end(); ++pointIterator)
          {
            QPointF currentPoint(*pointIterator);
            currentPoint.setX(pointIterator->x() * imgSize.width() + static_cast<double>(dx) - 5);
            currentPoint.setY(pointIterator->y() * imgSize.height() + static_cast<double>(dy) - 5);
            QRectF pointRect(currentPoint, QSizeF(10.0, 10.0));
            if(pointRect.contains(QPointF(event->pos())))
            {
              pointCaptured = true;
              movingPoint = &(*pointIterator);
              break;
            }
          }
          break;
        }
      }
    }

    if(pointCaptured && movingPoint)
    {
      movingPoint->setX(static_cast<double>(event->x() - dx)/imgSize.width());
      movingPoint->setY(static_cast<double>(event->y() - dy)/imgSize.height());
    }
  }

  QWidget::mouseMoveEvent(event);
}

void USingleImagePainter::setImage(QImage *image)
{
  if(!image) return;

  dispImage = image;
  emit setedImageSize(image->size());
  repaint();
}
