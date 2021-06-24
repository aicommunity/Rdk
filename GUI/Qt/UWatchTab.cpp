#include "UWatchTab.h"
#include "ui_UWatchTab.h"



UWatchTab::UWatchTab(QWidget *parent, RDK::UApplication* app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UWatchTab)
{
    ui->setupUi(this);
    colSplitter = nullptr;
    //создаем один график на вкладке
    createSingleLayout();

    //врем€ обновлени€ графика
    UpdateInterval = UpdateIntervalMs;
}

UWatchTab::~UWatchTab()
{
    delete ui;
}

void UWatchTab::createGraph()
{
    //создание 1 графика
    graph.push_back(new UWatchChart(this));
    graph.last()->setChartIndex(graph.count()-1);

    connect(graph.last(), SIGNAL(addSerieSignal(int)), this, SLOT(createSelectionDialogSlot(int)));
}

void UWatchTab::deleteGraph(int index)
{
    delete graph[index];
    graph.remove(index);
}


void UWatchTab::AUpdateInterface()
{
    for (int graphIndex=0; graphIndex < graph.count(); graphIndex++)
    {
        double x_min;
        double x_max;
        RDK::UELockPtr<RDK::UEnvironment> env=RDK::GetEnvironmentLock();
        for (int serieIndex=0; serieIndex < graph[graphIndex]->countSeries(); serieIndex++)
        {

            std::list<double>::iterator buffIX, buffIY;
            RDK::UControllerDataReader* data=env->GetDataReader(graph[graphIndex]->getSerie(serieIndex)->nameComponent.toStdString(),
                                                                graph[graphIndex]->getSerie(serieIndex)->nameProperty.toStdString(),
                                                                graph[graphIndex]->getSerie(serieIndex)->Jx,
                                                                graph[graphIndex]->getSerie(serieIndex)->Jy);
            if(!data)
                continue;

            graph[graphIndex]->getSerie(serieIndex)->clear();

            std::list<double>::iterator itx, ity;
            QList<QPointF> points;
            for (itx = data->XData.begin(), ity = data->YData.begin(); itx != data->XData.end(); ++itx, ++ity)
            {
                points.push_back(QPointF(*itx,*ity));
            }

            graph[graphIndex]->getSerie(serieIndex)->replace(points);

            if(data->XData.empty())
            {
             x_max = 0;
             x_min = 0;
            }
            else
            {
             x_max = data->XData.back();
             x_min = data->XData.front();
            }
        }
        if(x_max-x_min > 0.001)
        {
            graph[graphIndex]->setAxisXmax(x_max);
            graph[graphIndex]->setAxisXmin(x_min);
        }
        else
        {
            graph[graphIndex]->updateTimeIntervals(1);
        }
    }
}

void UWatchTab::createSelectionDialogSlot(int index)
{
    createSelectionDialog(index);
}

void UWatchTab::createSplitterGrid(int rowNumber)
{
    //создаем вертикальный контейнер, в котором располагаютс€ горизонтальные

    colSplitter = new QSplitter(this);
    colSplitter->setOrientation(Qt::Vertical);

    for (int i = 0; i < rowNumber; ++i)
    {
        rowSplitter.push_back(new QSplitter(this));
        rowSplitter.last()->setOrientation(Qt::Horizontal);
        colSplitter->addWidget(rowSplitter.last());
    }
    ui->horizontalLayout->addWidget(colSplitter);
}

void UWatchTab::deleteAllGraph()
{
    //удал€ем все графики
    for (int i = tabRowNumber-1; i >= 0; --i)
    {
        for (int j = tabColNumber-1; j >= 0; --j)
        {
            delete graph.takeLast();
        }
        delete rowSplitter.takeLast();
    }
    //удал€ем расположение
    if (colSplitter !=nullptr)
    {
        ui->horizontalLayout->removeWidget(colSplitter);
        delete colSplitter;
    }
}


