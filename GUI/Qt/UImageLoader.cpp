#include "UImageLoader.h"

#include <QMutexLocker>

UImageLoader::UImageLoader(QObject *parent, int channel, bool showLegend, bool indChannels, int imagesSizeMod) :
  QObject(parent),
  calcChannel(channel),
  sizeMode(imagesSizeMod),
  showLegend(showLegend),
  indChannels(indChannels),
  mutex(QMutex::Recursive),
  connected(false)
{

}

UImageLoader::~UImageLoader()
{

}

void UImageLoader::loadImage(QSize value)
{
    QMutexLocker locker(&mutex);
    if (!connected)
        return;
    int channel = indChannels?calcChannel:Core_GetSelectedChannelIndex();

    int copy_res = MModel_CopyComponentBitmapOutputHeader(
                      channel,
                      componentName.toLocal8Bit(),
                      componentPropertyName.toLocal8Bit(), &bmp_param);

    if(copy_res == 0)
    {
        tempBmp.SetRes(bmp_param.Width, bmp_param.Height, bmp_param.ColorModel);
        MModel_CopyComponentBitmapOutput(
                    channel,
                    componentName.toLocal8Bit(),
                    componentPropertyName.toLocal8Bit(), &tempBmp);
        srcImage = fromUBitmap(&tempBmp);
    }

    resizeImage(value);
}

void UImageLoader::resizeImage(QSize value)
{
    QMutexLocker locker(&mutex);
    if(!connected || srcImage.isNull())
    {
        emit imageLoaded(NULL);
        return;
    }

    int legendSpace = showLegend?28:8;
    switch (sizeMode)
    {
    case 0:
        emit imageLoaded(&srcImage);
        break;

    case 1:
        transformedImage = srcImage.scaled(value.width()-8, value.height()-legendSpace, Qt::KeepAspectRatio);
        emit imageLoaded(&transformedImage);
        break;

    case 2:
        transformedImage = srcImage.scaled(value.width()-8, value.height()-legendSpace, Qt::IgnoreAspectRatio);
        emit imageLoaded(&transformedImage);
        break;
    }
}

QImage UImageLoader::getSrcImage() const
{
    return srcImage;
}

int UImageLoader::getSizeMode() const
{
    return sizeMode;
}

void UImageLoader::setSizeMode(int value)
{
    sizeMode = value;
}

bool UImageLoader::getIndChannels() const
{
    return indChannels;
}

void UImageLoader::setIndChannels(bool value)
{
    indChannels = value;
}

bool UImageLoader::getShowLegend() const
{
    return showLegend;
}

void UImageLoader::setShowLegend(bool value)
{
    showLegend = value;
}

int UImageLoader::getCalcChannel() const
{
    return calcChannel;
}

void UImageLoader::setCalcChannel(int value)
{
    calcChannel = value;
}

QString UImageLoader::getComponentPropertyName() const
{
    return componentPropertyName;
}

void UImageLoader::setComponentPropertyName(const QString &value)
{
    componentPropertyName = value;
    if(!value.isEmpty())
        connected = true;
    else
        connected = false;
}

QString UImageLoader::getComponentName() const
{
    return componentName;
}

void UImageLoader::setComponentName(const QString &value)
{
    componentName = value;
}

QImage UImageLoader::fromUBitmap(RDK::UBitmap *tempBmp)
{
    switch(tempBmp->GetColorModel())
    {
    case RDK::ubmRGB24:
        {
            return QImage((const uchar *) tempBmp->GetData(), tempBmp->GetWidth(),
                          tempBmp->GetHeight(), tempBmp->GetLineByteLength(), QImage::Format_RGB888).rgbSwapped();
        }

    case RDK::ubmRGB32:
        {
            return QImage((const uchar *) tempBmp->GetData(), tempBmp->GetWidth(),
                         tempBmp->GetHeight(), tempBmp->GetLineByteLength(), QImage::Format_RGB32).rgbSwapped();
        }

    case RDK::ubmRGB96:
        {
            tempBmp->SetColorModel(RDK::ubmRGB24, true);
            return QImage((const uchar *) tempBmp->GetData(), tempBmp->GetWidth(),
                          tempBmp->GetHeight(), tempBmp->GetLineByteLength(), QImage::Format_RGB32).rgbSwapped();
        }

    case RDK::ubmY8:
        {
            return QImage((const uchar *) tempBmp->GetData(), tempBmp->GetWidth(),
                          tempBmp->GetHeight(), tempBmp->GetLineByteLength(), QImage::Format_Indexed8).convertToFormat(QImage::Format_RGB888);
        }

    case RDK::ubmY32:
        {
            tempBmp->SetColorModel(RDK::ubmY8, true);
            return QImage((const uchar *) tempBmp->GetData(), tempBmp->GetWidth(),
                          tempBmp->GetHeight(), tempBmp->GetLineByteLength(), QImage::Format_Indexed8).convertToFormat(QImage::Format_RGB888);
        }
    }

    return QImage();
}



QMutex *UImageLoader::getMutex()
{
    return &mutex;
}

bool UImageLoader::getConnected() const
{
    return connected;
}
