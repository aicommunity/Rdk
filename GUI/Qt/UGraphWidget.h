#ifndef U_GRAPH_WIDGET_H
#define U_GRAPH_WIDGET_H

#include "UVisualControllerWidget.h"
#include "UStructSingleGraph.h"
#include "qcustomplot.h"
#include "UGraphWindow.h"
#include "UComponentPropertySelectionWidget.h"


namespace Ui {
class UGraphWidget;
}

class UGraphWidget : public UVisualControllerWidget
{
        Q_OBJECT

    public:
        /// Вектор X, в которых хранятся массивы точек, по которым строятся графики
        std::vector<QVector<double>> masX;

        /// Вектор Y, в которых хранятся массивы точек, по которым строятся графики
        std::vector<QVector<double>> masY;

        /// Флаг обновления правой границы графика
        /// Если он -1, то не обновляется
        /// если 1 - обновляется
        int flagUpdateBordersX;
        int flagUpdateBordersY;

        /// Флаг говорит о том, что должны указываться последние
        /// Н-элементов по оси Х
        /// Если число -1, то график должен
        double lastNElements;

        ///Подпись оси х
        QString lableX;
        ///Подпись оси х
        QString lableY;

    public:
        /// Конструктор
        /// Задает врем обновления графика
        /// Создает в себе Виджет графика
        /// Связывает кнопки с действиями Виджета графика
        /// Определяет начальные параметры: размеры СК, подписи осей
        explicit UGraphWidget(QWidget *parent = 0, RDK::UApplication* app = NULL);

        /// Деструктор
        ~UGraphWidget();

        /// Передача значений, по которым строить график
        /// Параметры номер графика, координаты X, координаты Y
        void addDataToGraph(int id, std::vector<double> X1, std::vector<double> Y1);
        void addDataToGraph(int id, double X1, double Y1);


        ///Возвращает указатель на private OtherWindow
        UGraphWindow *getGraphPainter() const;

        ///Обновляет график
        ///Обращаемся к ядру, берем матрицу
        ///Ее данные положим на этот график
        virtual void AUpdateInterface();

        /// Запись файла настроек
        virtual void ASaveParameters();
        /// Считывание файла настроек
        virtual void ALoadParameters();

    signals:
        /*///Сигнал от кнопки drawOneGraph
        void drawSmth();*/

        ///Сигнал от кнопки delAllButton
        void delAllButtonSignal();

        ///Сигнал рисовалке, что нужно нарисовать
        ///В графике id по точкам X и Y продолжение графика
        void transferDataSignal(int id, QVector<double> X, QVector<double> Y);

        ///Сигнал от кнопки changeColor
        void changeColorSignal();

        ///Сигнал от кнопки сhangeCurrentItem
        void changeCurrentItemSignal();

public slots:
        void slotActionSelectOutput();
        void slotActionDeleteCurrentItem();
        void slotActionSettings();

private slots:
        //void on_drawOneGraph_clicked();

        /*/// При нажатии на кнопку Select Output вызывается меню
        /// В котором нужно выбрать какой из входов будет отображаться на графике
        void on_selectDir_clicked();*/

        /*/// Ставит/снимает флаг, влияющий на обновление границ графика
        //void on_updateBordersButton_clicked();*/

        /*void on_callDialog_clicked();*/

private:
        Ui::UGraphWidget *ui;
        UGraphWindow* graphPainter;
};

#endif // U_GRAPH_WIDGET_H
