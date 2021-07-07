#include "UWatchTab.h"
#include "ui_UWatchTab.h"



UWatchTab::UWatchTab(QWidget *parent, RDK::UApplication* app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UWatchTab)
{
    ui->setupUi(this);
    colSplitter = nullptr;
    //создаем один график на вкладке
    createGridLayout(1,1);

    //время обновления графика
    UpdateInterval = UpdateIntervalMs;
    setAccessibleName("UWatchTab");
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
        graph[graphIndex]->chartView->setUpdatesEnabled(false);
        double x_min=0.0;
        double x_max=0.0;
        for (int serieIndex=0; serieIndex < graph[graphIndex]->countSeries(); serieIndex++)
        {

            std::list<double>::iterator buffIX, buffIY;
            ReadSeriesDataSafe(graphIndex,serieIndex,XData,YData);

            std::list<double>::iterator itx, ity;
            points.resize(int(XData.size()));

            UWatchSerie * current_serie = graph[graphIndex]->getSerie(serieIndex);

            int i = 0;
            for (itx = XData.begin(), ity = YData.begin(); itx != XData.end(); ++itx, ++ity)
            {
                points[i] = QPointF(*itx,*ity + current_serie->YShift);
                i++;
            }

            current_serie->replace(points);

            if(XData.empty())
            {
                x_min = 0;
                x_max = graph[graphIndex]->axisXrange;
            }
            else
            {
                x_min = XData.front();
                x_max = graph[graphIndex]->axisXrange + x_min;
                if(XData.back()-XData.front() > graph[graphIndex]->axisXrange)
                    graph[graphIndex]->axisXrange = XData.back()-XData.front();
            }
        }
        if(x_max-x_min > 0.001)
        {
            graph[graphIndex]->setAxisXmax(x_max);
            graph[graphIndex]->setAxisXmin(x_min);
        }
        else
        {
            // Если есть серии
            if(graph[graphIndex]->countSeries())
                graph[graphIndex]->updateTimeIntervals(1);
        }
        graph[graphIndex]->chartView->setUpdatesEnabled(true);
    }
}

/// Безопасно считывает данные серии из ядра
void UWatchTab::ReadSeriesDataSafe(int graphIndex, int serieIndex, std::list<double> &xdata, std::list<double> &ydata)
{
    RDK::UELockPtr<RDK::UEnvironment> env=RDK::GetEnvironmentLock();
    RDK::UControllerDataReader* data=env->GetDataReader(graph[graphIndex]->getSerie(serieIndex)->nameComponent.toStdString(),
                                                        graph[graphIndex]->getSerie(serieIndex)->nameProperty.toStdString(),
                                                        graph[graphIndex]->getSerie(serieIndex)->Jx,
                                                        graph[graphIndex]->getSerie(serieIndex)->Jy);
    if(!data)
    {
     xdata.clear();
     ydata.clear();
     return;
    }
    xdata=data->XData;
    ydata=data->YData;
}

void UWatchTab::createSelectionDialogSlot(int index)
{
    createSelectionDialog(index);
}

void UWatchTab::seriesOptionTriggered()
{
    seriesOption = new UWatchSeriesOption(this);
    seriesOption->setWindowTitle("Series option");
    //seriesOption->setModal(true);
    seriesOption->show();
}

void UWatchTab::chartsOptionTriggered()
{
    chartOption = new UWatchChartOption(this);
    chartOption->setModal(true);
    chartOption->setWindowTitle("Charts option");
    chartOption->show();
}

