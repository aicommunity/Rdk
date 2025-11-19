#include "UGraphWidget.h"
#include "ui_UGraphWidget.h"
#include "UGraphControlDialog.h"
#include <iostream>


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

    //���������� �������� �� ������ ������
    addAction(ui->actionSelectOutput);
    addAction(ui->actionChangeCurrenItem);
    addAction(ui->actionDeleteAll);
    addAction(ui->actionDeleteCurrentItem);
    addAction(ui->actionSettings);

    //���������� �������� ���� � ������ ������� ����������� ��������, ��� ������
    connect(ui->actionSelectOutput, SIGNAL(triggered()), this, SLOT(slotActionSelectOutput()));
    connect(ui->actionChangeCurrenItem, SIGNAL(triggered()), graphPainter, SLOT(changeCurrentItem()));
    connect(ui->actionDeleteAll, SIGNAL(triggered()), graphPainter, SLOT(delAllGraph()));
    connect(ui->actionDeleteCurrentItem, SIGNAL(triggered()), this, SLOT(slotActionDeleteCurrentItem()));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(slotActionSettings()));




    // ���� ������ �� ������ - ���������� ������
    //connect(ui->drawOneGraph, SIGNAL(pressed()), this, SIGNAL(drawSmth()));
    //connect(ui->delAllButton, SIGNAL(pressed()), this, SIGNAL(delAllButtonSignal()));
    //connect(ui->changeColor, SIGNAL(pressed()), this, SIGNAL(changeColorSignal()));
    //connect(ui->changeCurrentItem, SIGNAL(pressed()), this, SIGNAL(changeCurrentItemSignal()));

    // ������ ����������� �� ������
    connect(this, SIGNAL(transferDataSignal(int,QVector<double>,QVector<double>)),
            graphPainter, SLOT(setData(int,QVector<double>,QVector<double>)));
    //connect(this, SIGNAL(delAllButtonSignal()), graphPainter, SLOT(delAllGraph()));
    connect(this, SIGNAL(changeCurrentItemSignal()), graphPainter, SLOT(changeCurrentItem()));

    //���������� �� ����� �������� ������ ������� �������
    //���� ���� �� ���������
    flagUpdateBordersX=1;

    flagUpdateBordersMaxY=1;
    flagUpdateBordersMinY=1;
    // ������ �������� ��������
    lastNElements = 0;

    double leftLimit = -1; //������ ���������, ��� ������ ������ �� ��� Ox
    double rightLimit =  12; //����� ���������, ��� ������ ������ �� ��� Ox
    double lowerLimit = -2; //������ ���������, ��� ������ ������ �� ��� Ox
    double upperLimit =  2; //����� ���������, ��� ������ ������ �� ��� Ox
    graphPainter->mainStartGraphSettings (leftLimit,rightLimit, lowerLimit, upperLimit,
                                          lableX, lableY);

}

UGraphWidget::~UGraphWidget()
{
    delete ui;
}

void UGraphWidget::addDataToGraph(size_t id, std::vector<double>  X1, std::vector<double> Y1)
{
    //��������� ���� �� ������ � ������ id, ���� ���, �� ��������
    size_t startSizeMas=masX.size();
    if (startSizeMas<id+1)
    {
        masX.resize(id+1);
        masY.resize(id+1);
    }

    //�������� ������ �� ������
    size_t startSizeElem = masX[id].size();
    size_t addSizeElem = X1.size();
    masX[id].resize(int(startSizeElem+addSizeElem));
    masY[id].resize(int(startSizeElem+addSizeElem));

   //��������� ���������� �����
    for (size_t j=startSizeElem; j<startSizeElem+addSizeElem-1; j++)
    {
        masX[id][int(j)]=X1[j-startSizeElem];
        masY[id][int(j)]=Y1[j-startSizeElem];
    }
    emit transferDataSignal(int(id), masX[id], masY[id]);
}

