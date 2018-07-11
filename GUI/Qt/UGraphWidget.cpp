#include "UGraphWidget.h"
#include "ui_UGraphWidget.h"
#include <iostream>

GraphWindow::GraphWindow(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent,app),
    ui(new Ui::GraphWindow)
{
    setAccessibleName("GraphWindow");
    UpdateInterval = 30;

    ui->setupUi(this);
    graphPainter = new OtherWindow(this);
    ui->verticalLayoutGraph->addWidget(graphPainter);

    connect(ui->drawOneGraph, SIGNAL(pressed()), this, SIGNAL(drawSmth()));
    connect(ui->delAllButton, SIGNAL(pressed()), this, SIGNAL(delSignal()));
    connect(ui->changeColor, SIGNAL(pressed()), this, SIGNAL(changeColorSignal()));
    connect(ui->changeCurrentItem, SIGNAL(pressed()), this, SIGNAL(changeCurrentItemSignal()));

    double leftLimit = -1; //Начало интервала, где рисуем график по оси Ox
    double rightLimit =  12; //Конец интервала, где рисуем график по оси Ox
    graphPainter->mainSettings (leftLimit,rightLimit,"Xxx","Yyy");



    connect(this, SIGNAL(transferDataSignal(int,QVector<double>,QVector<double>)), graphPainter, SLOT(dataFromMainWin(int,QVector<double>,QVector<double>)));
    connect(this, SIGNAL(drawSmth()), graphPainter, SLOT(pressEvent()));
    connect(this, SIGNAL(delSignal()), graphPainter, SLOT(delAllGraph()));
    connect(this, SIGNAL(changeColorSignal()), graphPainter, SLOT(changeColor()));
    connect(this, SIGNAL(changeCurrentItemSignal()), graphPainter, SLOT(changeCurrentItem()));

    //UpdateInterface(true);
}

GraphWindow::~GraphWindow()
{
    delete ui;
}


/*void GraphWindow::addMainData(int id, std::vector<double> X1, std::vector<double> Y1, const int N)
{
    masX.resize(id);
    masY.resize(id);
    masX[id-1].resize(N);
    masY[id-1].resize(N);

    for (int j=0; j<N; j++)
    {
        masX[id-1][j]=X1[j];
        masY[id-1][j]=Y1[j];
    }
    //std::cout<<X1[0]<<X1[1]<<X1[2]<<X1[3]<<std::endl;
    emit transferDataSignal(id, masX[id-1], masY[id-1]);
}
*/
/*void GraphWindow::addMainData(int id, std::vector<double> X1, std::vector<double> Y1)
{
    int N=X1.size();
    masX.resize(id);
    masY.resize(id);
    masX[id-1].resize(N);
    masY[id-1].resize(N);

    for (int j=0; j<N; j++)
    {
        masX[id-1][j]=X1[j];
        masY[id-1][j]=Y1[j];
    }
    //std::cout<<X1[0]<<X1[1]<<X1[2]<<X1[3]<<std::endl;
    emit transferDataSignal(id, masX[id-1], masY[id-1]);
}*/

void GraphWindow::addMainData(int id, std::vector<double>  X1, std::vector<double> Y1)
{
    id++;
    //проверить есть ли массив с нужным id, если нет, то добавить
    size_t startSizeMas=masX.size();
    if (startSizeMas<id)
    {
        masX.resize(id);
        masY.resize(id);
    }

    //Изменить размер на нужный
    size_t startSizeElem = masX[id-1].size();
    size_t addSizeElem = X1.size();
    masX[id-1].resize(startSizeElem+addSizeElem);
    masY[id-1].resize(startSizeElem+addSizeElem);

   //Заполнить появившеся место
    for (size_t j=startSizeElem; j<startSizeElem+addSizeElem-1; j++)
    {
        masX[id-1][j]=X1[j-startSizeElem];
        masY[id-1][j]=Y1[j-startSizeElem];
    }
    emit transferDataSignal(id, masX[id-1], masY[id-1]);
}

void GraphWindow::addMainData(int id, double X1, double Y1)
{
    id++;
    //проверить есть ли массив с нужным id, если нет, то добавить
    size_t startSizeMas=masX.size();
    if (startSizeMas<id)
    {
        masX.resize(id);
        masY.resize(id);
    }

    //Положить элементы в конец каждого массива
    masX[id-1].push_back(X1);
    masY[id-1].push_back(Y1);

    emit transferDataSignal(id, masX[id-1], masY[id-1]);
}
/*void  GraphWindow::addData (int id, std::vector<double> X, std::vector<double> Y)
{
    size_t i=X.size();
    size_t n=i+masX[id-1].size();
    masX[id-1].resize(n);
    masY[id-1].resize(n);
    for (size_t j=i; j<n; j++)
    {
        masX[id-1][j]=X[j-i];
        masY[id-1][j]=Y[j-i];
    }
    emit transferDataSignal(id, masX[id-1], masY[id-1]);
}*/
//--------------------------------------------------------------------------------
void GraphWindow::AUpdateInterface()
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
        addMainData(i,x,y);
    }
    graphPainter->redrawGraph();
}
//--------------------------------------------------------------------------------

OtherWindow *GraphWindow::getGraphPainter() const
{
    return graphPainter;
}

void GraphWindow::on_delAllButton_clicked()
{
}

void GraphWindow::on_changeCurrentItem_clicked()
{
}

void GraphWindow::on_changeColor_clicked()
{
}

void GraphWindow::on_drawOneGraph_clicked()
{
}

void GraphWindow::on_selectDir_clicked()
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
        int graph_index=graphPainter->addGraph("first",Qt::green);
        graphPainter->setGraphDataSource(graph_index,dialog.componentsList->getSelectedChannelIndex(),dialog.componentsList->getSelectedComponentLongName().toLocal8Bit().data(),dialog.componentsList->getSelectedPropertyName().toLocal8Bit().data(),"type",0,0);

        /*selectedImage->setComponentName(dialog.componentsList->getSelectedComponentLongName());
        selectedImage->setComponentPropertyName(dialog.componentsList->getSelectedPropertyName());
        selectedImage->setCalcChannel(dialog.componentsList->getSelectedChannelIndex());*/
    }
    dialog.writeSettings(QString::fromLocal8Bit(
                           application->GetProjectPath().c_str())+"settings.qt");

}
