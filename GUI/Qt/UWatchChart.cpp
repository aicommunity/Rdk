#include "UWatchChart.h"
#include "ui_UWatchChart.h"
#include <QVBoxLayout>
#include <iostream>

#include "UWatchTab.h"

UWatchChart::UWatchChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UWatchChart)
{
    setAccessibleName("UWatchChart");
    ui->setupUi(this);

    //������� ������, ��������� � ����������� �����������
    verticalLayout = new QVBoxLayout(this);
    chart = new QChart();
    chartView = new UWatchChartView(this);
    horizontalScrolBar = new QScrollBar(Qt::Horizontal,this);

    setLayout(verticalLayout);

    verticalLayout->addWidget(chartView);
    verticalLayout->addWidget(horizontalScrolBar);
    verticalLayout->setSpacing(0);
    verticalLayout->setContentsMargins(0,0,0,0);

    //������� � ����������� ���
    axisX = new QValueAxis(this);
    axisY = new QValueAxis(this);

    ///��������� ������� ���� � �� ���� � ���
    setAxisXname("time, sec");
    setAxisYname("Output parameter");
    axisXrange = 5;
    axisX->setRange(0, axisXrange);
    axisY->setRange(-5, 5);

    //������������� ���
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    //������������� ������ � ������ -_-
    chartView->setChart(chart);

    //������ �������� ������� ��� �������
    chartView->setFrameStyle(QFrame::Panel |QFrame::StyledPanel);

    chartView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(chartView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
    connect(chartView, SIGNAL(updateChartAxes(double, double, double)), this, SLOT(updateAxes(double, double, double)));

    WatchTab = dynamic_cast<UWatchTab*>(parent);
}

UWatchChart::~UWatchChart()
{
    delete ui;
}


void UWatchChart::setChartTitle(QString title)
{
    chart->setTitle(title);
}

void UWatchChart::setChartIndex(int index)
{
    chartIndex = index;
}

void UWatchChart::setSerieName(int serieIndex, QString name)
{
    series[serieIndex]->setName(name);
}

void UWatchChart::setSerieColor(int serieIndex, int colorIndex)
{
     series[serieIndex]->setColor(defaultColors[colorIndex]);
}

void UWatchChart::setSerieLineType(int serieIndex, Qt::PenStyle lineType)
{
    QPen pen;
    pen.setStyle(lineType);
      series[serieIndex]->setPen(pen);
}

void UWatchChart::setSerieWidth(int serieIndex, int width)
{
    QPen pen;
    pen.setWidth(width);
    series[serieIndex]->setPen(pen);
}

void UWatchChart::setSerieStyle(int serieIndex, QColor color, int width, Qt::PenStyle lineType)
{
    QPen pen;
    pen.setColor(color);
    pen.setWidth(width);
    pen.setStyle(lineType);
    series[serieIndex]->setPen(pen);
}

void UWatchChart::setSerieYshift(int serieIndex, int y_shift)
{
    series[serieIndex]->YShift = y_shift;
}

void UWatchChart::createSerie(int channelIndex, const QString componentName, const QString propertyName,
                              const QString type, int jx, int jy, double time_interval, double y_shift)
{
    //������� ����� ������ � ����������� ��� � ����
    series.push_back(new UWatchSerie());
    chart->addSeries(series.last());
    series.last()->attachAxis(axisX);
    series.last()->attachAxis(axisY);

    //��� ������� = ��� ���������� +  ��� ��������
    series.last()->setName(componentName+ ": " + propertyName +"(" + QString::number(jx)+", "+ QString::number(jy)+")");
    series.last()->setColor(defaultColors[series.count()-1]);

    //���������� ��������� ��������� ������
    series.last()->indexChannel = channelIndex;
    series.last()->nameComponent = componentName;
    series.last()->nameProperty = propertyName;
    series.last()->typeProperty = type;
    series.last()->Jx = jx;
    series.last()->Jy = jy;
    series.last()->YShift = y_shift;

    RDK::UELockPtr<RDK::UEnvironment> env=RDK::GetEnvironmentLock();

    RDK::UControllerDataReader * data=env->RegisterDataReader(componentName.toStdString(),
                                                              propertyName.toStdString(),
                                                              jx,
                                                              jy);
    if(data)
    {
        series.last()->data_reader = data;
        data->SetTimeInterval(time_interval);
    }
}

void UWatchChart::deleteSerie(int serieIndex)
{

    RDK::UELockPtr<RDK::UEnvironment> env=RDK::GetEnvironmentLock();
    env->UnRegisterDataReader(series[serieIndex]->nameComponent.toStdString(),
                              series[serieIndex]->nameProperty.toStdString(),
                              series[serieIndex]->Jx,
                              series[serieIndex]->Jy);

    delete series[serieIndex];
    series.remove(serieIndex);

}

void UWatchChart::addDataToSerie(int serieIndex, double x, double y)
{
    series[serieIndex]->append(x, y);
}

int UWatchChart::countSeries()
{
    return series.count();
}

void UWatchChart::setAxisXname(QString name)
{
    axisX->setTitleText(name);
}

void UWatchChart::setAxisYname(QString name)
{
    axisY->setTitleText(name);
}

void UWatchChart::setAxisXmin(double value)
{
    axisX->setMin(value);
}

void UWatchChart::setAxisXmax(double value)
{
    axisX->setMax(value);
}

void UWatchChart::updateTimeIntervals(double value)
{
    axisXrange = value;
    setAxisXmax(getAxisXmin()+axisXrange);
    for(int i = 0; i < series.size(); i++)
    {
        series[i]->data_reader->SetTimeInterval(value);
    }
}

void UWatchChart::setAxisYmin(double value)
{
    axisY->setMin(value);
}

void UWatchChart::setAxisYmax(double value)
{
    axisY->setMax(value);
}

void UWatchChart::updateAxes(double x_range, double y_min, double y_max)
{
    updateTimeIntervals(x_range);
    setAxisYmin(y_min);
    setAxisYmax(y_max);
}


QString UWatchChart::getChartTitle()
{
    return chart->title();
}

UWatchSerie *UWatchChart::getSerie(int index)
{
    return series[index];
}

QString UWatchChart::getAxisXName()
{
    return axisX->titleText();
}

QString UWatchChart::getAxisYName()
{
    return axisY->titleText();
}

QString UWatchChart::getSerieName(int serieIndex)
{
    return series[serieIndex]->name();
}

QColor UWatchChart::getSerieColor(int serieIndex)
{
    return series[serieIndex]->color();
}

int UWatchChart::getSerieWidth(int serieIndex)
{
    return series[serieIndex]->pen().width();
}

Qt::PenStyle UWatchChart::getSerieLineType(int serieIndex)
{
    return series[serieIndex]->pen().style();
}

double UWatchChart::getSerieYShift(int serieIndex)
{
    return series[serieIndex]->YShift;
}

double UWatchChart::getAxisXmin()
{
    return axisX->min();
}

double UWatchChart::getAxisXmax()
{
    return axisX->max();
}

double UWatchChart::getAxisYmin()
{
    return axisY->min();
}

double UWatchChart::getAxisYmax()
{
    return axisY->max();
}

void UWatchChart::wheelEvent(QWheelEvent *event)
{
    //��������� ��������� ������ ����
    /// ������ ��������� = ������
    /// ctrl + ��������� = ���
    int degrees = event->delta();
    if(isCtrlPressed && isAxisYzoomable)
    { 
        if(degrees>0)
        {
           if(axisY->min()>0 && axisY->max()>0)axisY->setRange(axisY->min()*0.9,axisY->max()*1.1);
           else if(axisY->min()<0 && axisY->max()>0)axisY->setRange(axisY->min()*1.1,axisY->max()*1.1);
           else if(axisY->min()<0 && axisY->max()<0)axisY->setRange(axisY->min()*1.1,axisY->max()*0.9);
        }
        else
        {
           if(axisY->min()>0 && axisY->max()>0)axisY->setRange(axisY->min()*1.1,axisY->max()*0.9);
           else if(axisY->min()<0 && axisY->max()>0)axisY->setRange(axisY->min()*0.9,axisY->max()*0.9);
           else if(axisY->min()<0 && axisY->max()<0)axisY->setRange(axisY->min()*0.9,axisY->max()*1.1);
        }
    }
    else if(isAxisYscrollable)
    {
        if(degrees>0)
           axisY->setRange(axisY->min()+0.001*axisY->min(), axisY->max()+0.001*axisY->max());
        else
           axisY->setRange(axisY->min()-0.001*axisY->min(), axisY->max()-0.001*axisY->max());
    }
}

//��������� ������ ctrl
void UWatchChart::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == 16777249) isCtrlPressed = true; //ctrl
}