void UGraphWidget::addDataToGraph(size_t id, double X1, double Y1)
{
    //��������� ���� �� ������ � ������ id, ���� ���, �� ��������
    size_t startSizeMas=masX.size();
    if (startSizeMas<id+1)
    {
        masX.resize(id+1);
        masY.resize(id+1);
    }

    //�������� �������� � ����� ������� �������
    masX[id].push_back(X1);
    masY[id].push_back(Y1);

    emit transferDataSignal(int(id), masX[id], masY[id]);
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
    int n=graphPainter->getSize();
    for(int i=0; i<n; i++)
    {
        const TSingleGraph& current_graph=graphPainter->getGraph(i);

        // ��������� � ����, ������� �������
        RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock<RDK::UNet>();
        //RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock();

        std::weak_ptr<RDK::UContainer> component_weak=model->GetComponentL(current_graph.nameComponent,true);
        if(component_weak.expired())
            continue;
        std::shared_ptr<RDK::UContainer> component=component_weak.lock();
        if(!component)
            continue;

        RDK::MDMatrix<double>* m=component->AccessPropertyData<RDK::MDMatrix<double> >(current_graph.nameProperty);
        if(!m)
            continue;

        if(m->GetCols()<=current_graph.Jx || m->GetRows()<=current_graph.Jy)
            continue;

        double y=(*m)(current_graph.Jy,current_graph.Jx);

        double x=Model_GetDoubleRealTime();

        // �� ������ ������� �� ���� ������
        addDataToGraph(i,x,y);
        //��������� ����� �� �������� ������� �� ��� �
        if (flagUpdateBordersX>0)
        {
            if (lastNElements>0)
            {
                graphPainter->setRightLimitGraph(x);
                graphPainter->setLeftLimitGraph(x-lastNElements);
            }
            else
                graphPainter->setRightLimitGraph(x+0.1*x);
        }
        //��������� ����� �� �������� ������� �� ��� Y
        if (flagUpdateBordersMaxY>0)
        {
            //������������ �������
            if ((1.1*y)>graphPainter->getUpperLimitGraph())
                graphPainter->setUpperLimitGraph(y+0.1*y);
            //std::cout<<graphPainter->getUpperLimitGraph()<<"   "<<y<<std::endl;
        }
        if (flagUpdateBordersMinY>0)
        {
            //����������� �������
            if ((1.1*y)<graphPainter->getLowerLimitGraph())
                graphPainter->setLowerLimitGraph(y+0.1*y);
        }

    }
    graphPainter->redrawGraph();
}

UGraphPaintWidget *UGraphWidget::getGraphPainter() const
{
    return graphPainter;
}


void UGraphWidget::ASaveParameters()
{
    if(!application) return;

    std::cout<<"ASaveParameters()"<<std::endl;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt", QSettings::IniFormat);

    settings.beginGroup(accessibleName());

    //���������� ��������� �� ������
    settings.setValue("geometry", saveGeometry());

    //-----------------------------------------------------------��������� UGraphWidget
    settings.setValue("int_flagUpdateBorders",flagUpdateBordersX);
    settings.setValue("QString_lableX",lableX);
    settings.setValue("Qstring_lableY",lableY);


    //-----------------------------------------------------------��������� UGraphWindow
    settings.setValue("int_currentItem",graphPainter->getCurrentItem());

    settings.setValue("double_leftLimitGraph",QVariant(graphPainter->getLeftLimitGraph()));
    settings.setValue("double_rightLimitGraph",QVariant(graphPainter->getRightLimitGraph()));
    settings.setValue("double_lowerLimitGraph",QVariant(graphPainter->getLowerLimitGraph()));
    settings.setValue("double_upperLimitGraph",QVariant(graphPainter->getUpperLimitGraph()));

    //�������� ���������� ���������  ����������
    int sizeVectorStruct=graphPainter->getSize();
    settings.setValue("int_sizeVectorStruct",QVariant(sizeVectorStruct));

    //���� �� ���������
    for (int j=0; j<sizeVectorStruct; j++)
    {
        static int graphColor;       //���� �������
        static  std::string graphName;   //��� �������
        static  int indexChannel;          //������ ������
        static  std::string nameComponent; //��� ����������
        static std::string nameProperty;  //��� ��������
        static std::string typeProperty;  //��� ����������
        //���������� �������� �������
        static int Jx;
        static int Jy;

        indexChannel=graphPainter->getStructContent(j,&graphColor,&graphName,&nameComponent,
                               &nameProperty,&typeProperty, &Jx, &Jy);

        settings.setValue(
                    "vectorStruct"+QString::number(j),
                    QString::number(indexChannel)+"*"+QString::fromStdString(nameComponent)
                    +"*"+QString::fromStdString(nameProperty)+"*"+QString::fromStdString(typeProperty)
                    +"*"+QString::number(Jx)+"*"+QString::number(Jy)+"*"+QString::fromStdString(graphName)
                    +"*"+QString::number(graphColor));
    }

    //-----------------------------------------------------------����� ������ ����������
    settings.endGroup();
}

