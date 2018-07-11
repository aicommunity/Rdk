#ifndef OTHERWINDOW_H
#define OTHERWINDOW_H
#include "UStructSingleGraph.h"
#include <QWidget>

;
namespace Ui {
class OtherWindow;
}

class OtherWindow : public QWidget
{
        Q_OBJECT
    ///Вектор структур, содержащих описание параметров графика
    std::vector<TSingleGraph> vectorGraph;
    ///Номер текущего элемента (изменение котороо происходит)
    int currentItem;
    ///Границы поля СК
    double leftLimitGraph;    //Начало Х
    double rightLimitGraph;   //Конец Х
    double upperLimitGraph;   //Начало У
    double lowerLimitGraph;   //Конец У


    public:
        explicit OtherWindow(QWidget *parent = 0);
        ~OtherWindow();

    public:
        ///Добавляет на СК основные пааметры - левую и правую границу, подпись осей
        void mainSettings (double leftLimit, double rightLimit, const QString& nameX, const QString& nameY);
        ///Указывает параметры структуры, характеризующие вид графика (цвет, имя)
        ///Возвращает индекс
        int addGraph(const std::string &graphName, QColor myColor);
        int addGraph(const std::string &graphName);
        ///По индексу добавляет оставшиеся параметры структуры
        void setGraphDataSource(int graph_index, int channel_index, const std::string &componentName, const std::string &propertyName,
                                const std::string &type, int jx, int jy);
        ///Возвращает количество элементов в векторе структур
        ///т.е.количество уже описанных внешне графиков
        int getSize(void);
        ///По индексу возвращает структуру с параметрами графика
        const TSingleGraph &getGraph(int id) const;
        ///Обновляет график
        void redrawGraph(void);


    public slots:
        ///Рисует первый раз начальные позиции графика
        void pressEvent();
        ///Удаляет все графики с СК
        void delAllGraph(void);
        ///Получает данные от GraphWin
        ///записывает их в graph()
        void dataFromMainWin(int id, QVector<double> X, QVector<double> Y);
        ///Изменяет текущий график на следующий
        void changeCurrentItem(void);
        ///Изменяет цвет текущего графика на следующий
        void changeColor(void);

    private:
        Ui::OtherWindow *ui;
};

#endif // OTHERWINDOW_H
