#ifndef USINGLEIMAGEPAINTER_H
#define USINGLEIMAGEPAINTER_H

#include <QWidget>
#include <QPainter>
#include <QMutex>
#include <QMutexLocker>
#include <QMouseEvent>
#include <QList>
#include <QPolygon>

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
  void setZones(QList<QPair<QPolygonF, QPen> > polygons);

  QPen getPen() const;
  void setPen(const QPen &value);

private:
  /// Указатель на изображение к отрисовке, поступает от UImageLoader через слот void setImage(QImage* image)
  QImage *dispImage;
  /// Мьютекс копирования изображения, разделен с UImageLoader
  QMutex *loaderMutex;

  QPen pen;

  /// Список полигонов к отображению
  QList<QPair<QPolygonF, QPen> > figures;

  /// Флаг включения режима рисования последнего полигона в списке figures
  bool drawMode;

  /// Флаг позволяющий рисовать на текущем виджете
  bool drawable;

protected:
  virtual void paintEvent(QPaintEvent*) override;

  /// Рисование на изображении, в случае возведения флага drawable
  virtual void mousePressEvent(QMouseEvent * event) override;

signals:
  void setedImageSize(QSize);
  void zoneFinished(QPolygonF, QSize);

public slots:

  /// Отрисовывает изображение в текущем виджете
  void setImage(QImage* image);
};

#endif // USINGLEIMAGEPAINTER_H
