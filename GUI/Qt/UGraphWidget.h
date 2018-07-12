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
        /// ������ X, � ������� �������� ������� �����, �� ������� �������� �������
        std::vector<QVector<double>> masX;

        /// ������ Y, � ������� �������� ������� �����, �� ������� �������� �������
        std::vector<QVector<double>> masY;

        /// ���� ���������� ������ ������� �������
        /// ���� �� -1, �� �� �����������
        /// ���� 1 - �����������
        int flagUpdateBorders;

    public:
        /// �����������
        /// ������ ���� ���������� �������
        /// ������� � ���� ������ �������
        /// ��������� ������ � ���������� ������� �������
        /// ���������� ��������� ���������: ������� ��, ������� ����
        explicit UGraphWidget(QWidget *parent = 0, RDK::UApplication* app = NULL);

        /// ����������
        ~UGraphWidget();

        /// �������� ��������, �� ������� ������� ������
        /// ��������� ����� �������, ���������� X, ���������� Y
        void addDataToGraph(int id, std::vector<double> X1, std::vector<double> Y1);
        void addDataToGraph(int id, double X1, double Y1);


        ///���������� ��������� �� private OtherWindow
        UGraphWindow *getGraphPainter() const;

        ///��������� ������
        ///���������� � ����, ����� �������
        ///�� ������ ������� �� ���� ������
        virtual void AUpdateInterface();

    signals:
        /*///������ �� ������ drawOneGraph
        void drawSmth();*/

        ///������ �� ������ delAllButton
        void delAllButtonSignal();

        ///������ ���������, ��� ����� ����������
        ///� ������� id �� ������ X � Y ����������� �������
        void transferDataSignal(int id, QVector<double> X, QVector<double> Y);

        ///������ �� ������ changeColor
        void changeColorSignal();

        ///������ �� ������ �hangeCurrentItem
        void changeCurrentItemSignal();


private slots:
        //void on_drawOneGraph_clicked();

        /// ��� ������� �� ������ Select Output ���������� ����
        /// � ������� ����� ������� ����� �� ������ ����� ������������ �� �������
        void on_selectDir_clicked();

        /// ������/������� ����, �������� �� ���������� ������ �������
        void on_updateBordersButton_clicked();

private:
        Ui::UGraphWidget *ui;
        UGraphWindow* graphPainter;
};

#endif // U_GRAPH_WIDGET_H
