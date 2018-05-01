#include "USingleImagePainter.h"


USingleImagePainter::USingleImagePainter(QWidget *parent):QWidget(parent), pen(Qt::green, 3)
{
  dispImage = NULL;
  loaderMutex = NULL;
  drawMode = false;
  drawable = false;
  selectedPolygonId = -1;
  selectedPolygon = NULL;
  movingPoint = NULL;
  isPolygonModified = false;

  drawableRect = false;
  drawRectMode = NoRect;
  rectangles = qMakePair(QRectF(0.0, 0.0, 0.0, 0.0), QRectF(0.0, 0.0, 0.0, 0.0));

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
    selectedPolygonId = -1;
    commitPolygonChangeIfModified();
  }
}

bool USingleImagePainter::isDrawable() const
{
  return drawable;
}

void USingleImagePainter::setPolygons(const QList<UDrawablePolygon> &polygons)
{
  if(isPolygonModified)
  {
    isPolygonModified = false;
    if(selectedPolygon && dispImage)
      emit polygonModified(*selectedPolygon, dispImage->size());
  }
  else
  {
    this->polygons = polygons;
  }
}

void USingleImagePainter::selectPolygon(int id)
{
  commitPolygonChangeIfModified();

  selectedPolygonId = id;
  selectedPolygon = NULL;
}

QPen USingleImagePainter::getPen() const
{
  return pen;
}

void USingleImagePainter::setPen(const QPen &value)
{
  pen = value;
}

int USingleImagePainter::getSelectedPolygonID()
{
  return selectedPolygonId;
}

void USingleImagePainter::setDrawRects(bool value)
{
  drawableRect = value;
}

void USingleImagePainter::setRectangles(const QPair<QRectF, QRectF> &rects)
{
  rectangles = rects;
}

QPair<int, int> USingleImagePainter::calcImgShift(const QSize &widgetSize, const QSize &imageSize) const
{
  return qMakePair(
        (widgetSize.width() > imageSize.width()) ?
          widgetSize.width()/2 - imageSize.width()/2 : 0,
        (widgetSize.height() > imageSize.height()) ?
          widgetSize.height()/2 - imageSize.height()/2 : 0);
}

void USingleImagePainter::commitPolygonChangeIfModified()
{
  if(isPolygonModified)
  {
    isPolygonModified = false;
    if(selectedPolygon && dispImage)
      emit polygonModified(*selectedPolygon, dispImage->size());
  }
}

void USingleImagePainter::addPoint()
{
  if(selectedPolygon)
  {
    if(additionPoint.first != selectedPolygon->polygon.begin()
       && additionPoint.first != selectedPolygon->polygon.end())
      selectedPolygon->polygon.insert(additionPoint.first, additionPoint.second);
    else
      selectedPolygon->polygon.push_back(additionPoint.second);

    isPolygonModified = true;
  }
}

