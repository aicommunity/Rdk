#include "UWatchTab.h"
#include "ui_UWatchTab.h"

UWatchTab::UWatchTab(QWidget *parent, RDK::UApplication* app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UWatchTab)
{
    ui->setupUi(this);
    //создаем один график на вкладке
    createSingleLayout();

    //время обновления графика
    UpdateInterval = UpdateIntervalMs;
}

UWatchTab::~UWatchTab()
{
    delete ui;
}

void UWatchTab::createGraph()
{
    graph.push_back(new UWatchChart(this));
}

void UWatchTab::deleteGraph(int index)
{
    delete graph[index];
    graph.remove(index);
}

void UWatchTab::deleteAllGraph()
{
    if (!graph.isEmpty())
        for (int i = graph.count()-1; i>=0 ; i--)
            deleteGraph(i);
}

void UWatchTab::AUpdateInterface()
{
    for (int graphIndex=0; graphIndex < graph.count(); graphIndex++)
    {
        double y;
        double x;
        for (int serieIndex=0; serieIndex < graph[graphIndex]->countSeries(); serieIndex++)
        {
            // обратимся к ядру, возьмем матрицу, компонент и имя свойства
            RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock<RDK::UNet>();

            RDK::UEPtr<RDK::UContainer> component=model->GetComponentL(graph[graphIndex]->getSerie(serieIndex)->nameComponent.toStdString(),true);
            if(!component)
                continue;

            RDK::MDMatrix<double>* m=component->AccessPropertyData<RDK::MDMatrix<double> >(graph[graphIndex]->getSerie(serieIndex)->nameProperty.toStdString());
            if(!m)
                continue;

            if(m->GetCols()<=graph[graphIndex]->getSerie(serieIndex)->Jx || graph[graphIndex]->getSerie(serieIndex)->Jy)
                continue;

            //вычисляем координаты точки
            y=(*m)(graph[graphIndex]->getSerie(serieIndex)->Jy,graph[graphIndex]->getSerie(serieIndex)->Jx);
            x=Model_GetDoubleRealTime();

            // и добавим ее в график
            graph[graphIndex]->addDataToSerie(serieIndex, x, y);
        }
        //смещаем ось Х вслед за временем
        if (x > graph[graphIndex]->axisXrange)
        {
            graph[graphIndex]->setAxisXmax(x+0.1);
            graph[graphIndex]->setAxisXmin(x-graph[graphIndex]->axisXrange);
        }

    }
}

void UWatchTab::createSingleLayout()
{
   createGrid();
   deleteAllGraph();
   createGraph();
   graph.last()->setChartTitle("Single graph");
   grid->addWidget(graph.last(),1,1);
}

void UWatchTab::createRowLayout(int rowNumber)
{
    createGrid();
    deleteAllGraph();
    for(int i=1; i <= rowNumber;i++)
    {
        createGraph();
        graph.last()->setChartTitle(QString("Horizontal graph %1").arg(QString::number(i)));
        grid->addWidget(graph.last(),i, 1);
    }
}

void UWatchTab::createColLayout(int colNumber)
{
    createGrid();
    deleteAllGraph();
    for(int i=1; i <= colNumber;i++)
    {
        createGraph();
        graph.last()->setChartTitle(QString("Vertical graph %1").arg(QString::number(i)));
        grid->addWidget(graph.last(),1, i);
    }
}

void UWatchTab::createGridLayout(int rowNumber, int colNumber)
{
    createGrid();
    deleteAllGraph();
    for(int i=1; i <= rowNumber;i++)
        for(int j=1; j <= colNumber; j++)
        {
            createGraph();
            graph.last()->setChartTitle(QString("Grid graph %1").arg(QString::number((i-1)*colNumber+j)));
            grid->addWidget(graph.last(),i, j);
        }
}

UWatchChart *UWatchTab::getChart(int index)
{
    return graph[index];
}


int UWatchTab::countGraphs()
{
    return graph.count();
}

void UWatchTab::createSelectionDialog(int chartIndex)
{
    int channelIndex=0;
    QString componentName;
    QString componentProperty;

    //почему то не рабоатет если раскоментить(
    //if(!application)
    //    return;

    //создаем окно для выбора источника данных
    UComponentPropertySelectionWidget dialog(this, 3,application);
    dialog.show();
    if (dialog.exec())
    {
         channelIndex = dialog.componentsList->getSelectedChannelIndex();
         componentName = dialog.componentsList->getSelectedComponentLongName();
         componentProperty = dialog.componentsList->getSelectedPropertyName();
    }

    //создаем серию для выбранного источника
    graph[chartIndex]->createSerie(channelIndex, componentName, componentProperty, "type", 0, 0);
}

void UWatchTab::saveUpdateInterval(int newInterval)
{
    UpdateIntervalMs = newInterval;
    UpdateInterval = newInterval;
}

void UWatchTab::createGrid()
{
    //создаем сетку для графиков
    ///рабоатет криво - опправить
    if(!ui->horizontalLayout->isEmpty())
    {
        ui->horizontalLayout->removeItem(grid);
        delete grid;
    }

    grid = new QGridLayout(this);
    grid->setSpacing(0);
    ui->horizontalLayout->addLayout(grid);
    grid->update();
}
