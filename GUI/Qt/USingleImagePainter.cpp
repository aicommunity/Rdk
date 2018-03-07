#include "USingleImagePainter.h"


USingleImagePainter::USingleImagePainter(QWidget *parent):QWidget(parent)
{
  dispImage = NULL;
  loaderMutex = NULL;
  drawMode = false;
  drawable = false;
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
}

bool USingleImagePainter::isDrawable() const
{
  return drawable;
}

void USingleImagePainter::paintEvent(QPaintEvent *)
{
  if(!dispImage || !loaderMutex) return;
  QPainter painter(this);

  loaderMutex->lock();
  painter.drawImage(dispImage->rect(), *dispImage, dispImage->rect());
  QSize imgSize = dispImage->size();
  loaderMutex->unlock();

  painter.setPen(QPen(Qt::green, 3));

  for(QList<QPolygonF>::iterator i = figures.begin(); i != figures.end(); ++i)
  {
    QPolygonF poly = *i;
    for(QPolygonF::iterator pointIterator = poly.begin(); pointIterator != poly.end(); ++pointIterator)
    {
      pointIterator->setX(pointIterator->x() * imgSize.width());
      pointIterator->setY(pointIterator->y() * imgSize.height());
    }
    painter.drawPolygon(poly);
  }
}

void USingleImagePainter::mousePressEvent(QMouseEvent *event)
{
  if(drawable && dispImage)
  {
    if(event->button() == Qt::LeftButton)
    {
      QSize imgSize = dispImage->size();
      QPointF point = event->localPos();
      point.setX(point.x()/imgSize.width());
      point.setY(point.y()/imgSize.height());

      if(drawMode)
      {
        figures.last() << point;
      }
      else
      {
        figures.push_back(QPolygonF() << point);
        drawMode = true;
      }
    }

    if(event->button() == Qt::RightButton)
    {
      if(drawMode)
      {
        drawMode = false;
      }
    }
  }
  QWidget::mousePressEvent(event);
}

void USingleImagePainter::setImage(QImage *image)
{
  if(!image) return;

  dispImage = image;
  emit setedImageSize(image->size());
  repaint();
}
