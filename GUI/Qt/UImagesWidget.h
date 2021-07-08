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
    explicit UImagesWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UImagesWidget();
    void AUpdateInterface();

    /// ������� @param numChannels ������ � ������ ��� ��� ���������� ��������� @param componentLongName
    void setCaptureForChannels(int numChannels, QString componentLongName, QString propertyName);

    /// �������� ����������� ����� @param id
    void selectImage(int id);

    /// ���������/��������� ��������� �������, ����� ��� ����������/�������� �������� � ������������ IndChannels
    void setEnableChanges(bool value);

    /// ������ ��������� ������� ��� �������� ���� - ����� ������, ��� ����������, ��������
    void selectCapture(const int channelNum, QString componentLongName, QString propertyName);

    // ----- ������� � UImagePainter -----
    /// ������������� ����� ��������� �� ��������� ����������� (selectedImage)
    void setPolygons(const QList<UDrawablePolygon> &polygons, int imageNum = -1);

    /// ������������� QPen � ���������� ����������� (selectedImage)
    void setImagePen(const QPen &value, int imageNum = -1);

    /// ������������� ����� ��������� ��� �������� ����������� (selectedImage)
    void setDrawable(bool value, int imageNum = -1);

    /// ������������� ���� ��� ��������� � ��������� ����������� (selectedImage)
    void selectPolygon(int id, int imageNum = -1);

    /// ������������� ����� ��������� ���� ��������������� ����� � ������ �������� ����
    void setDrawRects(bool value, int imageNum = -1);

    /// ������������� ��� �������������� � ������� rectangles
    void setRectangles(const QPair<QRectF, QRectF> &rects, int imageNum = -1);

    ///��������� ���������� ������������ ���� ��� ������ ������� (� ����������� ������ ���� ������� ������ ����)
    /// � ����� ��������� ����������� ���� ��� ���������� �������
    void PreventContextMenu();

    ///��������� ����������� ���� �
    void BlockContextMenu();

    ///��������� ��������� ������ �����������
    int GetImageWidth(int imageNum=-1);
    ///��������� ��������� ������ �����������
    int GetImageHeight(int imageNum=-1);

    ///������ ���������� ������ � ���� @param layoutW �� @param layoutH
    void SetImageLayout(int layoutW, int layoutH);

    ///������ �������� isChekBoxIndChannel
    ///true - ����� ����������
    ///false - �� ����� ����������
    void SetIsChekBoxIndChannel(bool isIt);

    ///���������� ������ �� ����������� ����
    //QMenu *getContextMenu();

    // ------------------------------

    /// ������ ����� ��������
    virtual void ASaveParameters(RDK::USerStorageXML &xml);
    /// ���������� ����� ��������
    virtual void ALoadParameters(RDK::USerStorageXML &xml);

    // ������� ���������� � �������� ���������
    virtual void AClearInterface(void);

signals:
    /// ������ ���������� ����� �������� ���������� �����������, ��� ����� selectedImage
    void selectedImageChannel(int channel);

    // signals from USingleImagePainter
    void polygonFinished(QPolygonF, QSize);
    void polygonModified(UDrawablePolygon, QSize);
    void polygonSelected(int);
    void rectanglesChanged(QPair<QRectF, QRectF>);

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

    ///���������� ����, ����� �� ���� ������� check box index chanel
    ///���� ��� - ������ �� ������ �������
    bool isChekBoxIndChannel;

};

#endif // UIMAGESWIDGET_H
