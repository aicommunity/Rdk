#include "USingleImagePainter.h"


USingleImagePainter::USingleImagePainter(QWidget *parent):QWidget(parent), pen(Qt::green, 3)
{
  dispImage = NULL;
  loaderMutex = NULL;
  drawMode = false;
  drawable = false;
  selectedZoneId = -1;
  selectedZone = NULL;
  movingPoint = NULL;
  isZoneModified = false;
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
  if(value) selectedZoneId = -1;
}

bool USingleImagePainter::isDrawable() const
{
  return drawable;
}

void USingleImagePainter::setZones(const QList<UDrawablePolygon> &polygons)
{
  commitZoneChangeIfModified();

  figures = polygons;
}

void USingleImagePainter::selectZone(int id)
{
  commitZoneChangeIfModified();

  selectedZoneId = id;
}

QPen USingleImagePainter::getPen() const
{
  return pen;
}

void USingleImagePainter::setPen(const QPen &value)
{
  pen = value;
}

QPair<int, int> USingleImagePainter::calcImgShift(const QSize &widgetSize, const QSize &imageSize) const
{
  return qMakePair(
        (widgetSize.width() > imageSize.width()) ?
          widgetSize.width()/2 - imageSize.width()/2 : 0,
        (widgetSize.height() > imageSize.height()) ?
          widgetSize.height()/2 - imageSize.height()/2 : 0
          );
}

void USingleImagePainter::commitZoneChangeIfModified()
{
  if(isZoneModified)
  {
    isZoneModified = false;
    if(selectedZone && dispImage)
      emit zoneModified(*selectedZone, dispImage->size());
  }
}

void USingleImagePainter::paintEvent(QPaintEvent *)
{
  if(!dispImage || !loaderMutex) return;
  QPainter painter(this);

  // ���������� ���� - ��������� �����������
  loaderMutex->lock();
  const QSize imgSize = dispImage->size();
  QPair<int, int> dxdy = calcImgShift(size(), imgSize);
  QRect imgRect = dispImage->rect();
  imgRect.moveTo(dxdy.first, dxdy.second);

  painter.drawImage(imgRect, *dispImage);
  loaderMutex->unlock();

  // ��������� ���� ���������
  selectedZone = NULL;
  for(QList<UDrawablePolygon>::iterator i = figures.begin(); i != figures.end(); ++i)
  {
    // ������� �������� �� ������������� ��������� � ��������
    UDrawablePolygon drawablePolygon = *i;

    for(QPolygonF::iterator pointIterator = drawablePolygon.polygon.begin();
        pointIterator != drawablePolygon.polygon.end(); ++pointIterator)
    {
      pointIterator->setX(pointIterator->x() * imgSize.width() + static_cast<double>(dxdy.first));
      pointIterator->setY(pointIterator->y() * imgSize.height() + static_cast<double>(dxdy.second));
    }

    // �������� ����, ���� ��� ��������� selectedZone �� ������������� ������ �����,
    //�������� ������������ ������ �������� � ���������� ��������� �� ��������� ����
    //
    // ���� ���� �������� drawMode, ������ ������������ ����� ����
    if( selectedZoneId == drawablePolygon.id || selectedZoneId == -2)
    {
      if(selectedZoneId == drawablePolygon.id)
      {
        painter.setPen(QPen(Qt::yellow, 5));
        selectedZone = &(*i);
      }

      for(QPolygonF::iterator pointIterator = drawablePolygon.polygon.begin();
          pointIterator != drawablePolygon.polygon.end(); ++pointIterator)
      {
        painter.drawEllipse(*pointIterator, 3, 3);
      }
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
  if(dispImage /*&& (drawable || selectedZone)*/)
  {
    if(event->button() == Qt::LeftButton)
    {
      const QSize imgSize = dispImage->size();
      QPair<int, int> dxdy = calcImgShift(size(), imgSize);

      // ������� ����� � ������������� �����������
      QPointF point(
            static_cast<qreal>(event->x() - dxdy.first) / imgSize.width(),
            static_cast<qreal>(event->y() - dxdy.second) / imgSize.height());


      // ���� ������� ����� ���������, ��������� ����� ������
      if(drawable)
      {
        if(drawMode)
        {
          figures.last().polygon << point;
        }
        else
        {
          figures.push_back(UDrawablePolygon(-2, QPolygonF() << point, pen));
          drawMode = true;
        }
      }
      else
      {
        // ���� ���� ��������� ����, ��������� �� ������ �� �� � ������� ����
        if(selectedZone)
        {
          for(QPolygonF::iterator pointIterator = selectedZone->polygon.begin();
              pointIterator != selectedZone->polygon.end(); ++pointIterator)
          {
            QPointF currentPoint(*pointIterator);
            currentPoint.setX(pointIterator->x() * imgSize.width() + dxdy.first - 5);
            currentPoint.setY(pointIterator->y() * imgSize.height() + dxdy.second - 5);
            QRectF pointRect(currentPoint, QSizeF(10.0, 10.0));
            if(pointRect.contains(event->localPos()))
            {
              // ���� ����� �����, ����� �� �� ��������
              movingPoint = &(*pointIterator);
              break;
            }
          }
        }


        // ��������� ����
        for(QList<UDrawablePolygon>::iterator polygonsIterator = figures.begin();
            polygonsIterator != figures.end(); ++polygonsIterator)
        {
          if(polygonsIterator->polygon.containsPoint(point, Qt::OddEvenFill))
          {
            if(polygonsIterator->id == selectedZoneId)
              break;

            commitZoneChangeIfModified();

            selectedZoneId = polygonsIterator->id;
            emit zoneSelected(selectedZoneId);
            break;
          }
        }
      }
    }
    else
    {
      if(event->button() == Qt::RightButton)
      {
        // ���������� ��������� ����
        if(drawMode)
        {
          drawMode = false;
          emit zoneFinished(figures.last().polygon, dispImage->size());
          return;
        }
      }
    }
  }

  QWidget::mousePressEvent(event);
}

void USingleImagePainter::mouseReleaseEvent(QMouseEvent *event)
{
  if(movingPoint)
  {
    movingPoint = NULL;
  }

  QWidget::mouseReleaseEvent(event);
}

void USingleImagePainter::mouseMoveEvent(QMouseEvent *event)
{
  // ���� ��������� ����� ����, ������� �����
  if(dispImage && movingPoint)
  {
    const QSize imgSize = dispImage->size();
    QPair<int, int> dxdy = calcImgShift(size(), imgSize);
    movingPoint->setX(static_cast<double>(event->x() - dxdy.first)/imgSize.width());
    movingPoint->setY(static_cast<double>(event->y() - dxdy.second)/imgSize.height());
    isZoneModified = true;
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
