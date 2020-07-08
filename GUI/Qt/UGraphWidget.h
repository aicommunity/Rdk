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

        /// ������ X, � ������� �������� ������� �����, �� ������� �������� �������
        std::vector<QVector<double> > masX;

        /// ������ Y, � ������� �������� ������� �����, �� ������� �������� �������
        std::vector<QVector<double> > masY;

        /// ���� ���������� ������ ������� �������
        /// ���� �� -1, �� �� �����������
        /// ���� 1 - �����������
        int flagUpdateBordersX;
        int flagUpdateBordersMaxY;
        int flagUpdateBordersMinY;

        /// ���� ������� � ���, ��� ������ ����������� ���������
        /// �-��������� �� ��� �
        /// ���� ����� -1, �� ������ ������
        int lastNElements;

        ///������� ��� �
        QString lableX;
        ///������� ��� �
        QString lableY;

    public:
        /// �����������
        /// ������ ���� ���������� �������
        /// ������� � ���� ������ �������
        /// ��������� ������ � ���������� ������� �������
        /// ���������� ��������� ���������: ������� ��, ������� ����
        explicit UGraphWidget(QWidget *parent = NULL, RDK::UApplication* app = NULL);

        /// ����������
        ~UGraphWidget();

        /// �������� ��������, �� ������� ������� ������
        /// ��������� ����� �������, ���������� X, ���������� Y
        void addDataToGraph(size_t id, std::vector<double> X1, std::vector<double> Y1);
        void addDataToGraph(size_t id, double X1, double Y1);

        /// ��������� �� �� �������� ��������� ��������� ��������
        /// ������� ��, ������� ����
        void mainGraphSettings (double leftLimit, double rightLimit,double lowerLimit, double upperLimit,
                                     const QString& nameX, const QString& nameY);

        /// ��������� ��������� ���������, ��������������� ��� ������� (����, ���)
        /// ���������� ������
        int addGraphParameters(const std::string &graphName, int myColor);
        int addGraphParameters(const std::string &graphName);

        /// �� ������� ��������� �������� ������ ��� ����������� �������
        void setGraphDataSource(int graph_index, int channel_index, const std::string &componentName,
                                const std::string &propertyName, const std::string &type, int jx, int jy);

        ///���������� ������ �������� currentItem
        void setCurrentItem(int myCurrentItem);

        ///���������� ��������� �� private OtherWindow
        UGraphPaintWidget *getGraphPainter() const;

        ///��������� ������
        ///���������� � ����, ����� �������
        ///�� ������ ������� �� ���� ������
        virtual void AUpdateInterface();

        /// ������ ����� ��������
        virtual void ASaveParameters();
        /// ���������� ����� ��������
        virtual void ALoadParameters();

signals:

        ///������ �� ������ delAllButton
        void delAllButtonSignal();

        ///������ ���������, ��� ����� ����������
        ///� ������� id �� ������ X � Y ����������� �������
        void transferDataSignal(int id, QVector<double> X, QVector<double> Y);

        ///������ �� ������ changeColor
        void changeColorSignal();

        ///������ �� ������ �hangeCurrentItem
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
