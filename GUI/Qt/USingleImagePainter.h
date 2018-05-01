#ifndef USINGLEIMAGEPAINTER_H
#define USINGLEIMAGEPAINTER_H

#include <QWidget>
#include <QPainter>
#include <QMutex>
#include <QMutexLocker>
#include <QMouseEvent>
#include <QList>
#include <QPolygon>
#include <QMenu>
#include <QRectF>

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

  QPen getPen() const;
  void setPen(const QPen &value);

  // ---------- POLYGONS ----------

  bool getDrawMode() const;
  void setDrawable(bool value);

  bool isDrawable() const;

  /// Устанавливает набор полигонов на изображение
  void setPolygons(const QList<UDrawablePolygon> &polygons);

  /// устанавливает зону как выбранную в выделеноне изображение (selectedImage)
  void selectPolygon(int id);

  /// Если есть выбранная зона, отправляет сигнал polygonSelected(int)
  int getSelectedPolygonID();

  // ---------- RECTANGLES --------

  /// Устанавливает режим рисования двух прямоугольников левой и правой кнопками мыши
  void setDrawRects(bool value);

  /// Устанавливает два прямоугольника в текущий rectangles
  void setRectangles(const QPair<QRectF, QRectF> &rects);

  // ------------------------------

private:
  /// Указатель на изображение к отрисовке, поступает от UImageLoader через слот void setImage(QImage* image)
  QImage *dispImage;
  /// Мьютекс копирования изображения, разделен с UImageLoader
  QMutex *loaderMutex;

  /// Текущий стиль отрисовки линий
  QPen pen;

  // ---------- POLYGONS ----------

  /// Флаг позволяющий рисовать на текущем виджете
  /// Если флаг возведен - рисуется полигон, ПКМ завершает рисование полигона
  bool drawable;

  /// Флаг включения режима рисования последнего полигона в списке figures.
  /// Возводится после отрисовки первой точки полигона.
  /// Полностью скрытый флаг, извне не устанавливается.
  bool drawMode;

  /// Список полигонов к отображению
  QList<UDrawablePolygon> polygons;

  /// Выбранная зона
  int selectedPolygonId;

  /// Указатель на выбранную зону, также служит флагом
  UDrawablePolygon *selectedPolygon;

  /// Указатель на передвигаемую точку по museMoveEvent, также служит флагом
  QPointF *movingPoint;

  /// Флаг, возводится при перемещении точки выделенной зоны
  bool isPolygonModified;

  /// Меню с событием для добавления точки на зону, вызывается с ПКМ на линии выбранной зоны
  QMenu *lineMenu;

  /// Меню с событием удаления точки из зоны
  QMenu *pointMenu;


  /// Итератор и точка для добавления после события контекстного меню линии
  QPair<QPolygonF::iterator, QPointF> additionPoint;

  /// Итератор на точку для удаления
  QPolygonF::iterator deletePointIterator;

  // ---------- RECTANGLES --------

  /// Флаг, позволяющий рисовать пару прямоугольников на текущем виджете
  /// Если флаг возведен, ЛКМ рисует зеленый прямоугольник, ПКМ рисует голубенький прямоугольник;
  bool drawableRect;

  /// Флаг, возводится, когда рисуется новый прямоугольник
  enum RectType {LMBRect, RMBRect, NoRect} drawRectMode;

  /// Зелёный (ЛКМ) и голубенький (ПКМ) прямоугольники.
  /// Прямоугольники рисуются, только если активен режим drawableRect
  QPair<QRectF, QRectF> rectangles;

  // ------------------------------

  // methods

  /// Рассчитывает смещение dx dy, необходимое чтобы изображение находилось в центре виджета
  QPair<int, int> calcImgShift(const QSize &widgetSize, const QSize &imageSize) const;

  /// Если выбранная зона изменяласьь, то испускает сигнал polygonModified
  void commitPolygonChangeIfModified();

private slots:
  /// Слот добавления точки в выбранную зону зону
  void addPoint();

  /// Слот удаления выбранной точки
  void deletePoint();

protected:
  virtual void paintEvent(QPaintEvent*) override;

  virtual void mousePressEvent(QMouseEvent *event) override;

  virtual void mouseReleaseEvent(QMouseEvent *event) override;

  virtual void mouseMoveEvent(QMouseEvent *event) override;

signals:
  void setedImageSize(QSize);
  void polygonFinished(QPolygonF, QSize);
  void polygonModified(UDrawablePolygon, QSize);
  void polygonSelected(int);

  void rectanglesChanged(QPair<QRectF, QRectF>);

public slots:

  /// Отрисовывает изображение в текущем виджете
  void setImage(QImage *image);
};

#endif // USINGLEIMAGEPAINTER_H
