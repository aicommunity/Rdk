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

/// ������ ��������� ����������� �� UImageLoader, ����� ��������� �������� �������������� �������
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

  /// ������������� ����� ��������� �� �����������
  void setPolygons(const QList<UDrawablePolygon> &polygons);

  /// ������������� ���� ��� ��������� � ���������� ����������� (selectedImage)
  void selectPolygon(int id);

  /// ���� ���� ��������� ����, ���������� ������ polygonSelected(int)
  int getSelectedPolygonID();

  // ---------- RECTANGLES --------

  /// ������������� ����� ��������� ���� ��������������� ����� � ������ �������� ����
  void setDrawRects(bool value);

  /// ������������� ��� �������������� � ������� rectangles
  void setRectangles(const QPair<QRectF, QRectF> &rects);

  // ------------------------------

private:
  /// ��������� �� ����������� � ���������, ��������� �� UImageLoader ����� ���� void setImage(QImage* image)
  QImage *dispImage;
  /// ������� ����������� �����������, �������� � UImageLoader
  QMutex *loaderMutex;

  /// ������� ����� ��������� �����
  QPen pen;

  // ---------- POLYGONS ----------

  /// ���� ����������� �������� �� ������� �������
  /// ���� ���� �������� - �������� �������, ��� ��������� ��������� ��������
  bool drawable;

  /// ���� ��������� ������ ��������� ���������� �������� � ������ figures.
  /// ���������� ����� ��������� ������ ����� ��������.
  /// ��������� ������� ����, ����� �� ���������������.
  bool drawMode;

  /// ������ ��������� � �����������
  QList<UDrawablePolygon> polygons;

  /// ��������� ����
  int selectedPolygonId;

  /// ��������� �� ��������� ����, ����� ������ ������
  UDrawablePolygon *selectedPolygon;

  /// ��������� �� ������������� ����� �� museMoveEvent, ����� ������ ������
  QPointF *movingPoint;

  /// ����, ���������� ��� ����������� ����� ���������� ����
  bool isPolygonModified;

  /// ���� � �������� ��� ���������� ����� �� ����, ���������� � ��� �� ����� ��������� ����
  QMenu *lineMenu;

  /// ���� � �������� �������� ����� �� ����
  QMenu *pointMenu;


  /// �������� � ����� ��� ���������� ����� ������� ������������ ���� �����
  QPair<QPolygonF::iterator, QPointF> additionPoint;

  /// �������� �� ����� ��� ��������
  QPolygonF::iterator deletePointIterator;

  // ---------- RECTANGLES --------

  /// ����, ����������� �������� ���� ��������������� �� ������� �������
  /// ���� ���� ��������, ��� ������ ������� �������������, ��� ������ ����������� �������������;
  bool drawableRect;

  /// ����, ����������, ����� �������� ����� �������������
  enum RectType {LMBRect, RMBRect, NoRect} drawRectMode;

  /// ������ (���) � ����������� (���) ��������������.
  /// �������������� ��������, ������ ���� ������� ����� drawableRect
  QPair<QRectF, QRectF> rectangles;

  // ------------------------------

  // methods

  /// ������������ �������� dx dy, ����������� ����� ����������� ���������� � ������ �������
  QPair<int, int> calcImgShift(const QSize &widgetSize, const QSize &imageSize) const;

  /// ���� ��������� ���� �����������, �� ��������� ������ polygonModified
  void commitPolygonChangeIfModified();

private slots:
  /// ���� ���������� ����� � ��������� ���� ����
  void addPoint();

  /// ���� �������� ��������� �����
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

  /// ������������ ����������� � ������� �������
  void setImage(QImage *image);
};

#endif // USINGLEIMAGEPAINTER_H