void UWatchTab::createSplitterGrid(int rowNumber)
{
    //создаем вертикальный контейнер, в котором располагаются горизонтальные

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

void UWatchTab::deleteGraphs(int new_graph_count)
{
    int graphs_to_remove = countGraphs() - new_graph_count;

    if(graphs_to_remove < 0)
    {
        graphs_to_remove = 0;
    }

    // deleting unnecessary graphs
    for(int i=0; i < graphs_to_remove; i++)
        delete graph.takeLast();


    //удаляем все графики
    for (int i = tabRowNumber-1; i >= 0; --i)
    {
        int widget_count = rowSplitter[i]->count();
        // clear children in rowSplitter, so while it is deleted, graphs won't be deleted
        for(int j = 0; j < widget_count; j++)
            rowSplitter[i]->widget(0)->setParent(nullptr);

        delete rowSplitter.takeLast();
    }

    //удаляем расположение
    if (colSplitter !=nullptr)
    {
        ui->horizontalLayout->removeWidget(colSplitter);
        delete colSplitter;
    }
}



void UWatchTab::createGridLayout(int rowNumber, int colNumber)
{

    deleteGraphs(rowNumber*colNumber);

    tabColNumber=colNumber;
    tabRowNumber=rowNumber;

    createSplitterGrid(rowNumber);


    // create graphs if needed
    while(countGraphs() < tabColNumber*tabRowNumber)
    {
        createGraph();
    }

    int k = 0;
    for(int i=0; i < rowNumber;i++)
    {
        for(int j=0; j < colNumber; j++)
        {

            //createGraph();
            graph[k]->setChartTitle(QString("Grid graph %1").arg(QString::number((i)*colNumber+j+1)));

            rowSplitter[i]->addWidget(graph[k]);
            k++;
        }
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

    //проверяем что у выбран не пустой элемент (если нет модели)
    if(!componentName.isEmpty() && !componentProperty.isEmpty())
    {
        bool is_int_or_double = false;

        {
            RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock<RDK::UNet>();

            RDK::UContainer *cont = model->GetComponentL(componentName.toStdString());
            if(!cont)
                return;

            RDK::UEPtr<RDK::UIProperty> prop=cont->FindProperty(componentProperty.toStdString());
            if(!prop)
                return;

            // Если тип double или int
            if(prop->GetLanguageType() == typeid(double) || prop->GetLanguageType() == typeid(int))
            {
                is_int_or_double = true;
            }
        }

        // Если тип double или int
        if(is_int_or_double)
        {
            //создаем серию для выбранного источника
            double time_interval = graph[channelIndex]->getAxisXmax() - graph[channelIndex]->getAxisXmin();
            graph[chartIndex]->createSerie(channelIndex, componentName, componentProperty, "type", 0, 0, time_interval, 0.0);
            return;
        }

        // Если тип, где надо выбрать ячейку (ряд и колонку)
        // MDMatrix<double>   MDMatrix<int>   MDVector<double>   MDVector<int>
        UMatrixFormDialog* form = new UMatrixFormDialog();
        form->SelectMatrix(componentName.toStdString(),componentProperty.toStdString());

        if(form->exec()== QDialog::Accepted)
        {
            if(form->SelectedRows.empty() || form->SelectedCols.empty())
             {
                form->SelectedRows = {0};
                form->SelectedCols = {0};
            }

            //создаем серии для выбранного источника
            double time_interval = graph[channelIndex]->getAxisXmax() - graph[channelIndex]->getAxisXmin();
            for(int i = 0; i < form->SelectedRows.size(); i++)
                graph[chartIndex]->createSerie(channelIndex, componentName, componentProperty, "type", form->SelectedRows[i], form->SelectedCols[i], time_interval, 0.0);
        }
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


// Сохраняет параметры интерфейса в xml
void UWatchTab::ASaveParameters(RDK::USerStorageXML &xml)
{
    xml.DelNodeInternalContent();
    xml.WriteInteger("GridColCount", tabColNumber);
    xml.WriteInteger("GridRowCount", tabRowNumber);
    xml.WriteInteger("GraphCount", countGraphs());

    // Пробегаем по списку всех открытых графов и серий в них
    for (int graphIndex=0; graphIndex < countGraphs(); graphIndex++)
    {
        xml.SelectNodeForce("graph_"+RDK::sntoa(graphIndex));

        xml.WriteString ("ChartTitle",      graph[graphIndex]->getChartTitle().toStdString());
        xml.WriteString ("AxisXName",       graph[graphIndex]->getAxisXName().toStdString());
        xml.WriteString ("AxisYName",       graph[graphIndex]->getAxisYName().toStdString());
        xml.WriteFloat  ("AxisXmin",        graph[graphIndex]->getAxisXmin());
        xml.WriteFloat  ("AxisXmax",        graph[graphIndex]->getAxisXmax());
        xml.WriteFloat  ("AxisYmin",        graph[graphIndex]->getAxisYmin());
        xml.WriteFloat  ("AxisYmax",        graph[graphIndex]->getAxisYmax());

        xml.WriteInteger("SeriesCount",     graph[graphIndex]->countSeries());

        for(int serieIndex=0; serieIndex<graph[graphIndex]->countSeries(); serieIndex++)
        {
            xml.SelectNodeForce("serie_"+RDK::sntoa(serieIndex));
            xml.WriteString ("SerieName",       graph[graphIndex]->getSerieName(serieIndex).toStdString());
            xml.WriteInteger("SerieWidth",      graph[graphIndex]->getSerieWidth(serieIndex));
            xml.WriteInteger("SerieLineType",   graph[graphIndex]->getSerieLineType(serieIndex));
            xml.WriteInteger("SerieColor",      graph[graphIndex]->getSerieColor(serieIndex).rgb());

            xml.WriteString ("SerieNameComponent",  graph[graphIndex]->getSerie(serieIndex)->nameComponent.toStdString());
            xml.WriteString ("SerieNameProperty",   graph[graphIndex]->getSerie(serieIndex)->nameProperty.toStdString());
            xml.WriteInteger("SerieJx",             graph[graphIndex]->getSerie(serieIndex)->Jx);
            xml.WriteInteger("SerieJy",             graph[graphIndex]->getSerie(serieIndex)->Jy);
            xml.WriteFloat  ("SerieYShift",         graph[graphIndex]->getSerie(serieIndex)->YShift);

            xml.SelectUp();
        }
        xml.SelectUp();
    }
}

// Загружает параметры интерфейса из xml
void UWatchTab::ALoadParameters(RDK::USerStorageXML &xml)
{
    int grid_cols = xml.ReadInteger("GridColCount", 1);
    int grid_rows = xml.ReadInteger("GridRowCount", 1);

    createGridLayout(grid_rows, grid_cols);

    int graph_count = xml.ReadInteger("GraphCount", 0);

    // ошибка в кол-ве созданных графов и указанных в xml-файле
    if(graph_count != countGraphs())
        return;

    for (int graphIndex=0; graphIndex < graph_count; graphIndex++)
    {
        xml.SelectNodeForce("graph_"+RDK::sntoa(graphIndex));

        graph[graphIndex]->setChartTitle (xml.ReadString ("ChartTitle",  "").c_str());
        graph[graphIndex]->setAxisXname  (xml.ReadString ("AxisXName",   "").c_str());
        graph[graphIndex]->setAxisYname  (xml.ReadString ("AxisYName",   "").c_str());

        graph[graphIndex]->setAxisYmin   (xml.ReadFloat  ("AxisYmin",    0));
        graph[graphIndex]->setAxisYmax   (xml.ReadFloat  ("AxisYmax",    0));

        graph[graphIndex]->axisXrange =  xml.ReadFloat("AxisXmax", 0) - xml.ReadFloat  ("AxisXmin", 0);
        graph[graphIndex]->setAxisXmin   (0.0);
        graph[graphIndex]->setAxisXmax   (graph[graphIndex]->axisXrange);
        graph[graphIndex]->fixInitialAxesState();
        int series_count = graph[graphIndex]->countSeries();
        for(int i = 0; i < series_count; i++)
            graph[graphIndex]->deleteSerie(0);

        series_count = xml.ReadInteger("SeriesCount", 0);

        for(int serieIndex=0; serieIndex < series_count; serieIndex++)
        {
            xml.SelectNodeForce("serie_"+RDK::sntoa(serieIndex));

            QString name_comp = xml.ReadString("SerieNameComponent", "").c_str();
            QString name_prop = xml.ReadString("SerieNameProperty", "").c_str();
            int jx = xml.ReadInteger("SerieJx", -1);
            int jy = xml.ReadInteger("SerieJy", -1);
            double y_shift = xml.ReadFloat("SerieYShift", 0.0);

            double time_interval = graph[graphIndex]->getAxisXmax() - graph[graphIndex]->getAxisXmin();
            graph[graphIndex]->createSerie(0, name_comp, name_prop, "type", jx, jy, time_interval, y_shift);
            graph[graphIndex]->setSerieName      (serieIndex, xml.ReadString("SerieName", "").c_str());
            graph[graphIndex]->setSerieWidth     (serieIndex, xml.ReadInteger("SerieWidth", 0));
            graph[graphIndex]->setSerieLineType  (serieIndex, static_cast<Qt::PenStyle>(xml.ReadInteger("SerieLineType", 0)));
            graph[graphIndex]->getSerie(serieIndex)->setColor(xml.ReadInteger("SerieColor", 0));
            xml.SelectUp();
        }
        xml.SelectUp();
    }
}
