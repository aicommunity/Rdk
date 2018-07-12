#include "UGraphWidget.h"
#include "ui_UGraphWidget.h"
#include <iostream>

UGraphWidget::UGraphWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent,app),
    ui(new Ui::UGraphWidget)
{
    setAccessibleName("GraphWindow");
    UpdateInterval = 30;

    ui->setupUi(this);

    graphPainter = new UGraphWindow(this);
    ui->verticalLayoutGraph->addWidget(graphPainter);

    // Если нажали на кнопку - появляется сигнал
    //connect(ui->drawOneGraph, SIGNAL(pressed()), this, SIGNAL(drawSmth()));
    connect(ui->delAllButton, SIGNAL(pressed()), this, SIGNAL(delAllButtonSignal()));
    connect(ui->changeColor, SIGNAL(pressed()), this, SIGNAL(changeColorSignal()));
    connect(ui->changeCurrentItem, SIGNAL(pressed()), this, SIGNAL(changeCurrentItemSignal()));

    // Сигнал связывается со слотом
    connect(this, SIGNAL(transferDataSignal(int,QVector<double>,QVector<double>)),
            graphPainter, SLOT(setData(int,QVector<double>,QVector<double>)));
    //connect(this, SIGNAL(drawSmth()), graphPainter, SLOT(pressEvent()));
    connect(this, SIGNAL(delAllButtonSignal()), graphPainter, SLOT(delAllGraph()));
    connect(this, SIGNAL(changeColorSignal()), graphPainter, SLOT(changeColor()));
    connect(this, SIGNAL(changeCurrentItemSignal()), graphPainter, SLOT(changeCurrentItem()));

    //Изначально не нужно изменять правую границу графика
    //Пока флаг не изменится
    flagUpdateBorders=-1;

    double leftLimit = -1; //Начало интервала, где рисуем график по оси Ox
    double rightLimit =  12; //Конец интервала, где рисуем график по оси Ox
    double lowerLimit = -2; //Начало интервала, где рисуем график по оси Ox
    double upperLimit =  2; //Конец интервала, где рисуем график по оси Ox
    graphPainter->mainStartGraphSettings (leftLimit,rightLimit, lowerLimit, upperLimit,
                                          "time","Selected Output");


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
    masX[id].resize(startSizeElem+addSizeElem);
    masY[id].resize(startSizeElem+addSizeElem);

   //Заполнить появившеся место
    for (size_t j=startSizeElem; j<startSizeElem+addSizeElem-1; j++)
    {
        masX[id][j]=X1[j-startSizeElem];
        masY[id][j]=Y1[j-startSizeElem];
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


void UGraphWidget::AUpdateInterface()
{
    int n=graphPainter->getSize();
    for(int i=0; i<n; i++)
    {
        const TSingleGraph& current_graph=graphPainter->getGraph(i);

        // обратимся к ядру, возьмем матрицу
        RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock<RDK::UNet>();
        //RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock();

        RDK::UEPtr<RDK::UContainer> component=model->GetComponentL(current_graph.nameCompоnent,true);
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
        if (flagUpdateBorders>0)
            graphPainter->changeRightBorder(x+0.1*x);
    }
    graphPainter->redrawGraph();
}


UGraphWindow *UGraphWidget::getGraphPainter() const
{
    return graphPainter;
}

void UGraphWidget::on_selectDir_clicked()
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

void UGraphWidget::on_updateBordersButton_clicked()
{
    flagUpdateBorders*=-1;
}
