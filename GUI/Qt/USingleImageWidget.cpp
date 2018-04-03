#include "USingleImageWidget.h"
#include "ui_USingleImageWidget.h"

#include <QImage>
#include <QMutex>

USingleImageWidget::USingleImageWidget(QWidget *parent, int row, int column, int channel, bool showLegend, bool indChannels, int imagesSizeMod) :
    UVisualControllerWidget(parent),
    column(column),
    row(row),
    /*calcChannel(channel),
    sizeMode(imagesSizeMod),
    selected(false),
    connected(false),*/
    ui(new Ui::USingleImageWidget)
{
    ui->setupUi(this);
    painter = new USingleImagePainter(this);
    ui->scrollArea->setWidget(painter);

    imageLoader = new UImageLoader(NULL, channel, showLegend, indChannels, imagesSizeMod);
    painter->setLoaderMutex(imageLoader->getMutex());
    thread = new QThread(this);
    imageLoader->moveToThread(thread);
    //connect(thread, SIGNAL(started()), imageLoader, SLOT(process()));
    connect(imageLoader, SIGNAL(imageLoaded(QImage*))          , painter    , SLOT(setImage(QImage*)));
    connect(this       , SIGNAL(loadImage(QSize))              , imageLoader, SLOT(loadImage(QSize)));
    connect(this       , SIGNAL(resizeImage(QSize))            , imageLoader, SLOT(resizeImage(QSize)));
    connect(painter    , SIGNAL(setedImageSize(QSize))         , this       , SLOT(setImageSizeInfo(QSize)));
    connect(painter    , SIGNAL(zoneFinished(QPolygonF, QSize)), this       , SIGNAL(zoneFinished(QPolygonF, QSize)));


    UpdateInterval = 30;
    setAccessibleName("USingleImageWidget"+QString::number(row)+"x"+QString::number(column));

    setShowLegend(showLegend);
    setShowChannels(indChannels);
    //reDrawWidget();
}

USingleImageWidget::~USingleImageWidget()
{
    thread->quit();
    thread->wait(3000);

    delete imageLoader;
    delete ui;
}

void USingleImageWidget::AUpdateInterface()
{
    /*int channel = showChannels?calcChannel:Core_GetSelectedChannelIndex();

    int copy_res=MModel_CopyComponentBitmapOutputHeader(
                    channel,
                    componentName.toLocal8Bit(),
                    componentPropertyName.toLocal8Bit(), &bmp_param);

    if(copy_res == 0)
    {
        tempBmp.SetRes(bmp_param.Width, bmp_param.Height, bmp_param.ColorModel);
        MModel_CopyComponentBitmapOutput(
                    channel,
                    componentName.toLocal8Bit(),
                    componentPropertyName.toLocal8Bit(), &bmp_param);
        (*i)->setImage(fromUBitmap(&tempBmp));
    }
    */
    emit loadImage(size());
}

bool USingleImageWidget::getShowLegend() const
{
    return imageLoader->getShowLegend();
}

int USingleImageWidget::getSizeMode() const
{
    return imageLoader->getSizeMode();
}

void USingleImageWidget::setShowLegend(bool value)
{
    imageLoader->setShowLegend(value);
    if(value)
        ui->legend_frame->show();
    else
      ui->legend_frame->hide();
}

void USingleImageWidget::setImageSizeInfo(QSize size)
{
  ui->labelInfo->setText(QString::number(size.width())+"x"+QString::number(size.height()));
}

void USingleImageWidget::reDrawWidget()
{
    /*if(!connected || srcImage.isNull())
    {
        //ui->labelImage->setText("no image");
        painter->setImage(NULL);
        return;
    }

    int legendSpace = showLegend?28:8;
    switch (sizeMode)
    {
    case 0:
        painter->setImage(&srcImage);
        break;

    case 1:
        transformedImage = srcImage.scaled(size().width()-8, size().height()-legendSpace, Qt::KeepAspectRatio);
        painter->setImage(&transformedImage);
        break;

    case 2:
        transformedImage = srcImage.scaled(size().width()-8, size().height()-legendSpace, Qt::IgnoreAspectRatio);
        painter->setImage(&transformedImage);
        break;
    }*/
    emit resizeImage(size());
}

