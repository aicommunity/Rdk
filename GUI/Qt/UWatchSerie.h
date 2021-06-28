#ifndef UWATCHSERIE_H
#define UWATCHSERIE_H

#include <QLineSeries>
#include <QString>

namespace RDK
{
struct UControllerDataReader;
}

class UWatchSerie: public QtCharts::QLineSeries
{
public:
    UWatchSerie();
    //~UWatchSerie();

    //������ �� ��������� ������
    int indexChannel;      //������ ������
    QString nameComponent; //��� ����������
    QString nameProperty;  //��� ��������
    QString typeProperty;  //��� ����������
    double YShift;
    // ������ DataReader-��
    RDK::UControllerDataReader * data_reader;
    //���������� �������� �������
    int Jx;
    int Jy;
};

#endif // UWATCHSERIE_H
