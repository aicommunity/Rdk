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

    void mousePressEvent(QMouseEvent*);
    void mouseDoubleClickEvent(QMouseEvent*);

    QString getComponentPropertyName() const;
    void setComponentPropertyName(const QString &value);

    QString getComponentName() const;
    void setComponentName(const QString &value);

    bool getConnected() const;

    int getCalcChannel() const;
    void setCalcChannel(int value);

    void setShowChannels(bool value);

public slots:
    void reDrawWidget();
    void setSize(int value);
    void setShowLegend(bool value);

signals:
    // signals to UImagesWidget
    void selectionSignal(USingleImageWidget *item);
    void fullScreenSignal(USingleImageWidget *item);

    // signals to UImageLoader
    void loadImage(QSize value);
    void resizeImage(QSize value);

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    Ui::USingleImageWidget *ui;

    int column;
    int row;
    /*int calcChannel;
    QString componentName;
    QString componentPropertyName;
    bool showLegend;
    bool showChannels;

    ///–азмер отображаемого изображени€
    ///
    /// 0 - ѕолноразмерное отображение
    /// 1 - раст€нуть, сохран€€ пропорции
    /// 2 - раст€нуть на всю область
    int sizeMode;


    bool connected;

    QImage srcImage;
    QImage transformedImage;
    */

    bool selected;

    //buffers
    RDK::UBitmap tempBmp;
    RDK::UBitmapParam bmp_param;

    QThread *thread;
    UImageLoader *imageLoader;
    USingleImagePainter *painter;
};

#endif // USINGLEIMAGEWIDGET_H