void USingleImagePainter::deletePoint()
{
  if(selectedPolygon)
  {
    selectedPolygon->polygon.erase(deletePointIterator);
    isPolygonModified = true;
    if(selectedPolygon->polygon.size() < 2)
      commitPolygonChangeIfModified();
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
  selectedPolygon = NULL;
  for(QList<UDrawablePolygon>::iterator i = polygons.begin(); i != polygons.end(); ++i)
  {
    // перевод полигона из относительных координат в экранные
    UDrawablePolygon drawablePolygon = *i;

    for(QPolygonF::iterator pointIterator = drawablePolygon.polygon.begin();
        pointIterator != drawablePolygon.polygon.end(); ++pointIterator)
    {
      pointIterator->setX(pointIterator->x() * imgSize.width() + dxdy.first);
      pointIterator->setY(pointIterator->y() * imgSize.height() + dxdy.second);
    }

    // проверка полигона, если он выбран selectedPolygon то устанавливаем желтую кисть,
    // отдельно отрисовываем каждую вершинку и запоминаем указатель на выбранный ролигон
    //
    // если полиг рисуется drawMode, просто отрисовываем вершины полигона
    if( selectedPolygonId == drawablePolygon.id || drawablePolygon.id == -2)
    {
      if(selectedPolygonId == drawablePolygon.id)
      {
        painter.setPen(QPen(Qt::yellow, 4));
        selectedPolygon = &(*i);
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

  // отрисовка прямоугольников
  painter.setPen(QPen(Qt::green, 3));
  QRectF rect(rectangles.first.x() * imgSize.width() + dxdy.first,
              rectangles.first.y() * imgSize.height() + dxdy.second,
              rectangles.first.width() * imgSize.width(),
              rectangles.first.height() * imgSize.height());
  painter.drawRect(rect);

  painter.setPen(QPen(Qt::cyan, 3));
  rect.setRect(rectangles.second.x() * imgSize.width() + dxdy.first,
               rectangles.second.y() * imgSize.height() + dxdy.second,
               rectangles.second.width() * imgSize.width(),
               rectangles.second.height() * imgSize.height());
  painter.drawRect(rect);
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
        polygons.last().polygon << point;
      }
      else
      {
        polygons.push_back(UDrawablePolygon(-2, QPolygonF() << point, pen));
        drawMode = true;
      }
    }
    else
    {

      // если есть выбранный полигон, проверяем не попали ли мы в вершину полигона
      if(selectedPolygon)
      {
        QRectF arroundPoint(point.x() - 5.0 / imgSize.width(),
                           point.y() - 5.0 / imgSize.height(),
                           10.0 / imgSize.width(), 10.0/ imgSize.height());
        for(QPolygonF::iterator pointIterator = selectedPolygon->polygon.begin();
            pointIterator != selectedPolygon->polygon.end(); ++pointIterator)
        {
          if(arroundPoint.contains(*pointIterator))
          {
            // если нашли точку, берем на неё укзатель
            movingPoint = &(*pointIterator);
            return;
          }
        }
      }


      // выделение полигона
      for(QList<UDrawablePolygon>::iterator polygonsIterator = polygons.begin();
          polygonsIterator != polygons.end(); ++polygonsIterator)
      {
        if(polygonsIterator->polygon.containsPoint(point, Qt::OddEvenFill))
        {
          if(polygonsIterator->id == selectedPolygonId)
            break;

          selectedPolygonId = polygonsIterator->id;

          commitPolygonChangeIfModified();

          emit polygonSelected(selectedPolygonId);
          return;
        }
      }


      // рисование прямоугольников
      if(drawableRect)
      {
        drawRectMode = LMBRect;
        rectangles.first.setCoords(point.x(), point.y(), 0, 0);
      }
    }
  }
  else
  {
    if(event->button() == Qt::RightButton)
    {

      // завершение рисования нового полигона
      if(drawMode)
      {
        drawMode = false;
        emit polygonFinished(polygons.last().polygon, dispImage->size());
        return;
      }


      // если есть выбранный полигон, проверяем не попали ли мы в вершину полигона
      if(selectedPolygon)
      {
        QRectF arroundPoint(point.x() - 5.0 / imgSize.width(),
                           point.y() - 5.0 / imgSize.height(),
                           10.0 / imgSize.width(), 10.0/ imgSize.height());
        for(QPolygonF::iterator pointIterator = selectedPolygon->polygon.begin();
            pointIterator != selectedPolygon->polygon.end(); ++pointIterator)
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
      if(selectedPolygon && selectedPolygon->polygon.size() > 1)
      {
        const QLineF pointHLine(
                point.x() - 5.0 / imgSize.width(), point.y(),
                point.x() + 5.0 / imgSize.width(), point.y());

        const QLineF pointVLine(
                point.x(), point.y() - 5.0 / imgSize.height(),
                point.x(), point.y() + 5.0 / imgSize.height());

        QPolygonF::iterator secondPI = selectedPolygon->polygon.begin();
        ++secondPI;
        for(QPolygonF::iterator firstPI = selectedPolygon->polygon.begin();
            firstPI != selectedPolygon->polygon.end(); ++firstPI)
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
          if(secondPI == selectedPolygon->polygon.end())
          {
            secondPI = selectedPolygon->polygon.begin();
          }
        }
      }


      // рисование прямоугольников
      if(drawableRect)
      {
        drawRectMode = RMBRect;
        rectangles.first.setCoords(point.x(), point.y(), 0, 0);
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

  if(drawableRect)
  {
    drawRectMode = NoRect;
    emit rectanglesChanged(rectangles);
  }

  QWidget::mouseReleaseEvent(event);
}

void USingleImagePainter::mouseMoveEvent(QMouseEvent *event)
{
  QWidget::mouseMoveEvent(event);

  // если захвачена вершина полигона, двигаем точку
  if(dispImage && (movingPoint || drawableRect))
  {
    const QSize imgSize = dispImage->size();
    QPair<int, int> dxdy = calcImgShift(size(), imgSize);
    const QPointF point(
          static_cast<qreal>(event->x() - dxdy.first) / imgSize.width(),
          static_cast<qreal>(event->y() - dxdy.second) / imgSize.height());

    if(movingPoint)
    {
//      movingPoint->setX(static_cast<double>(event->x() - dxdy.first)/imgSize.width());
//      movingPoint->setY(static_cast<double>(event->y() - dxdy.second)/imgSize.height());
      *movingPoint = point;
      isPolygonModified = true;
      return;
    }

    if(drawableRect)
    {
      if(drawRectMode == NoRect)
        return;

      QRectF rect;
      qreal x1 = drawRectMode == LMBRect ? rectangles.first.x() : rectangles.second.x(),
            y1 = drawRectMode == LMBRect ? rectangles.first.y() : rectangles.second.y(),
            x2 = point.x(),
            y2 = point.y();

      if(x1 > x2)
      {
        if(y1 > y2) rect.setCoords(x2, y2, x1, y1);
        else rect.setCoords(x2, y1, x1, y2);
      }
      else
      {
        if(y1 > y2) rect.setCoords(x1, y2, x2, y1);
        else rect.setCoords(x1, y1, x2, y2);
      }

      if(drawRectMode == LMBRect)
        rectangles.first = rect;
      else
        rectangles.second = rect;
    }
  }

}

void USingleImagePainter::setImage(QImage *image)
{
  if(!image) return;

  dispImage = image;
  emit setedImageSize(image->size());
  repaint();
}
