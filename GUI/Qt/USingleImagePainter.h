#ifndef USINGLEIMAGEPAINTER_H
#define USINGLEIMAGEPAINTER_H

#include <QWidget>
#include <QPainter>
#include <QMutex>
#include <QMutexLocker>
#include <QMouseEvent>
#include <QList>
#include <QPolygon>

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
  void setZones(QList<QPair<QPolygonF, QPen> > polygons);

  QPen getPen() const;
  void setPen(const QPen &value);

private:
  /// ��������� �� ����������� � ���������, ��������� �� UImageLoader ����� ���� void setImage(QImage* image)
  QImage *dispImage;
  /// ������� ����������� �����������, �������� � UImageLoader
  QMutex *loaderMutex;

  QPen pen;

  /// ������ ��������� � �����������
  QList<QPair<QPolygonF, QPen> > figures;

  /// ���� ��������� ������ ��������� ���������� �������� � ������ figures
  bool drawMode;

  /// ���� ����������� �������� �� ������� �������
  bool drawable;

protected:
  virtual void paintEvent(QPaintEvent*) override;

  /// ��������� �� �����������, � ������ ���������� ����� drawable
  virtual void mousePressEvent(QMouseEvent * event) override;

signals:
  void setedImageSize(QSize);
  void zoneFinished(QPolygonF, QSize);

public slots:

  /// ������������ ����������� � ������� �������
  void setImage(QImage* image);
};

#endif // USINGLEIMAGEPAINTER_H
