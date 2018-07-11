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
    ///������ ��������, ���������� �������� ���������� �������
    std::vector<TSingleGraph> vectorGraph;
    ///����� �������� �������� (��������� ������� ����������)
    int currentItem;
    ///������� ���� ��
    double leftLimitGraph;    //������ �
    double rightLimitGraph;   //����� �
    double upperLimitGraph;   //������ �
    double lowerLimitGraph;   //����� �


    public:
        explicit OtherWindow(QWidget *parent = 0);
        ~OtherWindow();

    public:
        ///��������� �� �� �������� �������� - ����� � ������ �������, ������� ����
        void mainSettings (double leftLimit, double rightLimit, const QString& nameX, const QString& nameY);
        ///��������� ��������� ���������, ��������������� ��� ������� (����, ���)
        ///���������� ������
        int addGraph(const std::string &graphName, QColor myColor);
        int addGraph(const std::string &graphName);
        ///�� ������� ��������� ���������� ��������� ���������
        void setGraphDataSource(int graph_index, int channel_index, const std::string &componentName, const std::string &propertyName,
                                const std::string &type, int jx, int jy);
        ///���������� ���������� ��������� � ������� ��������
        ///�.�.���������� ��� ��������� ������ ��������
        int getSize(void);
        ///�� ������� ���������� ��������� � ����������� �������
        const TSingleGraph &getGraph(int id) const;
        ///��������� ������
        void redrawGraph(void);


    public slots:
        ///������ ������ ��� ��������� ������� �������
        void pressEvent();
        ///������� ��� ������� � ��
        void delAllGraph(void);
        ///�������� ������ �� GraphWin
        ///���������� �� � graph()
        void dataFromMainWin(int id, QVector<double> X, QVector<double> Y);
        ///�������� ������� ������ �� ���������
        void changeCurrentItem(void);
        ///�������� ���� �������� ������� �� ���������
        void changeColor(void);

    private:
        Ui::OtherWindow *ui;
};

#endif // OTHERWINDOW_H
