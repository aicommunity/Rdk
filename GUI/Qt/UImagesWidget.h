#ifndef UIMAGESWIDGET_H
#define UIMAGESWIDGET_H

#include <UVisualControllerWidget.h>
#include <QMenu>
#include <USingleImageWidget.h>

namespace Ui {
class UImagesWidget;
}

class UImagesWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UImagesWidget(QWidget *parent = 0, QString settingsFile = "settings.qt", QString settingsGroup = "UImagesWidget");
    virtual ~UImagesWidget();
    void AUpdateInterface();
    QPixmap fromUBitmap(RDK::UBitmap *tempBmp);

public slots:
    ///���������� ������ ��������
    void readSettings(QString file, QString group = "UImagesWidget");
    ///������ ������ ��������
    void writeSettings(QString file, QString group);

    void actionSaveToBMP();
    void actionSaveToJPEG();
    void actionSaveAllToBMP();
    void actionSaveAllToJPEG();
    void actionSelectSource();
    void actionAddColumn();
    void actionAddRow();
    void actionDeleteColumn();
    void actionDeleteRow();

    void setShowLegend(bool b);
    void setIndChannels(bool b);
    void setOriginalSize();
    void setTiledSize();
    void setPropSize();

    void selectImage(USingleImageWidget *item);
    void showFullScreenImage(USingleImageWidget *item);
    void updateImages();

protected:
    //buffers
    RDK::UBitmap tempBmp;
    RDK::UBitmapParam bmp_param;


private:
    Ui::UImagesWidget *ui;

    QString settingsFileName;
    QString settingsGroupName;

    /// ���������� ��������
    USingleImageWidget *addSingleItem(int row, int column);

    /// ����������� ����
    QMenu *contextMenu;

    /// ������ �� ������ ����������� ������
    QList<USingleImageWidget *> imagesList;

    /// ��������� ������
    USingleImageWidget *selectedImage;

    int imagesSizeMod;
    bool showLegend;
    bool indChannels;
    bool singleImageMode;
    int columnsCounter;
    int rowsCounter;
};

#endif // UIMAGESWIDGET_H
