#ifndef UGRAPHCONTROLDIALOG_H
#define UGRAPHCONTROLDIALOG_H

#include <QDialog>

namespace Ui {
class UGraphControlDialog;
}

class UGraphControlDialog : public QDialog
{
    Q_OBJECT

public:

    /// Признак изменения цвета
    /// Если цвет изменился в настройках, то индекс в массиве
    /// Если нет, то -1
    int color;

    /// Нажималась ли кнопка удаления графика
    /// Если нет false
    /// Если да true
    bool click;

    ///Конструктор окна настройки
    explicit UGraphControlDialog(QWidget *parent = 0);

    ///Деструктор окна настройки
    ~UGraphControlDialog();



    //**********************************************************Выдача изменяемых параметров
    /// Выдать диапазон оси Х, за которым будет происходить слежение
    int getDoubleSpinBoxData() const;

    /// Выдать решение об изменениии границы по оси Х
    int getAxisXChangeData(void) const;

    /// Выдать решение об изменениии границы по оси У вверх
    int getAxisMaxYChangeData(void) const;

    /// Выдать решение об изменениии границы по оси У вниз
    int getAxisMinYChangeData(void) const;

    /// Выдать выбранный цвет
    int getColor (void) const;

    /// Выдать подпись по оси Х
    QString getLableX(void) const;

    /// Выдать подпись по оси У
    QString getLableY(void) const;

    /// Выдать решение об удалении текущего графика
    bool getDelInf(void) const;
    //**********************************************************


    //**********************************************************Установка исходных параметров окна
    /// Установить наличие изменениий границ по оси Х
    void setAxisXChangeData(int axisX) const;

    ///Установить наличие изменениий границ по оси У вверх
    void setAxisMaxYChangeData(int axisY) const;

    ///Установить наличие изменениий границ по оси У вниз
    void setAxisMinYChangeData(int axisY) const;

    /// Установить количество знаков после запятой
    /// у диапазона оси Х, за которым происходит слежение
    void setDecimalPlaces (int i) const;

    ///Установить диапазон оси Х, за которым происходит слежение
    void setDisplayInterval (int i) const;

    /// Установить подпись по оси Х
    void setLableX (QString lableX) const;

    /// Установить подпись по оси У
    void setLableY (QString lableY) const;
    //**********************************************************

private slots:

    /// Если нажали на кнопку удаления текущего графика
    void on_pushButton_clicked();

public slots:

    /// Должно ли быть активным окно с диапазоном слежения по оси Х
    void isActiveDoubleSpinBox(bool b);

    /// Если нажали изменить цвет - нужно изменить цвет
    void on_colorListWidget_currentRowChanged(int currentRow);

private:
    Ui::UGraphControlDialog *ui;
};

#endif // UGRAPHCONTROLDIALOG_H