void UWatchTab::createSingleLayout()
{
   deleteAllGraph();
   tabColNumber=1;
   tabRowNumber=1;

   createGraph();
   graph.last()->setChartTitle("Single graph");

   createSplitterGrid(1);
   rowSplitter.last()->addWidget(graph.last());
   layoutMode = 0;
}

void UWatchTab::createRowLayout(int rowNumber)
{
    deleteAllGraph();
    tabColNumber=1;
    tabRowNumber=rowNumber;

    createSplitterGrid(rowNumber);
    for(int i=0; i < rowNumber;i++)
    {
        createGraph();
        graph.last()->setChartTitle(QString("Horizontal graph %1").arg(QString::number(i)));
        rowSplitter[i]->addWidget(graph.last());
    }
    layoutMode = 2;
}

void UWatchTab::createColLayout(int colNumber)
{
    deleteAllGraph();

    tabColNumber=colNumber;
    tabRowNumber=1;

    createSplitterGrid(1);

    for(int i=0; i < colNumber;i++)
    {
        createGraph();
        graph.last()->setChartTitle(QString("Vertical graph %1").arg(QString::number(i)));
        rowSplitter[0]->addWidget(graph.last());
    }
    layoutMode = 1;
}

void UWatchTab::createGridLayout(int rowNumber, int colNumber)
{

    deleteAllGraph();

    tabColNumber=colNumber;
    tabRowNumber=rowNumber;

    createSplitterGrid(rowNumber);
    for(int i=0; i < rowNumber;i++)
    {
        for(int j=0; j < colNumber; j++)
        {
            createGraph();
            graph.last()->setChartTitle(QString("Grid graph %1").arg(QString::number((i)*colNumber+j+1)));

            rowSplitter[i]->addWidget(graph.last());
        }
    }
    layoutMode = 3;
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

    //создаем окно дл€ выбора источника данных
    UComponentPropertySelectionWidget dialog(this, 3,application);
    dialog.show();
    if (dialog.exec())
    {
         channelIndex = dialog.componentsList->getSelectedChannelIndex();
         componentName = dialog.componentsList->getSelectedComponentLongName();
         componentProperty = dialog.componentsList->getSelectedPropertyName();
    }

    //провер€ем что у выбран не пустой элемент (если нет модели)
    if(!componentName.isEmpty() && !componentProperty.isEmpty())
    {
        UMatrixForm* form = new UMatrixForm();
        form->SelectMatrix(componentName.toStdString(),componentProperty.toStdString());
        form->show();

        // »з-за того, что это QWidget, а не QDialog, ждем закрыти€ окна самосто€телььно
        QEventLoop loop;
        connect(form, SIGNAL(destroyed()), &loop, SLOT(quit()));
        loop.exec();

        if(form->SelectedRow == -1 || form->SelectedCol == -1)
            return;

        //создаем серию дл€ выбранного источника
        double time_interval = graph[channelIndex]->getAxisXmax() - graph[channelIndex]->getAxisXmin();
        graph[chartIndex]->createSerie(channelIndex, componentName, componentProperty, "type", form->SelectedRow, form->SelectedCol, time_interval);
    }
}

void UWatchTab::saveUpdateInterval(int newInterval)
{
    UpdateIntervalMs = newInterval;
    UpdateInterval = newInterval;
}

int UWatchTab::getColNumber()
{
    return tabColNumber;
}

int UWatchTab::getRowNumber()
{
    return tabRowNumber;
}


