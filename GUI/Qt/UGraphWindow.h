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

        /// Изменяет значение Правой границы графика
        void changeRightBorder(double newRightLim);


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
