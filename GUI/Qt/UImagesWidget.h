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
    QImage fromUBitmap(RDK::UBitmap *tempBmp);

public slots:
    ///считывание файлов настроек
    void readSettings(QString file, QString group = "UImagesWidget");
    ///запись файлов настроек
    void writeSettings(QString file, QString group = "UImagesWidget");

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

private:
    Ui::UImagesWidget *ui;

    QString settingsFileName;

    //buffers
    RDK::UBitmap tempBmp;
    RDK::UBitmapParam bmp_param;

    /// добавление элемента
    USingleImageWidget *addSingleItem(int row, int column);

    /// контекстное меню
    QMenu *contextMenu;

    /// вектор из окошек отображения входов
    QList<USingleImageWidget *> imagesList;

    /// выбранное окошко
    USingleImageWidget *selectedImage;

    ///Размер отображаемого изображения
    ///
    /// 0 - Полноразмерное отображение
    /// 1 - растянуть, сохраняя пропорции
    /// 2 - растянуть на всю область
    int imagesSizeMod;

    bool showLegend;
    bool indChannels;

    /// Флаг режима отображения только одного окна на всё доступное место
    bool singleImageMode;
    int columnsCounter;
    int rowsCounter;
};

#endif // UIMAGESWIDGET_H
