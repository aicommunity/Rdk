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

  lineMenu = new QMenu(this);
  QAction *actionAddPoint = new QAction(lineMenu);
  actionAddPoint->setText("add Point");
  connect(actionAddPoint, SIGNAL(triggered()), this, SLOT(addPoint()));
  lineMenu->addAction(actionAddPoint);

  pointMenu = new QMenu(this);
  QAction *actionDelPoint = new QAction(pointMenu);
  actionDelPoint->setText("delete Point");
  connect(actionDelPoint, SIGNAL(triggered()), this, SLOT(deletePoint()));
  pointMenu->addAction(actionDelPoint);
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
  if(value)
  {
    selectedZoneId = -1;
    commitZoneChangeIfModified();
  }
}

bool USingleImagePainter::isDrawable() const
{
  return drawable;
}

void USingleImagePainter::setZones(const QList<UDrawablePolygon> &polygons)
{
  //commitZoneChangeIfModified();
  if(isZoneModified)
  {
    isZoneModified = false;
    if(selectedZone && dispImage)
      emit zoneModified(*selectedZone, dispImage->size());
  }
  else
  {
    figures = polygons;
  }
}

void USingleImagePainter::selectZone(int id)
{
  commitZoneChangeIfModified();

  selectedZoneId = id;
  selectedZone = NULL;
}

QPen USingleImagePainter::getPen() const
{
  return pen;
}

void USingleImagePainter::setPen(const QPen &value)
{
  pen = value;
}

void USingleImagePainter::emitSelectedZone()
{
  if(selectedZoneId >= 0)
  {
    //commitZoneChangeIfModified();
    emit zoneSelected(selectedZoneId);
  }
}

