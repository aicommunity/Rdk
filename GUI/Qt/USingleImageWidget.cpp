#include "USingleImageWidget.h"
#include "ui_USingleImageWidget.h"

#include <QImage>
//#include <QFile>

USingleImageWidget::USingleImageWidget(QWidget *parent, int row, int column, int channel, bool showLegend, bool indChannels, int imagesSizeMod) :
    QWidget(parent),
    column(column),
    row(row),
    calcChannel(channel),
    sizeMode(imagesSizeMod),
    selected(false),
    connected(false),
    ui(new Ui::USingleImageWidget)
{
    ui->setupUi(this);
    painter = new USingleImagePainter(this);
    painter->setSizeMode(sizeMode);
    ui->scrollArea->setWidget(painter);

    setShowLegend(showLegend);
    setShowChannels(indChannels);
    reDrawWidget();
}

USingleImageWidget::~USingleImageWidget()
{
    delete ui;
}

bool USingleImageWidget::getShowLegend() const
{
    return showLegend;
}

int USingleImageWidget::getSizeMode() const
{
    return sizeMode;
}

void USingleImageWidget::setShowLegend(bool value)
{
    showLegend = value;
    if(showLegend)
        ui->frame->show();
    else
        ui->frame->hide();
}

void USingleImageWidget::reDrawWidget()
{
    if(!connected || srcImage.isNull())
    {
        //ui->labelImage->setText("no image");
        painter->setImage(NULL);
        return;
    }
    //painter->setImage(&srcImage);
    int legendSpace = showLegend?28:8;
    switch (sizeMode)
    {
    case 0:
        painter->setImage(&srcImage);
        break;

    case 1:
        //painter->setImage(&transformedImage);
        transformedImage = srcImage.scaled(size().width()-8, size().height()-legendSpace, Qt::KeepAspectRatio);
        painter->setImage(&transformedImage);
        break;

    case 2:
        transformedImage = srcImage.scaled(size().width()-8, size().height()-legendSpace, Qt::IgnoreAspectRatio);
        painter->setImage(&transformedImage);
        break;
    }
}

void USingleImageWidget::setSize(int value)
{
    sizeMode = value;
    painter->setSizeMode(value);
}

void USingleImageWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    reDrawWidget();
}

void USingleImageWidget::setShowChannels(bool value)
{
    showChannels = value;
    if(showChannels)
        ui->labelLegend->setText(QString::number(calcChannel) +" chan. "+componentName+"["+componentPropertyName+"]");
    else
        ui->labelLegend->setText(componentName+"["+componentPropertyName+"]");
}

int USingleImageWidget::getCalcChannel() const
{
    return calcChannel;
}

void USingleImageWidget::setCalcChannel(int value)
{
    calcChannel = value;
}

bool USingleImageWidget::getConnected() const
{
    return connected;
}

QString USingleImageWidget::getComponentName() const
{
    return componentName;
}

void USingleImageWidget::setComponentName(const QString &value)
{
    componentName = value;
}

QString USingleImageWidget::getComponentPropertyName() const
{
    return componentPropertyName;
}

void USingleImageWidget::setComponentPropertyName(const QString &value)
{
    componentPropertyName = value;
    if(showChannels)
        ui->labelLegend->setText(QString::number(calcChannel) +" chan. "+componentName+"["+componentPropertyName+"]");
    else
        ui->labelLegend->setText(componentName+"["+componentPropertyName+"]");
    connected = true;
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
        ui->frameSelectionBorder->setFrameShadow(QFrame::Raised);
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
    return srcImage;
}

void USingleImageWidget::setImage(const QImage &value)
{
    srcImage = value;
    ui->labelInfo->setText(QString::number(srcImage.size().width())+"x"+QString::number(srcImage.size().height()));
}

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
