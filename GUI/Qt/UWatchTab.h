#ifndef UWATCHTAB_H
#define UWATCHTAB_H

#include <QWidget>
#include <QString>
#include <QVector>
#include <QGridLayout>

#include "UWatchChart.h"
#include "UVisualControllerWidget.h"
#include "UComponentPropertySelectionWidget.h"

using namespace QtCharts;

namespace Ui {
class UWatchTab;
}

//////////////////////////////////////////////////////////////////////////////
// Класс, реализующий одну вкладку с одним или несколькими графиками
//////////////////////////////////////////////////////////////////////////////

class UWatchTab : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UWatchTab(QWidget *parent = nullptr, RDK::UApplication* app = NULL);
    ~UWatchTab();

    ///создание расположений графиков
    void createSingleLayout();
    void createRowLayout(int rowNumber);
    void createColLayout(int colNumber);
    void createGridLayout(int rowNumber, int colNumber);

    UWatchChart *getChart(int index);
    int countGraphs();

    ///вызывется окно для выбора источника данных
    void createSelectionDialog(int chartIndex);
    int UpdateIntervalMs = 30;
    void saveUpdateInterval(int newInterval);

private:
    void createGrid();
    void createGraph();
    void deleteGraph(int index);
    void deleteAllGraph();

    ///вектор графиков на одной вкладке
    QVector <UWatchChart*> graph;

    QGridLayout *grid;
    Ui::UWatchTab *ui;

    ///Обновляет графики
    ///Для каждого графика - обращаемся к ядру, берем матрицу по заданному источнику данных
    ///Ее данные положим на этот график
    virtual void AUpdateInterface();

};

#endif // UWATCHTAB_H
