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
  void setZones(QList<UDrawablePolygon> polygons);

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
  int selectedZone;

  /// ���� ��������� ������ ��������� ���������� �������� � ������ figures
  bool drawMode;

  /// ���� ����������� �������� �� ������� �������
  bool drawable;

  /// ����, ���������� ��� �� ������� ������� ���� ��������� ����
  bool zoneSelected;

  bool pointCaptured;

  QPointF* movingPoint;

protected:
  virtual void paintEvent(QPaintEvent*) override;

  /// ��������� �� �����������, � ������ ���������� ����� drawable
  virtual void mousePressEvent(QMouseEvent * event) override;

  virtual void mouseReleaseEvent(QMouseEvent * event) override;

  virtual void mouseMoveEvent(QMouseEvent * event) override;

signals:
  void setedImageSize(QSize);
  void zoneFinished(QPolygonF, QSize);

public slots:

  /// ������������ ����������� � ������� �������
  void setImage(QImage* image);
};

#endif // USINGLEIMAGEPAINTER_H
