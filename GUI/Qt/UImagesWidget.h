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
    explicit UImagesWidget(QWidget *parent = 0, RDK::UApplication *app = NULL, QString settingsFile = "settings.qt", QString settingsGroup = "UImagesWidget");
    virtual ~UImagesWidget();
    void AUpdateInterface();

    /// ������� @param numChannels ������ � ������ ��� ��� ���������� ��������� @param componentLongName
    void setCaptureForChannels(int numChannels, QString componentLongName, QString propertyName);

    /// �������� ����������� ����� @param id
    void selectImage(int id);

    /// ���������/��������� ��������� �������, ����� ��� ����������/�������� �������� � ������������ IndChannels
    void setEnableChanges(bool value);

    // ----- ������� � UImagePainter -----
    /// ������������� ����� ��������� �� ��������� ����������� (selectedImage)
    void setZones(const QList<UDrawablePolygon> &polygons, int imageNum = -1);

    /// ������������� QPen � ���������� ����������� (selectedImage)
    void setImagePen(const QPen &value);

    /// ������������� ����� ��������� ��� �������� ����������� (selectedImage)
    void setDrawable(bool value);

    /// ������������� ���� ��� ��������� � ���������� ����������� (selectedImage)
    void selectZone(int id);

    /// ������ ����� ��������
    virtual void ASaveParameters();
    /// ���������� ����� ��������
    virtual void ALoadParameters();
signals:
    /// ������ ���������� ����� �������� ���������� �����������, ��� ����� selectedImage
    void selectedImageChannel(int channel);

    // signals from USingleImagePainter
    void zoneFinished(QPolygonF, QSize);
    void zoneModified(UDrawablePolygon, QSize);
    void zoneSelected(int);

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

    /// ����������� ������� ��� ���� ��������
    void setShowLegend(bool b);
    /// ���������� ����������� ��� ���� ��������, ��������� ��� �������������� �����
    void setIndChannels(bool b);

    // images size policy
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

    /// ����, ����������� ��������� ����� �����������
    bool enableChanges;

    /// ���� ������ ����������� ������ ������ ���� �� �� ��������� �����
    bool singleImageMode;

    /// ������� ��������
    int columnsCounter;
    /// ������� �����
    int rowsCounter;
};

#endif // UIMAGESWIDGET_H
