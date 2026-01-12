#ifndef T_SINGLE_GRAPH_H
#define T_SINGLE_GRAPH_H


#include <string>
#include "qcustomplot.h"

struct TSingleGraph
{
    QColor t_graphColor;       //Цвет графика
    std::string t_graphName;   //Имя графика
    int indexChannel;          //Индекс канала
    std::string nameComponent; //Имя компонента
    std::string nameProperty;  //Имя свойства
    std::string typeProperty;  //Тип компонента
    //координаты элемента матрицы
    int Jx;
    int Jy;
    bool Online;

    TSingleGraph()
        : indexChannel(-1)
        , Jx(0)
        , Jy(0)
        , Online(true)
    {
    }
};

#endif // TSINGLEGRAPH_H

