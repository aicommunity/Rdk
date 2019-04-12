#ifndef USINGLEIMAGEWIDGET_H
#define USINGLEIMAGEWIDGET_H

#include "UVisualControllerWidget.h"

#include <QWidget>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QThread>

#include "USingleImagePainter.h"
#include "UImageLoader.h"

namespace Ui {
class USingleImageWidget;
}

class USingleImageWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit USingleImageWidget(QWidget *parent = 0, int row = 0, int column = 0, int channel = 0,  bool showLegend = true, bool indChannels = false, int imagesSizeMod = 0);
    virtual ~USingleImageWidget();

    virtual void AUpdateInterface();

    int getColumn() const;
    void setColumn(int value);

    int getRow() const;
    void setRow(int value);

    QImage getImage() const;
    //void setImage(const QImage &value);

    bool getShowLegend() const;
    int getSizeMode() const;

    bool getSelected() const;
    void setSelected(bool value);

    int getImageWidth();
    int getImageHeight();

    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

    QString getComponentPropertyName() const;
    void setComponentPropertyName(const QString &value);

    QString getComponentName() const;
    void setComponentName(const QString &value);

    bool getConnected() const;

    int getCalcChannel() const;
    void setCalcChannel(int value);

    void setShowChannels(bool value);

    // ----- Проброс в UImagePainter -----
    /// Устанавливает набор полигонов на изображение
    void setPolygons(const QList<UDrawablePolygon> &polygons);

    /// Устанавливает QPen в painter
    void setPainterPen(const QPen &value);

    /// Устанавливает режим рисования на текущий Painter
    void setDrawable(bool value);

    /// устанавливает зону как выбранную
    void selectPolygon(int id);

    /// Устанавливает режим рисования двух прямоугольников левой и правой кнопками мыши на текущий Painter
    void setDrawRects(bool value);

    /// Устанавливает два прямоугольника в текущий Painter
    void setRectangles(const QPair<QRectF, QRectF> &rects);

    // ------------------------------

public slots:
    void reDrawWidget();
    void setSize(int value);
    void setShowLegend(bool value);

    void setImageSizeInfo(QSize size);

signals:
    // signals to UImagesWidget
    void selectionSignal(USingleImageWidget *item);
    void fullScreenSignal(USingleImageWidget *item);

    // signals to UImageLoader
    void loadImage(QSize value);
    void resizeImage(QSize value);

    // signals from USingleImagePainter
    void polygonFinished(QPolygonF, QSize);
    void polygonModified(UDrawablePolygon, QSize);
    void polygonSelected(int);
    void rectanglesChanged(QPair<QRectF, QRectF>);

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    Ui::USingleImageWidget *ui;

    int column;
    int row;

    bool selected;

    //buffers
    RDK::UBitmap tempBmp;
    RDK::UBitmapParam bmp_param;

    QThread *thread;
    UImageLoader *imageLoader;
    USingleImagePainter *painter;
};

#endif // USINGLEIMAGEWIDGET_H
