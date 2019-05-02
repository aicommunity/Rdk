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

    // Сигнал связывается со слотом
    connect(this, SIGNAL(transferDataSignal(int,QVector<double>,QVector<double>)),
            graphPainter, SLOT(setData(int,QVector<double>,QVector<double>)));
    //connect(this, SIGNAL(delAllButtonSignal()), graphPainter, SLOT(delAllGraph()));
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

void UGraphWidget::addDataToGraph(int id, std::vector<double>  X1, std::vector<double> Y1)
{
    //проверить есть ли массив с нужным id, если нет, то добавить
    size_t startSizeMas=masX.size();
    if (startSizeMas<id+1)
    {
        masX.resize(id+1);
        masY.resize(id+1);
    }

    //Изменить размер на нужный
    size_t startSizeElem = masX[id].size();
    size_t addSizeElem = X1.size();
    masX[id].resize(int(startSizeElem+addSizeElem));
    masY[id].resize(int(startSizeElem+addSizeElem));

   //Заполнить появившеся место
    for (size_t j=startSizeElem; j<startSizeElem+addSizeElem-1; j++)
    {
        masX[id][j]=X1[int(j-startSizeElem)];
        masY[id][j]=Y1[int(j-startSizeElem)];
    }
    emit transferDataSignal(id, masX[id], masY[id]);
}

void UGraphWidget::addDataToGraph(int id, double X1, double Y1)
{
    //проверить есть ли массив с нужным id, если нет, то добавить
    size_t startSizeMas=masX.size();
    if (startSizeMas<id+1)
    {
        masX.resize(id+1);
        masY.resize(id+1);
    }

    //Положить элементы в конец каждого массива
    masX[id].push_back(X1);
    masY[id].push_back(Y1);

    emit transferDataSignal(id, masX[id], masY[id]);
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

        // обратимся к ядру, возьмем матрицу
        RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock<RDK::UNet>();
        //RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock();

        RDK::UEPtr<RDK::UContainer> component=model->GetComponentL(current_graph.nameComponent,true);
        if(!component)
            continue;

        RDK::MDMatrix<double>* m=component->AccessPropertyData<RDK::MDMatrix<double> >(current_graph.nameProperty);
        if(!m)
            continue;

        if(m->GetCols()<=current_graph.Jx || m->GetRows()<=current_graph.Jy)
            continue;

        double y=(*m)(current_graph.Jy,current_graph.Jx);

        double x=Model_GetDoubleRealTime();

        // ее данные положим на этот график
        addDataToGraph(i,x,y);
        //Проверяем нужно ли изменять границы по оси Х
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
        //Проверяем нужно ли изменять границы по оси Y
        if (flagUpdateBordersMaxY>0)
        {
            //максимальная граница
            if ((1.1*y)>graphPainter->getUpperLimitGraph())
                graphPainter->setUpperLimitGraph(y+0.1*y);
            //std::cout<<graphPainter->getUpperLimitGraph()<<"   "<<y<<std::endl;
        }
        if (flagUpdateBordersMinY>0)
        {
            //минимальная граница
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

    //Сохранение геометрии на экране
    settings.setValue("geometry", saveGeometry());

    //-----------------------------------------------------------Параметры UGraphWidget
    settings.setValue("int_flagUpdateBorders",flagUpdateBordersX);
    settings.setValue("QString_lableX",lableX);
    settings.setValue("Qstring_lableY",lableY);


    //-----------------------------------------------------------Параметры UGraphWindow
    settings.setValue("int_currentItem",graphPainter->getCurrentItem());

    settings.setValue("double_leftLimitGraph",QVariant(graphPainter->getLeftLimitGraph()));
    settings.setValue("double_rightLimitGraph",QVariant(graphPainter->getRightLimitGraph()));
    settings.setValue("double_lowerLimitGraph",QVariant(graphPainter->getLowerLimitGraph()));
    settings.setValue("double_upperLimitGraph",QVariant(graphPainter->getUpperLimitGraph()));

    //передать количество элементов  вструктуре
    int sizeVectorStruct=graphPainter->getSize();
    settings.setValue("int_sizeVectorStruct",QVariant(sizeVectorStruct));

    //цикл по элементам
    for (int j=0; j<sizeVectorStruct; j++)
    {
        static int graphColor;       //Цвет графика
        static  std::string graphName;   //Имя графика
        static  int indexChannel;          //Индекс канала
        static  std::string nameComponent; //Имя компонента
        static std::string nameProperty;  //Имя свойства
        static std::string typeProperty;  //Тип компонента
        //координаты элемента матрицы
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

    //-----------------------------------------------------------Конец записи параметров
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

    //**************** Заполнение сохраненными данными элементов

    restoreGeometry(settings.value("geometry").toByteArray());

    //-----------------------------------------------------------Параметры UGraphWidget

    flagUpdateBordersX=settings.value("int_flagUpdateBorders", -1).toInt();
    lableX=settings.value("QString_lableX", "time").toString();
    lableY=settings.value("QString_lableY", "Selected Output").toString();

    //-----------------------------------------------------------Параметры UGraphWindow
    graphPainter->mainStartGraphSettings (settings.value("double_leftLimitGraph", -1).toDouble(),
                                          settings.value("double_rightLimitGraph", 6).toDouble(),
                                          settings.value("double_lowerLimitGraph", -2).toDouble(),
                                          settings.value("double_upperLimitGraph",2).toDouble(),
                                          lableX,lableY);

    int currentItem = settings.value("int_currentItem", -1).toInt();
    //graphPainter->setCurrentItem(currentItem); написать позже, после создания графиков

    //считать количество элементов  структуре
    int sizeVectorStruct=settings.value("int_sizeVectorStruct", -1).toInt();
    if (sizeVectorStruct>0)
        for (int j=0; j<sizeVectorStruct; j++)
        {
            static int graphColor;       //Цвет графика
            static  std::string graphName;   //Имя графика
            static  int indexChannel;          //Индекс канала
            static  std::string nameComponent; //Имя компонента
            static std::string nameProperty;  //Имя свойства
            static std::string typeProperty;  //Тип компонента
            //координаты элемента матрицы
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


                    //Заполнение одного элемента вектора - структуры
                    int graph_index=graphPainter->addGraphVisualParameters(graphName,graphColor);
                    graphPainter->setGraphDataSource(graph_index,indexChannel,
                                                     nameComponent,
                                                     nameProperty,typeProperty ,Jx,Jy);
                }
            }
            graphPainter->setCurrentItem(currentItem);
        }
    //**************** Конец заполнения сохраненными данными элементов
    settings.endGroup();
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

