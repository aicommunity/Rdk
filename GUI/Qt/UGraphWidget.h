#ifndef U_GRAPH_WIDGET_H
#define U_GRAPH_WIDGET_H

#include "UVisualControllerWidget.h"
#include "UStructSingleGraph.h"
#include "qcustomplot.h"
#include "UGraphPaintWidget.h"
#include "UComponentPropertySelectionWidget.h"


namespace Ui {
class UGraphWidget;
}

class UGraphWidget : public UVisualControllerWidget
{
        Q_OBJECT

        /// Вектор X, в которых хранятся массивы точек, по которым строятся графики
        std::vector<QVector<double> > masX;

        /// Вектор Y, в которых хранятся массивы точек, по которым строятся графики
        std::vector<QVector<double> > masY;

        /// Флаг обновления правой границы графика
        /// Если он -1, то не обновляется
        /// если 1 - обновляется
        int flagUpdateBordersX;
        int flagUpdateBordersMaxY;
        int flagUpdateBordersMinY;

        /// Флаг говорит о том, что должны указываться последние
        /// Н-элементов по оси Х
        /// Если число -1, то график должен
        int lastNElements;

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
        explicit UGraphWidget(QWidget *parent = NULL, RDK::UApplication* app = NULL);

        /// Деструктор
        ~UGraphWidget();

        /// Передача значений, по которым строить график
        /// Параметры номер графика, координаты X, координаты Y
        void addDataToGraph(size_t id, std::vector<double> X1, std::vector<double> Y1);
        void addDataToGraph(size_t id, double X1, double Y1);

        /// Добавляет на СК основные начальные параметры пааметры
        /// Границы СК, подписи осей
        void mainGraphSettings (double leftLimit, double rightLimit,double lowerLimit, double upperLimit,
                                     const QString& nameX, const QString& nameY);

        /// Указывает параметры структуры, характеризующие вид графика (цвет, имя)
        /// Возвращает индекс
        int addGraphParameters(const std::string &graphName, int myColor);
        int addGraphParameters(const std::string &graphName);

        /// По индексу добавляет источник данных для конкретного графика
        void setGraphDataSource(int graph_index, int channel_index, const std::string &componentName,
                                const std::string &propertyName, const std::string &type, int jx, int jy);

        ///Выставляет нужное значение currentItem
        void setCurrentItem(int myCurrentItem);

        ///Возвращает указатель на private OtherWindow
        UGraphPaintWidget *getGraphPainter() const;

        ///Обновляет график
        ///Обращаемся к ядру, берем матрицу
        ///Ее данные положим на этот график
        virtual void AUpdateInterface();

        /// Запись файла настроек
        virtual void ASaveParameters();
        /// Считывание файла настроек
        virtual void ALoadParameters();

signals:

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


private:
        Ui::UGraphWidget* ui;
        UGraphPaintWidget* graphPainter;
};

#endif // U_GRAPH_WIDGET_H