// —охран€ет параметры интерфейса в xml
void UWatchTab::ASaveParameters(RDK::USerStorageXML &xml)
{
    xml.DelNodeInternalContent();
    xml.WriteInteger("GraphCount", countGraphs());
    // ѕробегаем по списку всех открытых серий
    for (int graphIndex=0; graphIndex < countGraphs(); graphIndex++)
    {
        xml.WriteString ("ChartTitle",      graph[graphIndex]->getChartTitle().toStdString());
        xml.WriteString ("AxisXName",       graph[graphIndex]->getAxisXName().toStdString());
        xml.WriteString ("AxisYName",       graph[graphIndex]->getAxisYName().toStdString());
        xml.WriteFloat  ("AxisXmin",        graph[graphIndex]->getAxisXmin());
        xml.WriteFloat  ("AxisXmax",        graph[graphIndex]->getAxisXmax());
        xml.WriteFloat  ("AxisYmin",        graph[graphIndex]->getAxisYmin());
        xml.WriteFloat  ("AxisYmax",        graph[graphIndex]->getAxisYmax());
        xml.WriteInteger("SeriesCount", graph[graphIndex]->countSeries());
        for(int serieIndex=0; serieIndex<graph[graphIndex]->countSeries(); serieIndex++)
        {
            xml.SelectNodeForce("serie_"+RDK::sntoa(serieIndex));
            xml.WriteString ("SerieName",       graph[graphIndex]->getSerieName(serieIndex).toStdString());
            xml.WriteInteger("SerieWidth",      graph[graphIndex]->getSerieWidth(serieIndex));
            xml.WriteInteger("SerieLineType",   graph[graphIndex]->getSerieLineType(serieIndex));
            xml.WriteInteger("SerieColor",      graph[graphIndex]->getSerieColor(serieIndex).rgb());

            xml.WriteString ("SerieNameComponent", graph[graphIndex]->getSerie(serieIndex)->nameComponent.toStdString());
            xml.WriteString ("SerieNameProperty", graph[graphIndex]->getSerie(serieIndex)->nameProperty.toStdString());
            xml.WriteInteger("SerieJx", graph[graphIndex]->getSerie(serieIndex)->Jx);
            xml.WriteInteger("SerieJy", graph[graphIndex]->getSerie(serieIndex)->Jy);


            xml.SelectUp();
        }
    }
}

// «агружает параметры интерфейса из xml
void UWatchTab::ALoadParameters(RDK::USerStorageXML &xml)
{
    int graph_count = xml.ReadInteger("GraphCount", 0);
    //createGraph();

    graph.last()->setChartTitle (xml.ReadString ("ChartTitle",  "").c_str());
    graph.last()->setAxisXname  (xml.ReadString ("AxisXName",   "").c_str());
    graph.last()->setAxisYname  (xml.ReadString ("AxisYName",   "").c_str());
    graph.last()->setAxisXmin   (xml.ReadFloat  ("AxisXmin",    0));
    graph.last()->setAxisXmax   (xml.ReadFloat  ("AxisXmax",    0));
    graph.last()->setAxisYmin   (xml.ReadFloat  ("AxisYmin",    0));
    graph.last()->setAxisYmax   (xml.ReadFloat  ("AxisYmax",    0));


    int series_count = graph.last()->countSeries();
    for(int i = 0; i < series_count; i++)
        graph.last()->deleteSerie(0);

    series_count = xml.ReadInteger("SeriesCount", 0);

    for(int serieIndex=0; serieIndex < series_count; serieIndex++)
    {
        xml.SelectNodeForce("serie_"+RDK::sntoa(serieIndex));

        QString name_comp = xml.ReadString("SerieNameComponent", "").c_str();
        QString name_prop = xml.ReadString("SerieNameProperty", "").c_str();
        int jx = xml.ReadInteger("SerieJx", -1);
        int jy = xml.ReadInteger("SerieJy", -1);

        double time_interval = graph.last()->getAxisXmax() - graph.last()->getAxisXmin();
        graph.last()->createSerie(0, name_comp, name_prop, "type", jx, jy, time_interval);

        graph.last()->setSerieName      (serieIndex, xml.ReadString("SerieName", "").c_str());
        graph.last()->setSerieWidth     (serieIndex, xml.ReadInteger("SerieWidth", 0));
        graph.last()->setSerieLineType  (serieIndex, static_cast<Qt::PenStyle>(xml.ReadInteger("SerieLineType", 0)));
        graph.last()->getSerie(serieIndex)->setColor(xml.ReadInteger("SerieColor", 0));
        xml.SelectUp();
    }
}