void UWatchChart::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == 16777249) isCtrlPressed = false;
}

void UWatchChart::slotCustomMenuRequested(QPoint pos)
{
    // ������� ������ ������������ ����
    QMenu * menu = new QMenu(this);

    // ������ �������� ��� ������������ ����
    QAction * addSeiesAction =      new QAction("Add series", this);
    QAction * seriesOptionAction =  new QAction("Series option", this);
    QAction * chartOptionAction =   new QAction("Chart's option", this);
    QAction * saveJpegAction =      new QAction("Save chart to JPEG", this);



    /* ���������� ����� ����������� ��� �������� ������������ ���� */
    connect(addSeiesAction, SIGNAL(triggered()), this, SLOT(addSeriesSlot()));
    connect(seriesOptionAction, SIGNAL(triggered()), this, SLOT(seriesOptionSlot()));
    connect(chartOptionAction, SIGNAL(triggered()), this, SLOT(chartOptionSlot()));
    connect(saveJpegAction, SIGNAL(triggered()), this, SLOT(saveToJpegSlot()));

    /* ������������� �������� � ���� */
    menu->addAction(addSeiesAction);
    menu->addAction(seriesOptionAction);
    menu->addAction(chartOptionAction);
    menu->addAction(saveJpegAction);

    /* �������� ����������� ���� */
    menu->popup(mapToGlobal(pos));
}

