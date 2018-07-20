#ifndef U_GRAPH_WINDOW_H
#define U_GRAPH_WINDOW_H
#include "UStructSingleGraph.h"
#include <QWidget>

namespace Ui {
class UGraphWindow;
}

class UGraphWindow : public QWidget
{
        Q_OBJECT
    /// Вектор структур, содержащих описание параметров графика
    std::vector<TSingleGraph> vectorGraph;

    /// Номер текущего элемента (изменение которого происходит)
    int currentItem;

    /// Границы поля СК Начало Х
    double leftLimitGraph;
    /// Границы поля СК Конец Х
    double rightLimitGraph;
    /// Границы поля СК Начало Y
    double upperLimitGraph;
    /// Границы поля СК Конец Y
    double lowerLimitGraph;


    public:
        explicit UGraphWindow(QWidget *parent = 0);
        ~UGraphWindow();

    public:
        /// Добавляет на СК основные начальные параметры пааметры
        /// Границы СК, подписи осей
        void mainStartGraphSettings (double leftLimit, double rightLimit,double lowerLimit, double upperLimit,
                                     const QString& nameX, const QString& nameY);

        /// Указывает параметры структуры, характеризующие вид графика (цвет, имя)
        /// Возвращает индекс
        int addGraphVisualParameters(const std::string &graphName, QColor myColor);
        int addGraphVisualParameters(const std::string &graphName, int myColor);
        int addGraphVisualParameters(const std::string &graphName);

        /// По индексу добавляет источник данных для конкретного графика
        void setGraphDataSource(int graph_index, int channel_index, const std::string &componentName,
                                const std::string &propertyName, const std::string &type, int jx, int jy);

        /// Возвращает количество элементов в векторе структур
        /// т.е.количество уже описанных внешне графиков
        int getSize(void);

        /// По индексу возвращает структуру с параметрами графика
        const TSingleGraph &getGraph(int id) const;

        /// Обновляет график
        void redrawGraph(void);

        //------------------------------------------------------------

        /// По номеру элемента выдает значения структуры вектора структур
        /// Все значения кроме индекса канала передаются в виде указателей
        int  getStructContent(int i, int* graphColor, std::string* graphName, std::string* nameCompоnent,
                               std::string* nameProperty, std::string* typeProperty, int* Jx, int* Jy);

        ///Передает значение currentItem
        int getCurrentItem(void);

        ///Передает значение Границы поля СК Начало Х
        double getLeftLimitGraph(void);

        ///Передает Границы поля СК Конец Х
        double getRightLimitGraph(void);

        ///Передает Границы поля СК Начало Y
        double getUpperLimitGraph(void);

        ///Передает Границы поля СК Конец Y
        double getLowerLimitGraph(void);

        //-------------------------------------------------------------

        ///Выставляет нужное значение currentItem
        void setCurrentItem(int myCurrentItem);

        ///Выставляет нужные Границы поля СК Начало Х
        void setLeftLimitGraph(double myLimitGraph);

        /// Выставляет нужные Границы поля СК Конец Х
        void setRightLimitGraph(double myLimitGraph);

        /// Выставляет нужные Границы поля СК Начало Y
        void setUpperLimitGraph(double myLimitGraph);

        /// Выставляет нужные Границы поля СК Конец Y
        void setLowerLimitGraph(double myLimitGraph);

        void setColorCurrentItem(int color);

        void setLables(QString lableX,QString  lableY);

        //----------------------------------------------------------------

        int delCurrentItemGraph(void);



    public slots:

        /// Удаляет все графики с СК
        void delAllGraph(void);

        /// Получает данные от GraphWin
        /// записывает их в graph()
        void setData(int id, QVector<double> X, QVector<double> Y);

        /// Изменяет текущий график на следующий
        void changeCurrentItem(void);

        /// Изменяет цвет текущего графика на следующий
        void changeColor(void);


    private:
        Ui::UGraphWindow *ui;
};

#endif // OU_GRAPH_WINDOW_H
