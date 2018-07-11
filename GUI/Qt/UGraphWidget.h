#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H


#include "UVisualControllerWidget.h"
#include "UStructSingleGraph.h"
#include "qcustomplot.h"
#include "UGraphWindow.h"
#include "UComponentPropertySelectionWidget.h"


namespace Ui {
class GraphWindow;
}

class GraphWindow : public UVisualControllerWidget
{
        Q_OBJECT

    public:
        ///Вектора, в которых хранятся массивы точек, по которым строятся графики
        std::vector<QVector<double>> masX;
        std::vector<QVector<double>> masY;

    public:
        ///Конструктор-деструктор
        explicit GraphWindow(QWidget *parent = 0, RDK::UApplication* app = NULL);
        ~GraphWindow();
        ///Передача значений, по которым строить
        void addMainData(int id, std::vector<double> X1, std::vector<double> Y1);
        void addMainData(int id, double X1, double Y1);
        /*///Передача дополнительных параметров
        ///Например тех данных, которые появились по ходу работы
        void addData (int id, std::vector<double> X, std::vector<double> Y);*/

        ///Возвращает указатель на private OtherWindow
        OtherWindow *getGraphPainter() const;

        ///Обновляет график
        ///Обращаемся к ядру, берем матрицу
        ///Ее данные положим на этот график
        virtual void AUpdateInterface();

    signals:
        ///Сигнал от кнопки drawOneGraph
        void drawSmth();
        ///Сигнал от кнопки delAllButton
        void delSignal();
        ///Сигнал рисовалке, что нужно нарисовать
        ///В графике id по точкам X и Y продолжение графика
        void transferDataSignal(int id, QVector<double> X, QVector<double> Y);
        ///Сигнал от кнопки changeColor
        void changeColorSignal();
        ///Сигнал от кнопки сhangeCurrentItem
        void changeCurrentItemSignal();


private slots:
        void on_delAllButton_clicked();
        void on_changeCurrentItem_clicked();
        void on_changeColor_clicked();
        void on_drawOneGraph_clicked();
        void on_selectDir_clicked();

private:
        Ui::GraphWindow *ui;
        OtherWindow* graphPainter;
};

#endif // GRAPHWINDOW_H
