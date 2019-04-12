#ifndef T_SINGLE_GRAPH_H
#define T_SINGLE_GRAPH_H


#include <string>
#include "qcustomplot.h"

struct TSingleGraph
{
    QColor t_graphColor;       //���� �������
    std::string t_graphName;   //��� �������
    int indexChannel;          //������ ������
    std::string nameComponent; //��� ����������
    std::string nameProperty;  //��� ��������
    std::string typeProperty;  //��� ����������
    //���������� �������� �������
    int Jx;
    int Jy;
};

#endif // TSINGLEGRAPH_H