void UWatchChart::addSeriesSlot()
{
    //�������� ���� ��� ���������� ����� �����
    emit addSerieSignal(chartIndex);
}

void UWatchChart::seriesOptionSlot()
{
    if(!WatchTab)
        return;

    WatchTab->seriesOptionTriggered();
}

void UWatchChart::chartOptionSlot()
{
    if(!WatchTab)
        return;

    WatchTab->chartsOptionTriggered();
}

void UWatchChart::saveToJpegSlot()
{
    QPixmap screenShot;
    screenShot = chartView->grab(); //����������� ������ ������� �������
    QString currentDate = QDateTime::currentDateTime().toString("dd-MM-yy HH-mm"); //�� ������� . � :, ����� �� ������� �����������

    //������ � ����� � ����� screenshot
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();

    //���������, ���� �� ����� screenshots
    //���� ���, �� �������
    if(dir.cd("screenshots"))
    {
        std::cout<<"screenshot folder is exist"<<std::endl;
    }
    else {
         std::cout<<"screenshot folder is not exist"<<std::endl;
         std::cout<<"screenshot folder creating"<<std::endl;
         dir.mkdir("screenshots");
         dir.cd("screenshots");
    }

    if(screenShot.save((dir.path()+"/"+chart->title()+ " " + currentDate + ".jpeg")))
    {
        //������� ��� ��� ������ � ��� ����� ��������
        std::cout<<"chart save succes"<<std::endl;
        QMessageBox messageBox;
        messageBox.setText("Chart save successfully!");
        messageBox.setInformativeText("saved in folder \"screenshots\"");
        messageBox.setWindowTitle("Saving chart");
        messageBox.setIcon(QMessageBox::Information);
        messageBox.setStandardButtons(QMessageBox::Cancel);
        messageBox.exec();
    }
    else std::cout<<"chart save not succes"<<std::endl;  //���-�� �� ���
}