void USingleImageWidget::setSize(int value)
{
    //sizeMode = value;
    imageLoader->setSizeMode(value);
}

void USingleImageWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    emit resizeImage(size());
}

void USingleImageWidget::setShowChannels(bool value)
{
    //showChannels = value;
    imageLoader->setIndChannels(value);
    if(value)
        ui->labelLegend->setText(QString::number(imageLoader->getCalcChannel()) +" chan. "+imageLoader->getComponentName()+"["+imageLoader->getComponentPropertyName()+"]");
    else
      ui->labelLegend->setText(imageLoader->getComponentName()+"["+imageLoader->getComponentPropertyName()+"]");
}

void USingleImageWidget::setZones(QList<UDrawablePolygon> polygons)
{
  painter->setZones(polygons);
}

void USingleImageWidget::setPainterPen(const QPen &value)
{
  painter->setPen(value);
}

void USingleImageWidget::setDrawable(bool value)
{
  painter->setDrawable(value);
}

void USingleImageWidget::selectZone(int id)
{
  painter->selectZone(id);
}

int USingleImageWidget::getCalcChannel() const
{
    //return calcChannel;
    return imageLoader->getCalcChannel();
}

void USingleImageWidget::setCalcChannel(int value)
{
    //calcChannel = value;
    imageLoader->setCalcChannel(value);
    if(imageLoader->getIndChannels())
        ui->labelLegend->setText(QString::number(imageLoader->getCalcChannel()) +" chan. "+imageLoader->getComponentName()+"["+imageLoader->getComponentPropertyName()+"]");
    else
      ui->labelLegend->setText(imageLoader->getComponentName()+"["+imageLoader->getComponentPropertyName()+"]");
}

bool USingleImageWidget::getConnected() const
{
    //return connected;
    return imageLoader->getConnected();
}

QString USingleImageWidget::getComponentName() const
{
    //return componentName;
    return imageLoader->getComponentName();
}

void USingleImageWidget::setComponentName(const QString &value)
{
    //componentName = value;
    imageLoader->setComponentName(value);
}

QString USingleImageWidget::getComponentPropertyName() const
{
//    return componentPropertyName;
    return imageLoader->getComponentPropertyName();
}

void USingleImageWidget::setComponentPropertyName(const QString &value)
{
    //componentPropertyName = value;
    imageLoader->setComponentPropertyName(value);
    thread->start();
    if(imageLoader->getIndChannels())
        ui->labelLegend->setText(QString::number(imageLoader->getCalcChannel()) +" chan. "+imageLoader->getComponentName()+"["+imageLoader->getComponentPropertyName()+"]");
    else
        ui->labelLegend->setText(imageLoader->getComponentName()+"["+imageLoader->getComponentPropertyName()+"]");
    //connected = true;
}

bool USingleImageWidget::getSelected() const
{
    return selected;
}

void USingleImageWidget::setSelected(bool value)
{
    if(value == selected) return;
    selected = value;
    if(selected)
        ui->frameSelectionBorder->setFrameShadow(QFrame::Plain);
    else
    {
        ui->frameSelectionBorder->setFrameShadow(QFrame::Raised);
        painter->selectZone(-1);
    }
}

void USingleImageWidget::mousePressEvent(QMouseEvent *)
{
    emit selectionSignal(this);
}

void USingleImageWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    emit fullScreenSignal(this);
}

QImage USingleImageWidget::getImage() const
{
    return imageLoader->getSrcImage();
}

/*void USingleImageWidget::setImage(const QImage &value)
{
    srcImage = value;
    ui->labelInfo->setText(QString::number(srcImage.size().width())+"x"+QString::number(srcImage.size().height()));
}*/

int USingleImageWidget::getRow() const
{
    return row;
}

void USingleImageWidget::setRow(int value)
{
    row = value;
}

int USingleImageWidget::getColumn() const
{
    return column;
}

void USingleImageWidget::setColumn(int value)
{
    column = value;
}
