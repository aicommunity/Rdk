#ifndef UIMAGELOADER_H
#define UIMAGELOADER_H

#include <QObject>
#include <QWaitCondition>
#include <QImage>
#include <QMutex>

#include <rdk_application.h>

class UImageLoader : public QObject
{
    Q_OBJECT
public:
    UImageLoader(QObject *parent = 0, int channel = 0, bool showLegend = false, bool indChannels = false, int imagesSizeMod = 0);
    virtual ~UImageLoader();

    QString getComponentName() const;
    void setComponentName(const QString &value);

    QString getComponentPropertyName() const;
    void setComponentPropertyName(const QString &value);

    int getCalcChannel() const;
    void setCalcChannel(int value);

    bool getShowLegend() const;
    void setShowLegend(bool value);

    bool getIndChannels() const;
    void setIndChannels(bool value);

    int getSizeMode() const;
    void setSizeMode(int value);

    int getImageWidth();
    int getImageHeight();

    QImage getSrcImage() const;

    bool getConnected() const;

    QMutex *getMutex();

signals:
    void imageLoaded(QImage *image);
public slots:
    void loadImage(QSize value);
    void resizeImage(QSize value);

private:
    //methods
    QImage fromUBitmap(RDK::UBitmap *tempBmp);


    //data
    QMutex mutex;

    QString componentName;
    QString componentPropertyName;
    bool connected;
    int calcChannel;
    bool showLegend;
    bool indChannels;

    ///Размер отображаемого изображения
    ///
    /// 0 - Полноразмерное отображение
    /// 1 - растянуть, сохраняя пропорции
    /// 2 - растянуть на всю область
    int sizeMode;

    QImage srcImage;
    QImage transformedImage;

    //buffers
    RDK::UBitmap tempBmp;
    RDK::UBitmapParam bmp_param;
};

#endif // UIMAGELOADER_H