void UGraphWidget::ALoadParameters()
{
    if(!application) return;
    std::cout<<"ALoadParameters()"<<std::endl;

    QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);

    settings.beginGroup(accessibleName());

    //**************** ���������� ������������ ������� ���������

    restoreGeometry(settings.value("geometry").toByteArray());

    //-----------------------------------------------------------��������� UGraphWidget

    flagUpdateBordersX=settings.value("int_flagUpdateBorders", -1).toInt();
    lableX=settings.value("QString_lableX", "time").toString();
    lableY=settings.value("QString_lableY", "Selected Output").toString();

    //-----------------------------------------------------------��������� UGraphWindow
    graphPainter->mainStartGraphSettings (settings.value("double_leftLimitGraph", -1).toDouble(),
                                          settings.value("double_rightLimitGraph", 6).toDouble(),
                                          settings.value("double_lowerLimitGraph", -2).toDouble(),
                                          settings.value("double_upperLimitGraph",2).toDouble(),
                                          lableX,lableY);

    int currentItem = settings.value("int_currentItem", -1).toInt();
    //graphPainter->setCurrentItem(currentItem); �������� �����, ����� �������� ��������

    //������� ���������� ���������  ���������
    int sizeVectorStruct=settings.value("int_sizeVectorStruct", -1).toInt();
    if (sizeVectorStruct>0)
        for (int j=0; j<sizeVectorStruct; j++)
        {
            static int graphColor;       //���� �������
            static  std::string graphName;   //��� �������
            static  int indexChannel;          //������ ������
            static  std::string nameComponent; //��� ����������
            static std::string nameProperty;  //��� ��������
            static std::string typeProperty;  //��� ����������
            //���������� �������� �������
            static int Jx;
            static int Jy;


            QString source = settings.value("vectorStruct"+QString::number(j)).toString();
            if(!source.isEmpty())
            {
                QStringList list = source.split("*");
                if (list.size() == 8)
                {
                    indexChannel = list.at(0).toInt();
                    nameComponent = list.at(1).toLocal8Bit().constData();
                    nameProperty = list.at(2).toLocal8Bit().constData();
                    typeProperty = list.at(3).toLocal8Bit().constData();
                    Jx =list.at(4).toInt();
                    Jy =list.at(5).toInt();
                    graphName = list.at(6).toLocal8Bit().constData();
                    graphColor = list.at(7).toInt();


                    //���������� ������ �������� ������� - ���������
                    int graph_index=graphPainter->addGraphVisualParameters(graphName,graphColor);
                    graphPainter->setGraphDataSource(graph_index,indexChannel,
                                                     nameComponent,
                                                     nameProperty,typeProperty ,Jx,Jy);
                }
            }
            graphPainter->setCurrentItem(currentItem);
        }
    //**************** ����� ���������� ������������ ������� ���������
    settings.endGroup();
}

void UGraphWidget::slotActionSelectOutput()
{
    std::cout<<"selectDir_clicked"<<std::endl;
    /*/// �������� ������ ����������
    RDK::UApplication *application;
    /// true - �� ����� ���������� AUpdateInterface, ��� ��������� ����� - false
    /// ��������� ������ ������������ ���������� ����������,
    /// ������������ � ������ ������� �������������� ���������� �� ���������� ���������� � �� ���� ����.*/
    if(!application)
        return;
    UComponentPropertySelectionWidget dialog(this, 3, application);
    //dialog.componentsList->setChannelsListVisible(ui->checkBoxIndChannels->isChecked());
    //if (dialog.exec() && selectedImage)
    if (dialog.exec())
    {
        //���������� ������ �������� ������� - ���������
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
        masX.erase(masX.begin()+a);
        masY.erase(masY.begin()+a);
        graphPainter->redrawGraph();

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
                masX.erase(masX.begin()+a);
                masY.erase(masY.begin()+a);
                graphPainter->redrawGraph();

            }
        }
    }

}

