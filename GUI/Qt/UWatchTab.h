#ifndef UWATCHTAB_H
#define UWATCHTAB_H

#include <iostream>

#include <QWidget>
#include <QString>
#include <QVector>
#include <QSplitter>

#include "UWatchChart.h"
#include "UMatrixFormDialog.h"
#include "UVisualControllerWidget.h"
#include "UComponentPropertySelectionWidget.h"
#include "UWatchChartOption.h"
#include "UWatchSeriesOption.h"

using namespace QtCharts;

namespace Ui {
class UWatchTab;
}

class UWatchChart;
//////////////////////////////////////////////////////////////////////////////
// �����, ����������� ���� ������� � ����� ��� ����������� ���������
//////////////////////////////////////////////////////////////////////////////

class UWatchTab : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UWatchTab(QWidget *parent = nullptr, RDK::UApplication* app = NULL);
    ~UWatchTab();

    ///�������� ������������ ��������
    void createGridLayout(int rowNumber, int colNumber);


    UWatchChart *getChart(int index);
    int countGraphs();

    ///��������� ���� ��� ������ ��������� ������
    void createSelectionDialog(int chartIndex);

    ///������� ���������� �������, ��
    int UpdateIntervalMs = 200;
    void saveUpdateInterval(int newInterval);

    //������ ���������� ����� � ��������
    int getColNumber();
    int getRowNumber();

    // ��������� ��������� ���������� � xml
    virtual void ASaveParameters(RDK::USerStorageXML &xml);

    // ��������� ��������� ���������� �� xml
    virtual void ALoadParameters(RDK::USerStorageXML &xml);



private:
    void createGraph();
    void createSplitterGrid(int rowNumber);
    void deleteGraph(int index);
    void deleteGraphs(int new_graph_count);

    int tabColNumber=0;
    int tabRowNumber=0;

    ///������ �������� �� ����� �������
    QVector <UWatchChart*> graph;
    std::list<double> XData;
    std::list<double> YData;
    QVector<QPointF> points;

    QSplitter *colSplitter;              //colSplitter
    QVector <QSplitter*> rowSplitter;    // |----..---rowSplitter[0]
                                         // |----..---
                                         // |----..---
                                         // ...
                                         // |----..---rowSplitter[n]

    Ui::UWatchTab *ui;

    UWatchChartOption *chartOption;
    UWatchSeriesOption *seriesOption;

    ///��������� �������
    ///��� ������� ������� - ���������� � ����, ����� ������� �� ��������� ��������� ������
    ///�� ������ ������� �� ���� ������
    virtual void AUpdateInterface();

    /// ��������� ��������� ������ ����� �� ����
    virtual void ReadSeriesDataSafe(int graphIndex, int serieIndex, std::list<double> &xdata, std::list<double> &ydata);

public slots:
    void createSelectionDialogSlot(int index);
    void seriesOptionTriggered();
    void chartsOptionTriggered();

};

#endif // UWATCHTAB_H
