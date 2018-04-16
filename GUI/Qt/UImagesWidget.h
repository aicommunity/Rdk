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

    /// Создает @param numChannels окошек и ставит для них источником компонент @param componentLongName
    void setCaptureForChannels(int numChannels, QString componentLongName, QString propertyName);

    /// Выбирает изображение номер @param id
    void selectImage(int id);

    /// Запрещает/разрешает изменения виджета, такие как добавление/удаление столбцов и переключение IndChannels
    void setEnableChanges(bool value);

    // ----- Проброс в UImagePainter -----
    /// Устанавливает набор полигонов на выбранное изображение (selectedImage)
    void setZones(const QList<UDrawablePolygon> &polygons, int imageNum = -1);

    /// Устанавливает QPen в выделенное изображение (selectedImage)
    void setImagePen(const QPen &value);

    /// Устанавливает режим рисования для текущего изображения (selectedImage)
    void setDrawable(bool value);

    /// устанавливает зону как выбранную в выделеноне изображение (selectedImage)
    void selectZone(int id);

    /// запись файла настроек
    virtual void ASaveParameters();
    /// считывание файла настроек
    virtual void ALoadParameters();
signals:
    /// Сигнал отправляет канал текущего выбранного изображения, при смене selectedImage
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

    /// Отображение легенды для всех виджетов
    void setShowLegend(bool b);
    /// Отображать изображение для всех виджетов, используя его индивидуальный канал
    void setIndChannels(bool b);

    // images size policy
    void setOriginalSize();
    void setTiledSize();
    void setPropSize();

    void selectImage(USingleImageWidget *item);
    void showFullScreenImage(USingleImageWidget *item);
    void updateImages();

    /// Слот принимающий от USingleImageWidget сигнал о том что полигон нарисован
    //void drawFinished(QPolygonF poly, QSize imgSize);

private:
    Ui::UImagesWidget *ui;

    //buffers
    RDK::UBitmap tempBmp;
    RDK::UBitmapParam bmp_param;

    /// добавление элемента
    USingleImageWidget *addSingleItem(int row, int column);

    /// Удаляет все текущие виджеты
    void clearImagesWidget();

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

    /// флаг, разрешающий изменения сетки изображений
    bool enableChanges;

    /// Флаг режима отображения только одного окна на всё доступное место
    bool singleImageMode;

    /// Счётчик столбцов
    int columnsCounter;
    /// Счетчик строк
    int rowsCounter;
};

#endif // UIMAGESWIDGET_H
