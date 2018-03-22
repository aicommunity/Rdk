#include "USingleImagePainter.h"


USingleImagePainter::USingleImagePainter(QWidget *parent):QWidget(parent), pen(Qt::green, 3)
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
  drawMode = false;
}

bool USingleImagePainter::isDrawable() const
{
  return drawable;
}

void USingleImagePainter::setZones(QList<QPair<QPolygonF, QPen> > polygons)
{
  figures = polygons;
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
  for(QList<QPair<QPolygonF, QPen>>::iterator i = figures.begin(); i != figures.end(); ++i)
  {
    QPair<QPolygonF, QPen> pairPolygonPen = *i;
    painter.setPen(pairPolygonPen.second);
    for(QPolygonF::iterator pointIterator = pairPolygonPen.first.begin();
        pointIterator != pairPolygonPen.first.end(); ++pointIterator)
    {
      pointIterator->setX(pointIterator->x() * imgSize.width() + static_cast<double>(dx));
      pointIterator->setY(pointIterator->y() * imgSize.height() + static_cast<double>(dy));
    }
    painter.drawPolygon(pairPolygonPen.first);
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
        figures.last().first << point;
      }
      else
      {
        figures.push_back(QPair<QPolygonF, QPen>(QPolygonF() << point, pen));
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
          emit zoneFinished(figures.last().first, dispImage->size());
          return;
        }
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
