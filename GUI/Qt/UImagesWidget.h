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

    /// ������������� ����� ��������� �� ��������� �����������
    void setZones(QList<QPair<QPolygonF, QPen> > polygons);

    /// ������������� QPen � ���������� ����������� (selectedImage)
    void setImagePen(const QPen &value);

    /// ������� @param numChannels ������ � ������ ��� ��� ���������� ��������� @param componentLongName
    void setCaptureForChannels(int numChannels, QString componentLongName, QString propertyName);

    /// �������� ����������� ����� @param id
    void selectImage(int id);

    /// ������������� ����� ��������� ��� �������� �����������
    void setDrawable(bool value);

public slots:
    ///���������� ������ ��������
    void readSettings(QString file, QString group = "UImagesWidget");
    ///������ ������ ��������
    void writeSettings(QString file, QString group = "UImagesWidget");

signals:
    /// ������ ���������� ����� �������� ���������� �����������, ��� ����� selectedImage
    void selectedImageChannel(int channel);

    void zoneFinished(QPolygonF, QSize);

private slots:
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

    /// ���� ����������� �� USingleImageWidget ������ � ��� ��� ������� ���������
    //void drawFinished(QPolygonF poly, QSize imgSize);

private:
    Ui::UImagesWidget *ui;

    QString settingsFileName;

    //buffers
    RDK::UBitmap tempBmp;
    RDK::UBitmapParam bmp_param;

    /// ���������� ��������
    USingleImageWidget *addSingleItem(int row, int column);

    /// ������� ��� ������� �������
    void clearImagesWidget();

    /// ����������� ����
    QMenu *contextMenu;

    /// ������ �� ������ ����������� ������
    QList<USingleImageWidget *> imagesList;

    /// ��������� ������
    USingleImageWidget *selectedImage;

    ///������ ������������� �����������
    ///
    /// 0 - �������������� �����������
    /// 1 - ���������, �������� ���������
    /// 2 - ��������� �� ��� �������
    int imagesSizeMod;

    bool showLegend;
    bool indChannels;

    /// ���� ������ ����������� ������ ������ ���� �� �� ��������� �����
    bool singleImageMode;
    int columnsCounter;
    int rowsCounter;
};

#endif // UIMAGESWIDGET_H
