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

  bool getDrawMode() const;
  void setDrawable(bool value);

  bool isDrawable() const;

  /// ������������� ����� ��������� �� �����������
  void setZones(const QList<UDrawablePolygon> &polygons);

  /// ������������� ���� ��� ��������� � ���������� ����������� (selectedImage)
  void selectZone(int id);

  QPen getPen() const;
  void setPen(const QPen &value);

private:
  /// ��������� �� ����������� � ���������, ��������� �� UImageLoader ����� ���� void setImage(QImage* image)
  QImage *dispImage;
  /// ������� ����������� �����������, �������� � UImageLoader
  QMutex *loaderMutex;

  QPen pen;

  /// ������ ��������� � �����������
  QList<UDrawablePolygon> figures;

  /// ��������� ����
  int selectedZoneId;

  /// ���� ��������� ������ ��������� ���������� �������� � ������ figures
  bool drawMode;

  /// ���� ����������� �������� �� ������� �������
  bool drawable;

  /// ����, ���������� ��� �� ������� ������� ���� ��������� ����
  UDrawablePolygon *selectedZone;

  /// ��������� �� ������������� ����� �� museMoveEvent, ����� ������ ������
  QPointF *movingPoint;

  /// ����, ���������� ��� ����������� ����� ���������� ����
  bool isZoneModified;

  QMenu *lineMenu;
  QMenu *pointMenu;

  /// ������������ �������� dx dy, ����������� ����� ����������� ���������� � ������ �������
  QPair<int, int> calcImgShift(const QSize &widgetSize, const QSize &imageSize) const;

  void commitZoneChangeIfModified();

private slots:
  void addPoint();
  void deletePoint();

protected:
  virtual void paintEvent(QPaintEvent*) override;

  /// ��������� �� �����������, � ������ ���������� ����� drawable
  virtual void mousePressEvent(QMouseEvent *event) override;

  virtual void mouseReleaseEvent(QMouseEvent *event) override;

  virtual void mouseMoveEvent(QMouseEvent *event) override;

signals:
  void setedImageSize(QSize);
  void zoneFinished(QPolygonF, QSize);
  void zoneModified(UDrawablePolygon, QSize);
  void zoneSelected(int);

public slots:

  /// ������������ ����������� � ������� �������
  void setImage(QImage *image);
};

#endif // USINGLEIMAGEPAINTER_H
