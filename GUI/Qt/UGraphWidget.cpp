#include "UGraphWidget.h"
#include "ui_UGraphWidget.h"
#include "UGraphControlDialog.h"
#include "UGuiTelemetry.h"
#include <iostream>


namespace {
const int kDefaultGraphBuffer = 4096;
}

void UGraphWidget::GraphSeriesBuffer::Append(double xValue, double yValue, int capacity)
{
    if (capacity <= 0)
        capacity = kDefaultGraphBuffer;

    if (X.size() >= capacity) {
        const int removeCount = X.size() - capacity + 1;
        X.remove(0, removeCount);
        Y.remove(0, removeCount);
    }

    X.append(xValue);
    Y.append(yValue);
}

UGraphWidget::UGraphWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent,app),
    ui(new Ui::UGraphWidget)
{
    setAccessibleName("UGraphWidget");

    UpdateInterval = 30;
    lableX = "time";
    lableY = "Selected Output";

    ui->setupUi(this);

    graphPainter = new UGraphPaintWidget(this);
    ui->verticalLayoutGraph->addWidget(graphPainter);

    //Добавление действий по правой кнопке
    addAction(ui->actionSelectOutput);
    addAction(ui->actionChangeCurrenItem);
    addAction(ui->actionDeleteAll);
    addAction(ui->actionDeleteCurrentItem);
    addAction(ui->actionSettings);

    //Соединение действия выше и слотав котором выполняется действие, кпо щелчку
    connect(ui->actionSelectOutput, SIGNAL(triggered()), this, SLOT(slotActionSelectOutput()));
    connect(ui->actionChangeCurrenItem, SIGNAL(triggered()), graphPainter, SLOT(changeCurrentItem()));
    connect(ui->actionDeleteAll, SIGNAL(triggered()), graphPainter, SLOT(delAllGraph()));
    connect(ui->actionDeleteCurrentItem, SIGNAL(triggered()), this, SLOT(slotActionDeleteCurrentItem()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(slotActionSettings()));




    // Если нажали на кнопку - появляется сигнал
    //connect(ui->drawOneGraph, SIGNAL(pressed()), this, SIGNAL(drawSmth()));
    //connect(ui->delAllButton, SIGNAL(pressed()), this, SIGNAL(delAllButtonSignal()));
    //connect(ui->changeColor, SIGNAL(pressed()), this, SIGNAL(changeColorSignal()));
    //connect(ui->changeCurrentItem, SIGNAL(pressed()), this, SIGNAL(changeCurrentItemSignal()));

    connect(this, SIGNAL(changeCurrentItemSignal()), graphPainter, SLOT(changeCurrentItem()));

    //Изначально не нужно изменять правую границу графика
    //Пока флаг не изменится
    flagUpdateBordersX=1;

    flagUpdateBordersMaxY=1;
    flagUpdateBordersMinY=1;
    // Задаем интервал слежения
    lastNElements = 0;

    double leftLimit = -1; //Начало интервала, где рисуем график по оси Ox
    double rightLimit =  12; //Конец интервала, где рисуем график по оси Ox
    double lowerLimit = -2; //Начало интервала, где рисуем график по оси Ox
    double upperLimit =  2; //Конец интервала, где рисуем график по оси Ox
    graphPainter->mainStartGraphSettings (leftLimit,rightLimit, lowerLimit, upperLimit,
                                          lableX, lableY);

}

UGraphWidget::~UGraphWidget()
{
    delete ui;
}

void UGraphWidget::addDataToGraph(size_t id, std::vector<double>  X1, std::vector<double> Y1)
{
    ensureBuffer(id);
    const int capacity = maxBufferLength();
    for (size_t j = 0; j < X1.size() && j < Y1.size(); ++j) {
        SeriesBuffers[id].Append(X1[j], Y1[j], capacity);
    }
    graphPainter->updateSeries(int(id), SeriesBuffers[id].X, SeriesBuffers[id].Y);
}

void UGraphWidget::addDataToGraph(size_t id, double X1, double Y1)
{
    ensureBuffer(id);
    SeriesBuffers[id].Append(X1, Y1, maxBufferLength());
    graphPainter->updateSeries(int(id), SeriesBuffers[id].X, SeriesBuffers[id].Y);
}

void UGraphWidget::ensureBuffer(size_t index)
{
    if (SeriesBuffers.size() <= index) {
        SeriesBuffers.resize(index + 1);
    }
}

int UGraphWidget::maxBufferLength() const
{
    if (lastNElements > 0)
        return lastNElements;
    return kDefaultGraphBuffer;
}

void UGraphWidget::mainGraphSettings(double leftLimit, double rightLimit, double lowerLimit, double upperLimit, const QString &nameX, const QString &nameY)
{
    graphPainter->mainStartGraphSettings(leftLimit,rightLimit,lowerLimit,upperLimit,nameX,nameY);
}

int UGraphWidget::addGraphParameters(const std::string &graphName, int myColor)
{
    int a = graphPainter->addGraphVisualParameters(graphName, myColor);
    return a;
}

int UGraphWidget::addGraphParameters(const std::string &graphName)
{
    int a = graphPainter->addGraphVisualParameters(graphName);
    return a;
}

void UGraphWidget::setGraphDataSource(int graph_index, int channel_index,
                                      const std::string &componentName, const std::string &propertyName,
                                      const std::string &type, int jx, int jy)
{
    graphPainter->setGraphDataSource(graph_index, channel_index, componentName, propertyName, type, jx, jy);
}

void UGraphWidget::setCurrentItem(int myCurrentItem)
{
    graphPainter->setCurrentItem(myCurrentItem);
}

void UGraphWidget::AUpdateInterface()
{
    NMSDK::UGuiTelemetryScope telemetry(QStringLiteral("UGraphWidget.Update"), accessibleName());
    RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock<RDK::UNet>();
    if(!model)
        return;

    const int seriesCount = graphPainter->getSize();
    const double xValue = Model_GetDoubleRealTime();

    for(int i=0; i<seriesCount; i++)
    {
        ensureBuffer(size_t(i));
        const TSingleGraph& current_graph=graphPainter->getGraph(i);

        bool available = false;
        double y=0.0;

        RDK::UEPtr<RDK::UContainer> component=model->GetComponentL(current_graph.nameComponent,true);
        if(component)
        {
            RDK::MDMatrix<double>* m=component->AccessPropertyData<RDK::MDMatrix<double> >(current_graph.nameProperty);
            if(m && m->GetCols()>current_graph.Jx && m->GetRows()>current_graph.Jy)
            {
                y=(*m)(current_graph.Jy,current_graph.Jx);
                available = true;
            }
        }

        graphPainter->setGraphAvailability(i, available);
        if(!available)
            continue;

        SeriesBuffers[i].Append(xValue, y, maxBufferLength());
        graphPainter->updateSeries(i, SeriesBuffers[i].X, SeriesBuffers[i].Y);

        if (flagUpdateBordersX>0)
        {
            if (lastNElements>0)
            {
                graphPainter->setRightLimitGraph(xValue);
                graphPainter->setLeftLimitGraph(xValue-lastNElements);
            }
            else
            {
                graphPainter->setRightLimitGraph(xValue + 0.1 * xValue);
            }
        }

        if (flagUpdateBordersMaxY>0)
        {
            if ((1.1*y)>graphPainter->getUpperLimitGraph())
                graphPainter->setUpperLimitGraph(y+0.1*y);
        }
        if (flagUpdateBordersMinY>0)
        {
            if ((1.1*y)<graphPainter->getLowerLimitGraph())
                graphPainter->setLowerLimitGraph(y+0.1*y);
        }
    }
    graphPainter->commitFrame();
}

UGraphPaintWidget *UGraphWidget::getGraphPainter() const
{
    return graphPainter;
}


void UGraphWidget::ASaveParameters(RDK::USerStorageXML &xml)
{
    SaveXmlSnapshot(xml);
    SaveLegacySettings();
}

void UGraphWidget::ALoadParameters(RDK::USerStorageXML &xml)
{
    bool loaded = LoadFromXml(xml);
    if (!loaded) {
        loaded = LoadLegacySettings();
    } else {
        // Держим legacy-настройки в актуальном виде для старых конфигураций.
        SaveLegacySettings();
    }

    if (!loaded) {
        graphPainter->delAllGraph();
        SeriesBuffers.clear();
    }
}

void UGraphWidget::slotActionSelectOutput()
{
    std::cout<<"selectDir_clicked"<<std::endl;
    /*/// Экзепляр класса приложения
    RDK::UApplication *application;
    /// true - во время выполнения AUpdateInterface, все остальное время - false
    /// Устраянет ошибку рекурсивного обновления интерфейса,
    /// появляющуюся в случае попытки одновременного обновления по требованию интерфейса и по тику ядра.*/
    if(!application)
        return;
    UComponentPropertySelectionWidget dialog(this, 3, application);
    //dialog.componentsList->setChannelsListVisible(ui->checkBoxIndChannels->isChecked());
    //if (dialog.exec() && selectedImage)
    if (dialog.exec())
    {
        //Заполнение одного элемента вектора - структуры
        int graph_index=graphPainter->addGraphVisualParameters("first",Qt::green);
        graphPainter->setGraphDataSource(graph_index,dialog.componentsList->getSelectedChannelIndex(),dialog.componentsList->getSelectedComponentLongName().toLocal8Bit().data(),dialog.componentsList->getSelectedPropertyName().toLocal8Bit().data(),"type",0,0);

        /*selectedImage->setComponentName(dialog.componentsList->getSelectedComponentLongName());
        selectedImage->setComponentPropertyName(dialog.componentsList->getSelectedPropertyName());
        selectedImage->setCalcChannel(dialog.componentsList->getSelectedChannelIndex());*/
    }
    dialog.writeSettings(QString::fromLocal8Bit(
                           application->GetProjectPath().c_str())+"settings.qt");

}

void UGraphWidget::slotActionDeleteCurrentItem()
{
    int a = graphPainter->delCurrentItemGraph();
    if(a>=0)
    {
        if (SeriesBuffers.size() > static_cast<size_t>(a)) {
            SeriesBuffers.erase(SeriesBuffers.begin() + a);
        }
        graphPainter->commitFrame();
    }
}

void UGraphWidget::slotActionSettings()
{
    std::cout<<"call dialod cliced"<<std::endl;
    if(!application)
        return;

    //UGraphControlDialog settingsDialog(this, flagUpdateBordersX, 1);
    UGraphControlDialog settingsDialog(this);
    settingsDialog.setAxisXChangeData(flagUpdateBordersX);
    settingsDialog.setAxisMaxYChangeData(flagUpdateBordersMaxY);
    settingsDialog.setAxisMinYChangeData(flagUpdateBordersMinY);
    settingsDialog.setDecimalPlaces(1);
    settingsDialog.setDisplayInterval(lastNElements);
    settingsDialog.setLableX(lableX);
    settingsDialog.setLableY(lableY);
    if (settingsDialog.exec())
    {
        lastNElements = settingsDialog.getDoubleSpinBoxData();
        flagUpdateBordersX = settingsDialog.getAxisXChangeData();
        flagUpdateBordersMaxY = settingsDialog.getAxisMaxYChangeData();
        flagUpdateBordersMinY = settingsDialog.getAxisMinYChangeData();
        lableX = settingsDialog.getLableX();
        lableY = settingsDialog.getLableY();
        graphPainter->setLables(lableX, lableY);
        int color = settingsDialog.getColor();
        graphPainter->setColorCurrentItem(color);

        if(settingsDialog.getDelInf())
        {
            int a = graphPainter->delCurrentItemGraph();
            if(a>=0)
            {
                if (SeriesBuffers.size() > static_cast<size_t>(a)) {
                    SeriesBuffers.erase(SeriesBuffers.begin() + a);
                }
                graphPainter->commitFrame();
            }
        }
    }

}

void UGraphWidget::SaveLegacySettings() const
{
    if (!application)
        return;

    QSettings settings(QString::fromLocal8Bit(application->GetProjectPath().c_str()) + "settings.qt",
                       QSettings::IniFormat);

    settings.beginGroup(accessibleName());
    settings.setValue("geometry", saveGeometry());
    settings.setValue("int_flagUpdateBorders", flagUpdateBordersX);
    settings.setValue("QString_lableX", lableX);
    settings.setValue("Qstring_lableY", lableY);
    settings.setValue("int_currentItem", graphPainter->getCurrentItem());
    settings.setValue("double_leftLimitGraph", graphPainter->getLeftLimitGraph());
    settings.setValue("double_rightLimitGraph", graphPainter->getRightLimitGraph());
    settings.setValue("double_lowerLimitGraph", graphPainter->getLowerLimitGraph());
    settings.setValue("double_upperLimitGraph", graphPainter->getUpperLimitGraph());
    settings.setValue("int_sizeVectorStruct", graphPainter->getSize());

    for (int j = 0; j < graphPainter->getSize(); ++j)
    {
        int graphColor = 0;
        std::string graphName;
        int indexChannel = -1;
        std::string nameComponent;
        std::string nameProperty;
        std::string typeProperty;
        int Jx = 0;
        int Jy = 0;

        indexChannel = graphPainter->getStructContent(j,
                                                      &graphColor,
                                                      &graphName,
                                                      &nameComponent,
                                                      &nameProperty,
                                                      &typeProperty,
                                                      &Jx,
                                                      &Jy);

        settings.setValue(
            "vectorStruct" + QString::number(j),
            QString::number(indexChannel) + "*" + QString::fromStdString(nameComponent) + "*" +
            QString::fromStdString(nameProperty) + "*" + QString::fromStdString(typeProperty) + "*" +
            QString::number(Jx) + "*" + QString::number(Jy) + "*" + QString::fromStdString(graphName) +
            "*" + QString::number(graphColor));
    }
    settings.endGroup();
}

bool UGraphWidget::LoadLegacySettings()
{
    if (!application)
        return false;

    QSettings settings(QString::fromLocal8Bit(application->GetProjectPath().c_str()) + "settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());

    restoreGeometry(settings.value("geometry").toByteArray());
    flagUpdateBordersX = settings.value("int_flagUpdateBorders", flagUpdateBordersX).toInt();
    lableX = settings.value("QString_lableX", lableX).toString();
    lableY = settings.value("QString_lableY", lableY).toString();

    graphPainter->mainStartGraphSettings(settings.value("double_leftLimitGraph", -1).toDouble(),
                                         settings.value("double_rightLimitGraph", 6).toDouble(),
                                         settings.value("double_lowerLimitGraph", -2).toDouble(),
                                         settings.value("double_upperLimitGraph", 2).toDouble(),
                                         lableX,
                                         lableY);

    const int currentItem = settings.value("int_currentItem", 0).toInt();
    const int sizeVectorStruct = settings.value("int_sizeVectorStruct", -1).toInt();

    graphPainter->delAllGraph();
    SeriesBuffers.clear();

    if (sizeVectorStruct <= 0) {
        settings.endGroup();
        return false;
    }

    for (int j = 0; j < sizeVectorStruct; ++j)
    {
        const QString source = settings.value("vectorStruct" + QString::number(j)).toString();
        if (source.isEmpty())
            continue;

        const QStringList list = source.split("*");
        if (list.size() != 8)
            continue;

        const int indexChannel = list.at(0).toInt();
        const std::string nameComponent = list.at(1).toLocal8Bit().constData();
        const std::string nameProperty = list.at(2).toLocal8Bit().constData();
        const std::string typeProperty = list.at(3).toLocal8Bit().constData();
        const int Jx = list.at(4).toInt();
        const int Jy = list.at(5).toInt();
        const std::string graphName = list.at(6).toLocal8Bit().constData();
        const int graphColor = list.at(7).toInt();

        const int graphIndex = graphPainter->addGraphVisualParameters(graphName, graphColor);
        graphPainter->setGraphDataSource(graphIndex,
                                         indexChannel,
                                         nameComponent,
                                         nameProperty,
                                         typeProperty,
                                         Jx,
                                         Jy);
    }

    graphPainter->setCurrentItem(currentItem);
    graphPainter->setLables(lableX, lableY);
    SeriesBuffers.resize(static_cast<size_t>(graphPainter->getSize()));

    settings.endGroup();
    return graphPainter->getSize() > 0;
}

void UGraphWidget::SaveXmlSnapshot(RDK::USerStorageXML &xml) const
{
    xml.DelNodeInternalContent();
    xml.WriteInteger("GridColCount", 1);
    xml.WriteInteger("GridRowCount", 1);
    xml.WriteInteger("GraphCount", 1);
    xml.WriteInteger("flagUpdateBordersX", flagUpdateBordersX);
    xml.WriteInteger("flagUpdateBordersMaxY", flagUpdateBordersMaxY);
    xml.WriteInteger("flagUpdateBordersMinY", flagUpdateBordersMinY);
    xml.WriteInteger("lastNElements", lastNElements);
    xml.WriteString("LabelX", lableX.toStdString());
    xml.WriteString("LabelY", lableY.toStdString());
    xml.WriteInteger("CurrentItem", graphPainter->getCurrentItem());

    xml.SelectNodeForce("graph_0");
    xml.WriteString("ChartTitle", "GraphWindow");
    xml.WriteString("AxisXName", lableX.toStdString());
    xml.WriteString("AxisYName", lableY.toStdString());
    xml.WriteFloat("AxisXmin", graphPainter->getLeftLimitGraph());
    xml.WriteFloat("AxisXmax", graphPainter->getRightLimitGraph());
    xml.WriteFloat("AxisYmin", graphPainter->getLowerLimitGraph());
    xml.WriteFloat("AxisYmax", graphPainter->getUpperLimitGraph());
    xml.WriteFloat("AxisXrange", graphPainter->getRightLimitGraph() - graphPainter->getLeftLimitGraph());

    const int graphsCount = graphPainter->getSize();
    xml.WriteInteger("SeriesCount", graphsCount);

    for (int i = 0; i < graphsCount; ++i)
    {
        const TSingleGraph &graph = graphPainter->getGraph(i);
        xml.SelectNodeForce("serie_" + RDK::sntoa(i));
        xml.WriteString("SerieName", graph.t_graphName);
        xml.WriteInteger("SerieWidth", 1);
        xml.WriteInteger("SerieLineType", 1);
        xml.WriteInteger("SerieColor", graph.t_graphColor.rgb());
        xml.WriteString("SerieNameComponent", graph.nameComponent);
        xml.WriteString("SerieNameProperty", graph.nameProperty);
        xml.WriteInteger("SerieJx", graph.Jx);
        xml.WriteInteger("SerieJy", graph.Jy);
        xml.WriteInteger("SerieChannel", graph.indexChannel);
        xml.WriteFloat("SerieYShift", 0.0);
        xml.SelectUp();
    }

    xml.SelectUp(); // graph_0
}

bool UGraphWidget::LoadFromXml(RDK::USerStorageXML &xml)
{
    int graphNodesCount = xml.ReadInteger("GraphCount", -1);
    const bool hasSeriesAtRoot = xml.ReadInteger("SeriesCount", -1) >= 0;

    if (graphNodesCount <= 0 && !hasSeriesAtRoot) {
        return false;
    }

    flagUpdateBordersX = xml.ReadInteger("flagUpdateBordersX", flagUpdateBordersX);
    flagUpdateBordersMaxY = xml.ReadInteger("flagUpdateBordersMaxY", flagUpdateBordersMaxY);
    flagUpdateBordersMinY = xml.ReadInteger("flagUpdateBordersMinY", flagUpdateBordersMinY);
    lastNElements = xml.ReadInteger("lastNElements", lastNElements);
    lableX = QString::fromStdString(xml.ReadString("LabelX", lableX.toStdString()));
    lableY = QString::fromStdString(xml.ReadString("LabelY", lableY.toStdString()));

    graphPainter->delAllGraph();
    SeriesBuffers.clear();

    const auto loadGraphNode = [this](RDK::USerStorageXML &node, bool configureAxis) -> bool
    {
        if (configureAxis)
        {
            const QString axisXName = QString::fromStdString(node.ReadString("AxisXName", lableX.toStdString()));
            const QString axisYName = QString::fromStdString(node.ReadString("AxisYName", lableY.toStdString()));
            const double axisXmin = node.ReadFloat("AxisXmin", graphPainter->getLeftLimitGraph());
            const double axisXmax = node.ReadFloat("AxisXmax", graphPainter->getRightLimitGraph());
            const double axisYmin = node.ReadFloat("AxisYmin", graphPainter->getLowerLimitGraph());
            const double axisYmax = node.ReadFloat("AxisYmax", graphPainter->getUpperLimitGraph());
            graphPainter->mainStartGraphSettings(axisXmin, axisXmax, axisYmin, axisYmax, axisXName, axisYName);
        }

        const int seriesCount = node.ReadInteger("SeriesCount", 0);
        bool added = false;
        for (int serieIndex = 0; serieIndex < seriesCount; ++serieIndex)
        {
            node.SelectNodeForce("serie_" + RDK::sntoa(serieIndex));
            const QString component = QString::fromStdString(node.ReadString("SerieNameComponent", ""));
            const QString property = QString::fromStdString(node.ReadString("SerieNameProperty", ""));
            if (component.isEmpty() || property.isEmpty())
            {
                node.SelectUp();
                continue;
            }

            const QString serieName = QString::fromStdString(node.ReadString("SerieName", ""));
            const QColor color = QColor::fromRgb(node.ReadInteger("SerieColor", QColor(Qt::green).rgb()));
            const int jx = node.ReadInteger("SerieJx", 0);
            const int jy = node.ReadInteger("SerieJy", 0);
            const int channel = node.ReadInteger("SerieChannel", 0);

            const int graphIndex = graphPainter->addGraphVisualParameters(serieName.toStdString(), color);
            graphPainter->setGraphDataSource(graphIndex,
                                             channel,
                                             component.toStdString(),
                                             property.toStdString(),
                                             "type",
                                             jx,
                                             jy);
            added = true;
            node.SelectUp();
        }
        return added;
    };

    bool configuredAxis = false;
    bool anyLoaded = false;

    if (graphNodesCount <= 0)
        graphNodesCount = 1;

    for (int graphIndex = 0; graphIndex < graphNodesCount; ++graphIndex)
    {
        if (xml.SelectNode("graph_" + RDK::sntoa(graphIndex)))
        {
            anyLoaded |= loadGraphNode(xml, !configuredAxis);
            configuredAxis = true;
            xml.SelectUp();
        }
        else if (hasSeriesAtRoot && graphIndex == 0)
        {
            anyLoaded |= loadGraphNode(xml, true);
            configuredAxis = true;
        }
    }

    graphPainter->setCurrentItem(xml.ReadInteger("CurrentItem", 0));
    graphPainter->setLables(lableX, lableY);
    SeriesBuffers.resize(static_cast<size_t>(graphPainter->getSize()));

    return anyLoaded && graphPainter->getSize() > 0;
}

