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
        ///�������, � ������� �������� ������� �����, �� ������� �������� �������
        std::vector<QVector<double>> masX;
        std::vector<QVector<double>> masY;

    public:
        ///�����������-����������
        explicit GraphWindow(QWidget *parent = 0, RDK::UApplication* app = NULL);
        ~GraphWindow();
        ///�������� ��������, �� ������� �������
        void addMainData(int id, std::vector<double> X1, std::vector<double> Y1);
        void addMainData(int id, double X1, double Y1);
        /*///�������� �������������� ����������
        ///�������� ��� ������, ������� ��������� �� ���� ������
        void addData (int id, std::vector<double> X, std::vector<double> Y);*/

        ///���������� ��������� �� private OtherWindow
        OtherWindow *getGraphPainter() const;

        ///��������� ������
        ///���������� � ����, ����� �������
        ///�� ������ ������� �� ���� ������
        virtual void AUpdateInterface();

    signals:
        ///������ �� ������ drawOneGraph
        void drawSmth();
        ///������ �� ������ delAllButton
        void delSignal();
        ///������ ���������, ��� ����� ����������
        ///� ������� id �� ������ X � Y ����������� �������
        void transferDataSignal(int id, QVector<double> X, QVector<double> Y);
        ///������ �� ������ changeColor
        void changeColorSignal();
        ///������ �� ������ �hangeCurrentItem
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
