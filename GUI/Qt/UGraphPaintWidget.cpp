#include "UGraphPaintWidget.h"
#include "ui_UGraphWidget.h"
#include "ui_UGraphPaintWidget.h"
#include <iostream>

UGraphPaintWidget::UGraphPaintWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UGraphPaintWidget)
{
    ui->setupUi(this);
}

UGraphPaintWidget::~UGraphPaintWidget()
{
    delete ui;
}

// Определяет границы графика по оси абсцисс
// Подписывает наименование осей
void UGraphPaintWidget::mainStartGraphSettings (double leftLimit, double rightLimit,
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
    std::cout<<getLeftLimitGraph()<<std::endl;
    std::cout<<getRightLimitGraph()<<std::endl;
    std::cout<<getLowerLimitGraph()<<std::endl;
    std::cout<<getUpperLimitGraph()<<std::endl;
}
\

// Изменяет параметры цвета и имени одного и графиков
// Если график новый - добавляет его в вектор следующим элементом
// Изменяет номер "активного" графика на изменяемый
int UGraphPaintWidget::addGraphVisualParameters(const std::string &graphName, QColor myColor)
{
    int myID=vectorGraph.size();
    TSingleGraph newTSingleGraph;
    newTSingleGraph.t_graphColor=myColor;
    newTSingleGraph.t_graphName=graphName;
    vectorGraph.push_back(newTSingleGraph);
    std::cout<<"addGraph"<<std::endl;
    ui->widget->addGraph();
    ui->widget->graph(0)->setPen(newTSingleGraph.t_graphColor);
    return myID;
}

int UGraphPaintWidget::addGraphVisualParameters(const std::string &graphName, int myColor)
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
    int myID=vectorGraph.size();
    TSingleGraph newTSingleGraph;
    newTSingleGraph.t_graphColor=masColor[myColor];
    newTSingleGraph.t_graphName=graphName;
    vectorGraph.push_back(newTSingleGraph);
    std::cout<<"addGraph"<<std::endl;
    ui->widget->addGraph();
    ui->widget->graph(0)->setPen(newTSingleGraph.t_graphColor);
    return myID;
}

int UGraphPaintWidget::addGraphVisualParameters(const std::string &graphName)
{
    int myID=vectorGraph.size();
    TSingleGraph newTSingleGraph;
    newTSingleGraph.t_graphColor=Qt::green;
    newTSingleGraph.t_graphName=graphName;
    vectorGraph.push_back(newTSingleGraph);
    std::cout<<"addGraph"<<std::endl;
    ui->widget->addGraph();
    ui->widget->graph(0)->setPen(newTSingleGraph.t_graphColor);
    return myID;
}

