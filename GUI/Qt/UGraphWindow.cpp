#include "UGraphWindow.h"
#include "ui_otherwindow.h"
#include "ui_graphwindow.h";
#include <iostream>

UGraphWindow::UGraphWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UGraphWindow)
{
    ui->setupUi(this);
}

UGraphWindow::~UGraphWindow()
{
    delete ui;
}

// Определяет границы графика по оси абсцисс
// Подписывает наименование осей
void UGraphWindow::mainStartGraphSettings (double leftLimit, double rightLimit,
                                           double lowerLimit, double upperLimit,
                                           const QString& nameX, const QString& nameY)
{
    std::cout<<"MainSettings"<<std::endl;
    ui->widget->clearGraphs();//очищаем все графики

    leftLimitGraph=leftLimit;
    rightLimitGraph=rightLimit;
    lowerLimitGraph=lowerLimit;
    upperLimitGraph=upperLimit;

    //Применем начальные параметры
    ui->widget->xAxis->setLabel(nameX);
    ui->widget->yAxis->setLabel(nameY);
    ui->widget->xAxis->setRange(leftLimitGraph, rightLimitGraph);
    ui->widget->yAxis->setRange(lowerLimitGraph, upperLimitGraph);
    currentItem=-1;
}
\

// Изменяет параметры цвета и имени одного и графиков
// Если график новый - добавляет его в вектор следующим элементом
// Изменяет номер "активного" графика на изменяемый
int UGraphWindow::addGraphVisualParameters(const std::string &graphName, QColor myColor)
{
    int myID=vectorGraph.size();
    TSingleGraph newTSingleGraph;
    newTSingleGraph.t_graphColor=myColor;
    newTSingleGraph.t_graphName=graphName;
    vectorGraph.push_back(newTSingleGraph);
    std::cout<<"addGraph"<<std::endl;
    ui->widget->addGraph();
    return myID;
}

int UGraphWindow::addGraphVisualParameters(const std::string &graphName)
{
    int myID=vectorGraph.size();
    TSingleGraph newTSingleGraph;
    newTSingleGraph.t_graphColor=Qt::green;
    newTSingleGraph.t_graphName=graphName;
    vectorGraph.push_back(newTSingleGraph);
    std::cout<<"addGraph"<<std::endl;
    ui->widget->addGraph();
    return myID;
}

void UGraphWindow::setGraphDataSource(int graph_index, int channel_index, const std::string &componentName,
                                      const std::string &propertyName,
                                      const std::string &type, int jx, int jy)
{
    vectorGraph[graph_index].indexChannel=channel_index;
    vectorGraph[graph_index].nameCompоnent=componentName;
    vectorGraph[graph_index].nameProperty=propertyName;
    vectorGraph[graph_index].typeProperty=type;
    vectorGraph[graph_index].Jx=jx;
    vectorGraph[graph_index].Jy=jy;
}

/*// Применение границ окна СК
// Идет по вектору структур и рисует графики
void UGraphWindow::pressEvent()
{
    int t=0;
    upperLimitGraph=2;
    lowerLimitGraph=-2;
    ui->widget->xAxis->setRange(leftLimitGraph, rightLimitGraph);
    ui->widget->yAxis->setRange(lowerLimitGraph, upperLimitGraph);
    for (size_t j = 0; j<vectorGraph.size(); j++)
    {
        if (currentItem!=-1)
        {
            ui->widget->graph(j)->setPen(QPen(vectorGraph[j].t_graphColor));
            t++;
        }
    }
    if (currentItem!=-1)
        ui->widget->graph(currentItem)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
    if(t!=0)
        ui->widget->replot();
}*/


void UGraphWindow::delAllGraph(void)
{
    vectorGraph.clear();
    ui->widget->clearGraphs();
    ui->widget->replot();
    currentItem=-1;
}

//Определяет форму графика (передает массивы)
void UGraphWindow::setData(int id, QVector<double> X, QVector<double> Y)
{
    ui->widget->graph(id)->setData(X,Y);
    currentItem=id;
}

void UGraphWindow::changeCurrentItem(void)
{
    if (currentItem!=-1)
    {
        std::cout<<"changeCurrentItem"<<std::endl;
        ui->widget->graph(currentItem)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
        currentItem++;
        if (currentItem>=vectorGraph.size())
            currentItem=0;
        ui->widget->graph(currentItem)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
        ui->widget->replot();
    }
}

void UGraphWindow::changeColor(void)
{
    static QColor masColor[]={
            Qt::color1,
            Qt::black,
            Qt::white,
            Qt::darkGray,
            Qt::gray,
            Qt::lightGray,
            Qt::red,
            Qt::green,
            Qt::blue,
            Qt::cyan,
            Qt::magenta,
            Qt::yellow,
            Qt::darkRed,
            Qt::darkGreen,
            Qt::darkBlue,
            Qt::darkCyan,
            Qt::darkMagenta,
            Qt::darkYellow,
            Qt::transparent
                              };
    if (currentItem!=-1)
    {
        for (int j=0;j<18;j++)
        {
            if (vectorGraph[currentItem].t_graphColor==masColor[j])
            {
                    if(j==17)
                        vectorGraph[currentItem].t_graphColor=masColor[0];
                    else
                        vectorGraph[currentItem].t_graphColor=masColor[j+1];
                    break;
            }
        }
        ui->widget->graph(currentItem)->setPen(QPen(vectorGraph[currentItem].t_graphColor));
        ui->widget->replot();
    }
}

int UGraphWindow::getSize(void)
{
    return vectorGraph.size();
}

const TSingleGraph& UGraphWindow::getGraph(int id) const
{
    return vectorGraph[id];
}

void UGraphWindow::redrawGraph(void)
{
    ui->widget->xAxis->setRange(leftLimitGraph, rightLimitGraph);
    //ui->widget->yAxis->setRange(lowerLimitGraph, upperLimitGraph);
    ui->widget->replot();
}

void UGraphWindow::changeRightBorder(double newRightLim)
{
    rightLimitGraph = newRightLim;
}