QPair<int, int> USingleImagePainter::calcImgShift(const QSize &widgetSize, const QSize &imageSize) const
{
  return qMakePair(
        (widgetSize.width() > imageSize.width()) ?
          widgetSize.width()/2 - imageSize.width()/2 : 0,
        (widgetSize.height() > imageSize.height()) ?
          widgetSize.height()/2 - imageSize.height()/2 : 0);
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

void USingleImagePainter::addPoint()
{
  if(selectedZone)
  {
    if(additionPoint.first != selectedZone->polygon.begin()
       && additionPoint.first != selectedZone->polygon.end())
      selectedZone->polygon.insert(additionPoint.first, additionPoint.second);
    else
      selectedZone->polygon.push_back(additionPoint.second);

    isZoneModified = true;
  }
}

void USingleImagePainter::deletePoint()
{
  if(selectedZone)
  {
    selectedZone->polygon.erase(deletePointIterator);
    isZoneModified = true;
    if(selectedZone->polygon.size() < 2)
      commitZoneChangeIfModified();
  }
}

void USingleImagePainter::paintEvent(QPaintEvent *)
{
  if(!dispImage || !loaderMutex) return;
  QPainter painter(this);

  // защищённая зона - отрисовка изображения
  loaderMutex->lock();
  const QSize imgSize = dispImage->size();
  const QPair<int, int> dxdy = calcImgShift(size(), imgSize);
  QRect imgRect = dispImage->rect();
  imgRect.moveTo(dxdy.first, dxdy.second);

  painter.drawImage(imgRect, *dispImage);
  loaderMutex->unlock();

  // отрисовка всех полигонов
  selectedZone = NULL;
  for(QList<UDrawablePolygon>::iterator i = figures.begin(); i != figures.end(); ++i)
  {
    // перевод полигона из относительных координат в экранные
    UDrawablePolygon drawablePolygon = *i;

    for(QPolygonF::iterator pointIterator = drawablePolygon.polygon.begin();
        pointIterator != drawablePolygon.polygon.end(); ++pointIterator)
    {
      pointIterator->setX(pointIterator->x() * imgSize.width() + dxdy.first);
      pointIterator->setY(pointIterator->y() * imgSize.height() + dxdy.second);
    }

    // проверка зоны, если она выбранная selectedZone то устанавливаем желтую кисть,
    // отдельно отрисовываем каждую вершинку и запоминаем указатель на выбранную зону
    //
    // если зона рисуется drawMode, просто отрисовываем точки зоны
    if( selectedZoneId == drawablePolygon.id || drawablePolygon.id == -2)
    {
      if(selectedZoneId == drawablePolygon.id)
      {
        painter.setPen(QPen(Qt::yellow, 4));
        selectedZone = &(*i);
      }
      else
      {
        painter.setPen(QPen(drawablePolygon.pen.color(), 4));
      }

      for(QPolygonF::iterator pointIterator = drawablePolygon.polygon.begin();
          pointIterator != drawablePolygon.polygon.end(); ++pointIterator)
      {
        painter.drawEllipse(*pointIterator, 2, 2);
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
  QWidget::mousePressEvent(event);

  if(!dispImage) return;

  const QSize imgSize = dispImage->size();
  const QPair<int, int> dxdy = calcImgShift(size(), imgSize);
  // текущая точка в относительных координатах
  const QPointF point(
        static_cast<qreal>(event->x() - dxdy.first) / imgSize.width(),
        static_cast<qreal>(event->y() - dxdy.second) / imgSize.height());

  if(event->button() == Qt::LeftButton)
  {

    // если активен режим рисования, добавляем точку фигуре
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

      // если есть выбранная зона, проверяем не попали ли мы в вершину зоны
      if(selectedZone)
      {
        QRectF arroundPoint(point.x() - 5.0 / imgSize.width(),
                           point.y() - 5.0 / imgSize.height(),
                           10.0 / imgSize.width(), 10.0/ imgSize.height());
        for(QPolygonF::iterator pointIterator = selectedZone->polygon.begin();
            pointIterator != selectedZone->polygon.end(); ++pointIterator)
        {
          if(arroundPoint.contains(*pointIterator))
          {
            // если нашли точку, берем на неё укзатель
            movingPoint = &(*pointIterator);
            return;
          }
        }
      }


      // выделение зоны
      for(QList<UDrawablePolygon>::iterator polygonsIterator = figures.begin();
          polygonsIterator != figures.end(); ++polygonsIterator)
      {
        if(polygonsIterator->polygon.containsPoint(point, Qt::OddEvenFill))
        {
          if(polygonsIterator->id == selectedZoneId)
            break;

          selectedZoneId = polygonsIterator->id;

          commitZoneChangeIfModified();

          emit zoneSelected(selectedZoneId);
          return;
        }
      }
    }
  }
  else
  {
    if(event->button() == Qt::RightButton)
    {

      // завершение рисования зоны
      if(drawMode)
      {
        drawMode = false;
        emit zoneFinished(figures.last().polygon, dispImage->size());
        return;
      }


      // если есть выбранная зона, проверяем не попали ли мы в вершину зоны
      if(selectedZone)
      {
        QRectF arroundPoint(point.x() - 5.0 / imgSize.width(),
                           point.y() - 5.0 / imgSize.height(),
                           10.0 / imgSize.width(), 10.0/ imgSize.height());
        for(QPolygonF::iterator pointIterator = selectedZone->polygon.begin();
            pointIterator != selectedZone->polygon.end(); ++pointIterator)
        {
          if(arroundPoint.contains(*pointIterator))
          {
            // если нашли точку, берем на неё укзатель
            deletePointIterator = pointIterator;
            pointMenu->exec(event->globalPos());

            return;
          }
        }
      }


      // добавление точки на линию
      if(selectedZone && selectedZone->polygon.size() > 1)
      {
        const QLineF pointHLine(
                point.x() - 5.0 / imgSize.width(), point.y(),
                point.x() + 5.0 / imgSize.width(), point.y());

        const QLineF pointVLine(
                point.x(), point.y() - 5.0 / imgSize.height(),
                point.x(), point.y() + 5.0 / imgSize.height());

        QPolygonF::iterator secondPI = selectedZone->polygon.begin();
        ++secondPI;
        for(QPolygonF::iterator firstPI = selectedZone->polygon.begin();
            firstPI != selectedZone->polygon.end(); ++firstPI)
        {
          const QLineF polyLine(*firstPI, *secondPI);

          if(QLineF::BoundedIntersection == polyLine.intersect(pointHLine, NULL)
             || QLineF::BoundedIntersection == polyLine.intersect(pointVLine, NULL))
          {
            additionPoint.first = secondPI;
            additionPoint.second = point;

            lineMenu->exec(event->globalPos());
            break;
          }

          ++secondPI;
          if(secondPI == selectedZone->polygon.end())
          {
            secondPI = selectedZone->polygon.begin();
          }
        }
      }

    }
  }
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
  // если захвачена точка зоны, двигаем точку
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
