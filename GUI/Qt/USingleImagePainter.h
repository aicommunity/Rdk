#ifndef USINGLEIMAGEPAINTER_H
#define USINGLEIMAGEPAINTER_H

#include <QWidget>
#include <QPainter>
#include <QMutex>
#include <QMutexLocker>
#include <QMouseEvent>
#include <QList>
#include <QPolygon>

struct UDrawablePolygon
{
  UDrawablePolygon(int _id, QPolygonF _polygon, QPen _pen):
    id(_id), polygon(_polygon), pen(_pen)
  {}

  int id;
  QPolygonF polygon;
  QPen pen;
};

/// Виджет рисования изображения от UImageLoader, также позволяет рисовать геометрические объекты
class USingleImagePainter : public QWidget
{
  Q_OBJECT
public:
  explicit USingleImagePainter(QWidget *parent = 0);

  void setLoaderMutex(QMutex *mutex);

  bool getDrawMode() const;
  void setDrawable(bool value);

  bool isDrawable() const;

  /// Устанавливает набор полигонов на изображение
  void setZones(QList<UDrawablePolygon> polygons);

  /// устанавливает зону как выбранную в выделеноне изображение (selectedImage)
  void selectZone(int id);

  QPen getPen() const;
  void setPen(const QPen &value);

private:
  /// Указатель на изображение к отрисовке, поступает от UImageLoader через слот void setImage(QImage* image)
  QImage *dispImage;
  /// Мьютекс копирования изображения, разделен с UImageLoader
  QMutex *loaderMutex;

  QPen pen;

  /// Список полигонов к отображению
  QList<UDrawablePolygon> figures;

  /// Выбранная зона
  int selectedZone;

  /// Флаг включения режима рисования последнего полигона в списке figures
  bool drawMode;

  /// Флаг позволяющий рисовать на текущем виджете
  bool drawable;

  /// Флаг, означающий что на текущем виджете есть выбранная зона
  bool zoneSelected;

  bool pointCaptured;

  QPointF* movingPoint;

protected:
  virtual void paintEvent(QPaintEvent*) override;

  /// Рисование на изображении, в случае возведения флага drawable
  virtual void mousePressEvent(QMouseEvent * event) override;

  virtual void mouseReleaseEvent(QMouseEvent * event) override;

  virtual void mouseMoveEvent(QMouseEvent * event) override;

signals:
  void setedImageSize(QSize);
  void zoneFinished(QPolygonF, QSize);

public slots:

  /// Отрисовывает изображение в текущем виджете
  void setImage(QImage* image);
};

#endif // USINGLEIMAGEPAINTER_H
