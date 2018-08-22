#ifndef U_GRAPH_PAINT_WIDGET_H
#define U_GRAPH_PAINT_WIDGET_H
#include "UStructSingleGraph.h"
#include <QWidget>

namespace Ui {
class UGraphPaintWidget;
}

class UGraphPaintWidget : public QWidget
{
        Q_OBJECT
    /// ������ ��������, ���������� �������� ���������� �������
    std::vector<TSingleGraph> vectorGraph;

    /// ����� �������� �������� (��������� �������� ����������)
    int currentItem;

    /// ������� ���� �� ������ �
    double leftLimitGraph;
    /// ������� ���� �� ����� �
    double rightLimitGraph;
    /// ������� ���� �� ������ Y
    double upperLimitGraph;
    /// ������� ���� �� ����� Y
    double lowerLimitGraph;


    public:
        explicit UGraphPaintWidget(QWidget *parent = 0);
        ~UGraphPaintWidget();

    public:
        /// ��������� �� �� �������� ��������� ��������� ��������
        /// ������� ��, ������� ����
        void mainStartGraphSettings (double leftLimit, double rightLimit,double lowerLimit, double upperLimit,
                                     const QString& nameX, const QString& nameY);

        /// ��������� ��������� ���������, ��������������� ��� ������� (����, ���)
        /// ���������� ������
        int addGraphVisualParameters(const std::string &graphName, QColor myColor);
        int addGraphVisualParameters(const std::string &graphName, int myColor);
        int addGraphVisualParameters(const std::string &graphName);

        /// �� ������� ��������� �������� ������ ��� ����������� �������
        void setGraphDataSource(int graph_index, int channel_index, const std::string &componentName,
                                const std::string &propertyName, const std::string &type, int jx, int jy);

        /// ���������� ���������� ��������� � ������� ��������
        /// �.�.���������� ��� ��������� ������ ��������
        int getSize(void) const;

        /// �� ������� ���������� ��������� � ����������� �������
        const TSingleGraph &getGraph(int id) const;

        /// ��������� ������
        void redrawGraph(void);

        //------------------------------------------------------------

        /// �� ������ �������� ������ �������� ��������� ������� ��������
        /// ��� �������� ����� ������� ������ ���������� � ���� ����������
        int getStructContent(int i, int* graphColor, std::string* graphName, std::string* nameComponent,
                               std::string* nameProperty, std::string* typeProperty, int* Jx, int* Jy);

        ///�������� �������� currentItem
        int getCurrentItem(void) const;

        ///�������� �������� ������� ���� �� ������ �
        double getLeftLimitGraph(void) const;

        ///�������� ������� ���� �� ����� �
        double getRightLimitGraph(void) const;

        ///�������� ������� ���� �� ������ Y
        double getUpperLimitGraph(void)  const;

        ///�������� ������� ���� �� ����� Y
        double getLowerLimitGraph(void)  const;

        //-------------------------------------------------------------

        ///���������� ������ �������� currentItem
        void setCurrentItem(int myCurrentItem);

        ///���������� ������ ������� ���� �� ������ �
        void setLeftLimitGraph(double myLimitGraph);

        /// ���������� ������ ������� ���� �� ����� �
        void setRightLimitGraph(double myLimitGraph);

        /// ���������� ������ ������� ���� �� ������ Y
        void setUpperLimitGraph(double myLimitGraph);

        /// ���������� ������ ������� ���� �� ����� Y
        void setLowerLimitGraph(double myLimitGraph);

        ///������������� ���� �������� �������
        void setColorCurrentItem(int color);

        ///������������� ������� �� ���� � � �
        void setLables(QString lableX,QString  lableY);

        //----------------------------------------------------------------

        ///������� ������� ������
        int delCurrentItemGraph(void);



    public slots:

        /// ������� ��� ������� � ��
        void delAllGraph(void);

        /// �������� ������ �� GraphWin
        /// ���������� �� � graph()
        void setData(int id, QVector<double> X, QVector<double> Y);

        /// �������� ������� ������ �� ���������
        void changeCurrentItem(void);

        /// �������� ���� �������� ������� �� ���������
        void changeColor(void);


    private:
        Ui::UGraphPaintWidget *ui;
};

#endif // OU_GRAPH_PAINT_WIDGET_H
