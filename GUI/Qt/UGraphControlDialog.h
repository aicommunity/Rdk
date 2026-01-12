#ifndef UGRAPHCONTROLDIALOG_H
#define UGRAPHCONTROLDIALOG_H

#include <QDialog>
#include <QColor>
#include <QString>

class UGraphPaintWidget;

namespace Ui {
class UGraphControlDialog;
}

class UGraphControlDialog : public QDialog
{
    Q_OBJECT

public:
    ///Конструктор окна настройки
    explicit UGraphControlDialog(QWidget *parent = 0, UGraphPaintWidget *graphPainter = nullptr);

    ///Деструктор окна настройки
    ~UGraphControlDialog();



    //**********************************************************Выдача изменяемых параметров
    /// Выдать диапазон оси Х, за которым будет происходить слежение
    double getDisplayInterval() const;

    /// Выдать решение об изменениии границы по оси Х
    int getAxisXChangeData(void) const;

    /// Выдать решение об изменениии границы по оси У вверх
    int getAxisMaxYChangeData(void) const;

    /// Выдать решение об изменениии границы по оси У вниз
    int getAxisMinYChangeData(void) const;

    /// Выдать выбранный цвет (индекс в массиве)
    int getColorIndex(void) const;

    /// Выдать выбранный цвет (QColor)
    QColor getColor(void) const;

    /// Выдать подпись по оси Х
    QString getLabelX(void) const;

    /// Выдать подпись по оси У
    QString getLabelY(void) const;

    /// Выдать индекс выбранного графика
    int getSelectedGraphIndex() const;

    /// Выдать новое имя графика (если было изменено)
    QString getGraphName() const;

    /// Было ли изменено имя графика
    bool isGraphNameChanged() const;

    /// Было ли изменено имя графика
    bool isGraphDataSourceChanged() const;

    /// Выдать новый источник данных (если был изменен)
    void getGraphDataSource(int &channelIndex, QString &componentName, QString &propertyName, int &jx, int &jy) const;

    /// Выдать решение об удалении текущего графика
    bool shouldDeleteGraph(void) const;

    /// Установить флаг удаления графика
    void setDeleteRequested(bool requested);
    //**********************************************************


    //**********************************************************Установка исходных параметров окна
    /// Установить виджет графика для получения информации
    void setGraphPainter(UGraphPaintWidget *painter);

    /// Установить наличие изменениий границ по оси Х
    void setAxisXChangeData(int axisX);

    ///Установить наличие изменениий границ по оси У вверх
    void setAxisMaxYChangeData(int axisY);

    ///Установить наличие изменениий границ по оси У вниз
    void setAxisMinYChangeData(int axisY);

    /// Установить количество знаков после запятой
    /// у диапазона оси Х, за которым происходит слежение
    void setDecimalPlaces (int i);

    ///Установить диапазон оси Х, за которым происходит слежение
    void setDisplayInterval (double value);

    /// Установить подпись по оси Х
    void setLabelX (const QString &labelX);

    /// Установить подпись по оси У
    void setLabelY (const QString &labelY);

    /// Обновить список графиков и выбрать текущий
    void refreshGraphsList(int currentGraphIndex = -1);
    //**********************************************************

private slots:
    /// Если нажали на кнопку удаления текущего графика
    void onDeleteGraphClicked();

    /// Должно ли быть активным окно с диапазоном слежения по оси Х
    void onAxisXToggled(bool enabled);

    /// Если нажали изменить цвет - нужно изменить цвет
    void onColorSelectionChanged(int index);

    /// При выборе графика из списка
    void onGraphSelectionChanged(int index);

    /// При изменении имени графика
    void onGraphNameChanged();

    /// При нажатии кнопки изменения источника данных
    void onChangeDataSourceClicked();

    /// Обновить информацию о выбранном графике
    void updateGraphInfo(int graphIndex);

private:
    Ui::UGraphControlDialog *ui;
    UGraphPaintWidget *graphPainter;
    int selectedGraphIndex;
    bool graphNameChanged;
    bool graphDataSourceChanged;
    QString newGraphName;
    int newChannelIndex;
    QString newComponentName;
    QString newPropertyName;
    int newJx, newJy;
    bool deleteRequested;

    /// Инициализировать список цветов
    void initializeColorList();

    /// Получить цвет по индексу
    QColor getColorByIndex(int index) const;

    /// Получить индекс цвета
    int getColorIndex(const QColor &color) const;
};

#endif // UGRAPHCONTROLDIALOG_H
