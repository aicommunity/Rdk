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

    /// Создает @param numChannels окошек и ставит для них источником компонент @param componentLongName
    void setCaptureForChannels(int numChannels, QString componentLongName, QString propertyName);

    /// Выбирает изображение номер @param id
    void selectImage(int id);

    /// Запрещает/разрешает изменения виджета, такие как добавление/удаление столбцов и переключение IndChannels
    void setEnableChanges(bool value);

    /// Задать параметры захвата для текущего окна - номер канала, имя компонента, свойство
    void selectCapture(const int channelNum, QString componentLongName, QString propertyName);

    // ----- Проброс в UImagePainter -----
    /// Устанавливает набор полигонов на выбранное изображение (selectedImage)
    void setPolygons(const QList<UDrawablePolygon> &polygons, int imageNum = -1);

    /// Устанавливает QPen в выделенное изображение (selectedImage)
    void setImagePen(const QPen &value, int imageNum = -1);

    /// Устанавливает режим рисования для текущего изображения (selectedImage)
    void setDrawable(bool value, int imageNum = -1);

    /// устанавливает зону как выбранную в выделеное изображение (selectedImage)
    void selectPolygon(int id, int imageNum = -1);

    /// Устанавливает режим рисования двух прямоугольников левой и правой кнопками мыши
    void setDrawRects(bool value, int imageNum = -1);

    /// Устанавливает два прямоугольника в текущий rectangles
    void setRectangles(const QPair<QRectF, QRectF> &rects, int imageNum = -1);

    ///Отключает функционал контекстного меню для самого виджета (с сохранением приема всех нажатий клавиш мыши)
    /// а также отключает контекстное меню для внутренних классов
    void PreventContextMenu();

    ///Отключает контекстное меню у
    void BlockContextMenu();

    ///Извлекает настоящую ширину изображения
    int GetImageWidth(int imageNum=-1);
    ///Извлекает настоящую высоту изображения
    int GetImageHeight(int imageNum=-1);

    ///Задает размещение окошек в окне @param layoutW на @param layoutH
    void SetImageLayout(int layoutW, int layoutH);

    ///задает значение isChekBoxIndChannel
    ///true - может изменяться
    ///false - не может изменяться
    void SetIsChekBoxIndChannel(bool isIt);

    ///Возвращает ссылку на контекстное меню
    //QMenu *getContextMenu();

    // ------------------------------

    /// запись файла настроек
    virtual void ASaveParameters(RDK::USerStorageXML &xml);
    /// считывание файла настроек
    virtual void ALoadParameters(RDK::USerStorageXML &xml);

    // Возврат интерфейса в исходное состояние
    virtual void AClearInterface(void);

signals:
    /// Сигнал отправляет канал текущего выбранного изображения, при смене selectedImage
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

    ///показатель того, может ли быть активен check box index chanel
    ///если нет - значит он всегда активен
    bool isChekBoxIndChannel;

};

#endif // UIMAGESWIDGET_H