void UGraphPaintWidget::setGraphDataSource(int graph_index, int channel_index, const std::string &componentName,
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


void UGraphPaintWidget::delAllGraph(void)
{
    vectorGraph.clear();
    ui->widget->clearGraphs();
    ui->widget->replot();
    currentItem=-1;
}

//Определяет форму графика (передает массивы)
void UGraphPaintWidget::setData(int id, QVector<double> X, QVector<double> Y)
{
    ui->widget->graph(id)->setData(X,Y);
    //currentItem=id;
}

void UGraphPaintWidget::changeCurrentItem(void)
{
    //std::cout<<"start "<<currentItem<<std::endl;
    if (currentItem==-2)
    {
       // std::cout<<"changeCurrentItem1"<<currentItem<<std::endl;
        currentItem=0;
        ui->widget->graph(currentItem)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
        ui->widget->replot();
    }
    else
        if (currentItem!=-1)
        {
            //std::cout<<"changeCurrentItem2"<<currentItem<<std::endl;
            ui->widget->graph(currentItem)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
            currentItem++;
            if (currentItem>=(vectorGraph.size()))
                currentItem=-2;
            if (currentItem!=-2)
            {
                ui->widget->graph(currentItem)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
                ui->widget->replot();
            }
        }
    //std::cout<<"finish "<<currentItem<<std::endl;
}

void UGraphPaintWidget::changeColor(void)
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

int UGraphPaintWidget::getSize(void) const
{
    return vectorGraph.size();
}

const TSingleGraph& UGraphPaintWidget::getGraph(int id) const
{
    return vectorGraph[id];
}

void UGraphPaintWidget::redrawGraph(void)
{
    ui->widget->xAxis->setRange(leftLimitGraph, rightLimitGraph);
    ui->widget->yAxis->setRange(lowerLimitGraph, upperLimitGraph);
    ui->widget->replot();
}



//---------------------------------------------------Выдача параметров


int  UGraphPaintWidget::getStructContent(int i, int* graphColor, std::string* graphName, std::string* nameCompоnent,
                      std::string* nameProperty, std::string* typeProperty, int* Jx, int* Jy)
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
    for (int j=0;j<18; j++)
        if (vectorGraph[i].t_graphColor == masColor[j])
        {
            *graphColor = j;
        }
    *graphName = vectorGraph[i].t_graphName;
    *nameCompоnent = vectorGraph[i].nameCompоnent;
    *nameProperty = vectorGraph[i].nameProperty;
    *typeProperty = vectorGraph[i].typeProperty;
    *Jx = vectorGraph[i].Jx;
    *Jy= vectorGraph[i].Jy;

    return vectorGraph[i].indexChannel;
}

int UGraphPaintWidget::getCurrentItem(void) const
{
    return currentItem;
}

double UGraphPaintWidget::getLeftLimitGraph(void) const
{
    return leftLimitGraph;
}

double UGraphPaintWidget::getRightLimitGraph(void) const
{
    return rightLimitGraph;
}

double UGraphPaintWidget::getUpperLimitGraph(void) const
{
    return upperLimitGraph;
}

double UGraphPaintWidget::getLowerLimitGraph(void) const
{
    return lowerLimitGraph;
    //std::cout<<lowerLimitGraph<<std::endl;
}

//---------------------------------------------------Занесение параметров

void UGraphPaintWidget::setCurrentItem(int myCurrentItem)
{
    currentItem=myCurrentItem;

}

void UGraphPaintWidget::setLeftLimitGraph(double myLimitGraph)
{
    leftLimitGraph=myLimitGraph;
}

void UGraphPaintWidget::setRightLimitGraph(double myLimitGraph)
{
    rightLimitGraph=myLimitGraph;
}

void UGraphPaintWidget::setUpperLimitGraph(double myLimitGraph)
{
    upperLimitGraph=myLimitGraph;
}

void UGraphPaintWidget::setLowerLimitGraph(double myLimitGraph)
{
    lowerLimitGraph=myLimitGraph;
}

void UGraphPaintWidget::setColorCurrentItem(int color)
{
    static QColor masColor[]={
            Qt::black, //0
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
            Qt::darkYellow  //16
                              };
    if ((currentItem!=-1)&&(color>=0)&&(color<=16))
    {
        vectorGraph[currentItem].t_graphColor=masColor[color];
        ui->widget->graph(currentItem)->setPen(QPen(vectorGraph[currentItem].t_graphColor));
        ui->widget->replot();
    }
}

void UGraphPaintWidget::setLables(QString lableX, QString lableY)
{
    ui->widget->xAxis->setLabel(lableX);
    ui->widget->yAxis->setLabel(lableY);
    ui->widget->replot();
}
//--------------------------------------------------
int UGraphPaintWidget::delCurrentItemGraph()
{

    int a = currentItem;
    if (currentItem>=0)
    {
        ui->widget->removeGraph(currentItem);
        //удаляем нужный жлемент структуры
        vectorGraph.erase(vectorGraph.begin()+currentItem);
        //перестать выделять текущий график
        if (vectorGraph.size()<1)
            currentItem=-1;
        else
            currentItem=-2;
    }
    return a;
}


