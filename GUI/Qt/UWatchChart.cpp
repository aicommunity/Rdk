#include "UWatchChart.h"
#include "ui_UWatchChart.h"
#include "UStyleManager.h"
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QSet>
#include <QtCharts/QXYSeries>
#include <iostream>

#include "UWatchTab.h"
#include "UVisualControllerWidget.h"

UWatchChart::UWatchChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UWatchChart),
    pendingUpdate(false)
{
    setAccessibleName("UWatchChart");
    setObjectName(QStringLiteral("UWatchChart"));
    ui->setupUi(this);

    verticalLayout = new QVBoxLayout(this);
    chart = new QChart();
    chartView = new UWatchChartView(this);

    setLayout(verticalLayout);

    modeBar = new QToolBar(tr("Chart tools"), this);
    modeBar->setIconSize(QSize(16, 16));
    modeBar->setMovable(false);
    actPan = modeBar->addAction(tr("Pan"));
    actBoxZoom = modeBar->addAction(tr("Box zoom"));
    actTrack = modeBar->addAction(tr("Track"));
    actReset = modeBar->addAction(tr("Reset"));
    actPan->setCheckable(true);
    actBoxZoom->setCheckable(true);
    actTrack->setCheckable(true);
    actTrack->setChecked(true);
    actBoxZoom->setChecked(true);
    connect(actPan, &QAction::triggered, this, &UWatchChart::onModePan);
    connect(actBoxZoom, &QAction::triggered, this, &UWatchChart::onModeBoxZoom);
    connect(actTrack, &QAction::triggered, this, &UWatchChart::onModeTrack);
    connect(actReset, &QAction::triggered, this, &UWatchChart::onModeReset);

    verticalLayout->addWidget(modeBar);
    verticalLayout->addWidget(chartView);
    verticalLayout->setSpacing(0);
    // Leave room so selection chrome in paintEvent is not covered by children.
    verticalLayout->setContentsMargins(3, 3, 3, 3);

    //создаем и настраиваем оси
    axisX = new QValueAxis(this);
    axisY = new QValueAxis(this);

    ///Дефолтные подписи осей и их макс и мин
    setAxisXname("time, sec");
    setAxisYname("Output parameter");
    axisXrange = 2;
    axisX->setRange(0, axisXrange);
    axisY->setRange(-1, 1);

    //устанавливаем оси
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    if (chart->legend())
        chart->legend()->setVisible(m_legendVisible);

    //устанавливаем график в график -_-
    chartView->setChart(chart);
    chartView->setRubberBand(QChartView::RectangleRubberBand);

    //делаем красивую рамочку для графика
    chartView->setFrameStyle(QFrame::Panel |QFrame::StyledPanel);

    chartView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(chartView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
    connect(chartView, SIGNAL(updateChartAxes(double, double, double, double)), this, SLOT(updateAxes(double, double, double, double)));
    connect(chartView, &UWatchChartView::chartClicked, this, [this]() {
        emit chartActivated(chartIndex);
    });

    WatchTab = dynamic_cast<UWatchTab*>(parent);

    connect(this, SIGNAL(UpdateTabGuiSignal(bool)), parent, SLOT(UpdateInterface(bool)));

    // Применяем стили темы к графику
    applyTheme();

    fixInitialAxesState();
}

UWatchChart::~UWatchChart()
{
    delete ui;
}

QColor UWatchChart::getDefaultColor(int index) const
{
    return UStyleManager::instance()->getChartSeriesColor(index);
}

void UWatchChart::applyTheme()
{
    if(!chart || !axisX || !axisY)
        return;
    
    UStyleManager* styleManager = UStyleManager::instance();
    
    // Применяем фон графика
    chart->setBackgroundBrush(QBrush(styleManager->getChartBackgroundColor()));
    
    // Применяем цвета осей
    QPen axisPen(styleManager->getChartAxisColor());
    axisX->setLinePenColor(styleManager->getChartAxisColor());
    axisY->setLinePenColor(styleManager->getChartAxisColor());
    
    // Применяем цвета подписей осей
    QBrush labelBrush(styleManager->getChartLabelColor());
    axisX->setLabelsBrush(labelBrush);
    axisY->setLabelsBrush(labelBrush);
    
    // Применяем цвета сетки
    axisX->setGridLineColor(styleManager->getChartGridColor());
    axisY->setGridLineColor(styleManager->getChartGridColor());
    
    // Применяем цвет заголовка
    chart->setTitleBrush(QBrush(styleManager->getChartTitleColor()));
    
    // Обновляем график
    chart->update();
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
    if (serieIndex < 0 || serieIndex >= series.count() || !series[serieIndex])
        return;
    const QColor c = getDefaultColor(colorIndex);
    series[serieIndex]->setColor(c);
    QPen pen = series[serieIndex]->pen();
    pen.setColor(c);
    if (pen.widthF() < 1.0)
        pen.setWidth(2);
    series[serieIndex]->setPen(pen);
}

int UWatchChart::suggestAutoColorIndex(int serieIndex) const
{
    QSet<QRgb> used;
    for (int i = 0; i < series.count(); ++i)
    {
        if (i == serieIndex || !series[i])
            continue;
        used.insert(series[i]->color().rgb());
    }
    // Prefer visually distinct order (avoid red, darkRed as first two).
    static const int kOrder[] = {8, 4, 10, 6, 0, 2, 11, 7, 5, 9, 1, 3, 12, 13, 14};
    const int n = qMax(1, UStyleManager::instance()->getChartSeriesColorCount());
    for (int oi : kOrder)
    {
        if (oi >= n)
            continue;
        if (!used.contains(getDefaultColor(oi).rgb()))
            return oi;
    }
    return serieIndex >= 0 ? (serieIndex % n) : 0;
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

void UWatchChart::fixInitialAxesState()
{
    InitialAxesState = {getAxisXmin(), getAxisXmax(), getAxisYmin(), getAxisYmax()};
}

void UWatchChart::restoreInitialAxesState()
{
//    updateTimeIntervals(InitialAxesState[0]);
    setAxisXmin(InitialAxesState[0]);
    setAxisXmax(InitialAxesState[1]);
    setAxisYmin(InitialAxesState[2]);
    setAxisYmax(InitialAxesState[3]);
}

bool UWatchChart::checkZoomed(void)
{
 if(InitialAxesState[0] != getAxisXmin() ||
    InitialAxesState[1] != getAxisXmax() ||
    InitialAxesState[2] != getAxisYmin() ||
    InitialAxesState[3] != getAxisYmax())
  return true;

 return false;
}

void UWatchChart::createSerie(int channelIndex, const QString componentName, const QString propertyName,
                              const QString type, int jx, int jy, double time_interval, double y_shift)
{
    Q_UNUSED(type);
    series.push_back(new UWatchSerie());
    chart->addSeries(series.last());
    series.last()->attachAxis(axisX);
    series.last()->attachAxis(axisY);

    series.last()->setName(componentName+ ": " + propertyName +"(" + QString::number(jx)+", "+ QString::number(jy)+")");
    {
        const int autoIdx = suggestAutoColorIndex(series.count() - 1);
        setSerieColor(series.count() - 1, autoIdx);
    }

    series.last()->indexChannel = channelIndex;
    series.last()->nameComponent = componentName;
    series.last()->nameProperty = propertyName;
    series.last()->Jx = jx;
    series.last()->Jy = jy;
    series.last()->YShift = y_shift;
    series.last()->vizKind = NMSDK::Plot::VizKind::TimeSeries;

    RDK::UELockPtr<RDK::UEnvironment> env=RDK::GetEnvironmentLock();

    RDK::UControllerDataReader * data_reader = env->RegisterDataReader(componentName.toStdString(),
                                                                       propertyName.toStdString(),
                                                                       jx,
                                                                       jy);
    if(data_reader)
    {
        series.last()->data_reader = data_reader;
        data_reader->SetTimeInterval(time_interval);
    }
    connectSerieTooltip(series.last());
    emit UpdateTabGuiSignal(false);
}

void UWatchChart::createSerieXY(int channelIndex,
                                const QString& xComponent, const QString& xProperty, int xJx, int xJy,
                                const QString& yComponent, const QString& yProperty, int yJx, int yJy,
                                double y_shift, NMSDK::Plot::VizKind viz)
{
    series.push_back(new UWatchSerie());
    chart->addSeries(series.last());
    series.last()->attachAxis(axisX);
    series.last()->attachAxis(axisY);

    series.last()->setName(QStringLiteral("%1.%2 vs %3.%4")
                               .arg(xComponent, xProperty, yComponent, yProperty));
    {
        const int autoIdx = suggestAutoColorIndex(series.count() - 1);
        setSerieColor(series.count() - 1, autoIdx);
    }
    series.last()->indexChannel = channelIndex;
    series.last()->YShift = y_shift;
    series.last()->vizKind = viz;
    series.last()->applyBinding(
        NMSDK::Plot::makeXYBinding(
            channelIndex,
            NMSDK::Plot::PropertyRef{xComponent, xProperty, xJx, xJy},
            NMSDK::Plot::PropertyRef{yComponent, yProperty, yJx, yJy}),
        viz);

    // Markers so stepped X dwells read as points connected by segments.
    series.last()->setPointsVisible(true);
    if (viz == NMSDK::Plot::VizKind::XYScatter)
    {
        QPen p = series.last()->pen();
        p.setStyle(Qt::NoPen);
        series.last()->setPen(p);
    }

    RDK::UELockPtr<RDK::UEnvironment> env = RDK::GetEnvironmentLock();
    if (env)
    {
        series.last()->x_data_reader = env->RegisterDataReader(
            xComponent.toStdString(), xProperty.toStdString(), xJx < 0 ? 0 : xJx, xJy < 0 ? 0 : xJy);
        series.last()->data_reader = env->RegisterDataReader(
            yComponent.toStdString(), yProperty.toStdString(), yJx < 0 ? 0 : yJx, yJy < 0 ? 0 : yJy);
        if (series.last()->x_data_reader)
            series.last()->x_data_reader->SetTimeInterval(0); // keep long history; XY uses latest values
        if (series.last()->data_reader)
            series.last()->data_reader->SetTimeInterval(0);
    }

    if (vizKind == NMSDK::Plot::VizKind::TimeSeries)
        setVizKind(viz);
    setAxisXname(QStringLiteral("X"));
    connectSerieTooltip(series.last());
    emit UpdateTabGuiSignal(false);
}

void UWatchChart::deleteSerie(int serieIndex)
{

    RDK::UELockPtr<RDK::UEnvironment> env=RDK::GetEnvironmentLock();
    env->UnRegisterDataReader(series[serieIndex]->nameComponent.toStdString(),
                              series[serieIndex]->nameProperty.toStdString(),
                              series[serieIndex]->Jx,
                              series[serieIndex]->Jy);
    if (!series[serieIndex]->xNameComponent.isEmpty())
    {
        env->UnRegisterDataReader(series[serieIndex]->xNameComponent.toStdString(),
                                  series[serieIndex]->xNameProperty.toStdString(),
                                  series[serieIndex]->xJx < 0 ? 0 : series[serieIndex]->xJx,
                                  series[serieIndex]->xJy < 0 ? 0 : series[serieIndex]->xJy);
    }

    delete series[serieIndex];
    series.remove(serieIndex);
    emit UpdateTabGuiSignal(false);

}

void UWatchChart::addDataToSerie(int serieIndex, double x, double y)
{
    series[serieIndex]->append(x, y);
    emit UpdateTabGuiSignal(false);
}

int UWatchChart::countSeries()
{
    return series.count();
}

void UWatchChart::setAxisXname(QString name)
{
    axisX->setTitleText(name);
    emit UpdateTabGuiSignal(false);
}

void UWatchChart::setAxisYname(QString name)
{
    axisY->setTitleText(name);
    emit UpdateTabGuiSignal(false);
}

void UWatchChart::setAxisXmin(double value)
{
    axisX->setMin(value);
    requestUpdate();
}

void UWatchChart::setAxisXmax(double value)
{
    axisX->setMax(value);
    requestUpdate();
}

bool UWatchChart::getIsAxisXtrackable(void) const
{
 return isAxisXtrackable;
}

void UWatchChart::updateTimeIntervals(double value)
{
    setAxisXrange(value);
    for(int i = 0; i < series.size(); i++)
    {
        if (series[i]->data_reader)
            series[i]->data_reader->SetTimeInterval(value);
        if (series[i]->x_data_reader)
            series[i]->x_data_reader->SetTimeInterval(value);
    }
}

void UWatchChart::setAxisYmin(double value)
{
    axisY->setMin(value);
    requestUpdate();
}

void UWatchChart::setAxisYmax(double value)
{
    axisY->setMax(value);
    requestUpdate();
}

void UWatchChart::requestUpdate()
{
    pendingUpdate = true;
}

void UWatchChart::commitUpdate()
{
    if (pendingUpdate) {
        pendingUpdate = false;
        chartView->update();
    }
}

void UWatchChart::updateAxes(double x_min, double x_max, double y_min, double y_max)
{
    // Если зум в зоне не чувствительности:
    // изменение по любой из осей меньше 10% от текущего диапазона
    if(std::abs(x_max-x_min) < 0.02*(getAxisXmax()-getAxisXmin()) || std::abs(y_max-y_min) < 0.02*(getAxisYmax()-getAxisYmin()))
        return;

    // Если зум обратный, то восстанавливаем начальные значения
    if(x_max < x_min || y_max < y_min)
    {
        restoreInitialAxesState();
    }
    // В другом случае - зум
    else
    {
        //updateTimeIntervals(x_range);
        setAxisXmin(x_min);
        setAxisXmax(x_max);
        setAxisYmin(y_min);
        setAxisYmax(y_max);

    }
    emit UpdateTabGuiSignal(false);
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

double UWatchChart::getAxisXrange(void) const
{
 return axisXrange;
}

void UWatchChart::setAxisXrange(double value)
{
 axisXrange=value;
}

double  UWatchChart::getInitialAxisYmin()
{
 return InitialAxesState[2];
}

double  UWatchChart::getInitialAxisYmax()
{
 return InitialAxesState[3];
}


void UWatchChart::wheelEvent(QWheelEvent *event)
{
    //обработка прокрутки колеса мыши
    /// просто прокрутка = скролл
    /// ctrl + прокрутка = зум
    int degrees = event->angleDelta().y() / 8; // Convert from 1/8 degree units to degrees
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
        fixInitialAxesState();
    }
    else if(isAxisYscrollable)
    {
        if(degrees>0)
           axisY->setRange(axisY->min()+0.001*axisY->min(), axisY->max()+0.001*axisY->max());
        else
           axisY->setRange(axisY->min()-0.001*axisY->min(), axisY->max()-0.001*axisY->max());
    }
    emit UpdateTabGuiSignal(false);
}

//обработка кнопки ctrl
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
    QMenu * menu = new QMenu(this);

    QAction * addSeiesAction =      new QAction("Add series", this);
    QAction * seriesOptionAction =  new QAction("Series settings", this);
    QAction * chartOptionAction =   new QAction("Chart settings", this);
    QAction * saveJpegAction =      new QAction("Save chart to JPEG", this);
    QAction * restoreAxesAction =   new QAction("Restore Axes", this);

    connect(addSeiesAction, SIGNAL(triggered()), this, SLOT(addSeriesSlot()));
    connect(seriesOptionAction, SIGNAL(triggered()), this, SLOT(seriesOptionSlot()));
    connect(chartOptionAction, SIGNAL(triggered()), this, SLOT(chartOptionSlot()));
    connect(saveJpegAction, SIGNAL(triggered()), this, SLOT(saveToJpegSlot()));
    connect(restoreAxesAction, SIGNAL(triggered()), this, SLOT(restoreAxes()));

    menu->addAction(addSeiesAction);
    menu->addAction(seriesOptionAction);
    menu->addAction(chartOptionAction);
    menu->addAction(saveJpegAction);
    menu->addAction(restoreAxesAction);

    menu->popup(mapToGlobal(pos));
}

void UWatchChart::addSeriesSlot()
{
    emit addSerieSignal(chartIndex);
}

void UWatchChart::seriesOptionSlot()
{
    emit chartActivated(chartIndex);
    emit openSettingsPanel(chartIndex, true);
}

void UWatchChart::chartOptionSlot()
{
    emit chartActivated(chartIndex);
    emit openSettingsPanel(chartIndex, false);
}

void UWatchChart::setSelected(bool selected)
{
    if (m_selected == selected)
    {
        update();
        return;
    }
    m_selected = selected;
    update();
}

void UWatchChart::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    const bool multiChart = WatchTab && WatchTab->countGraphs() > 1;
    if (!multiChart)
        return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    if (m_selected)
    {
        QPen pen(QColor(47, 128, 237), 3);
        pen.setJoinStyle(Qt::MiterJoin);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(rect().adjusted(1, 1, -2, -2));
    }
    else
    {
        QPen pen(QColor(154, 160, 166), 1);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawRect(rect().adjusted(0, 0, -1, -1));
    }
}

void UWatchChart::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit chartActivated(chartIndex);
    QWidget::mousePressEvent(event);
}

void UWatchChart::saveToJpegSlot()
{
    QPixmap screenShot;
    screenShot = chartView->grab(); //захватываем только текущую вкладку
    QString currentDate = QDateTime::currentDateTime().toString("dd-MM-yy HH-mm"); //не ставить . и :, иначе не создает расшираение

    //работа с путем к папке screenshot
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();

    //проверяем, есть ли папка screenshots
    //если нет, то создаем
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
        //говорим что все хорошо и где натйи скриншот
        std::cout<<"chart save succes"<<std::endl;
        QMessageBox messageBox;
        messageBox.setText("Chart save successfully!");
        messageBox.setInformativeText("saved in folder \"screenshots\"");
        messageBox.setWindowTitle("Saving chart");
        messageBox.setIcon(QMessageBox::Information);
        messageBox.setStandardButtons(QMessageBox::Cancel);
        messageBox.exec();
    }
    else std::cout<<"chart save not succes"<<std::endl;  //что-то не так
}

 void UWatchChart::restoreAxes()
 {
     restoreInitialAxesState();
     emit UpdateTabGuiSignal(false);
 }

void UWatchChart::setVizKind(NMSDK::Plot::VizKind kind)
{
    vizKind = kind;
    for (int i = 0; i < series.size(); ++i)
    {
        if (series[i]
            && series[i]->vizKind == NMSDK::Plot::VizKind::TimeSeries
            && (kind == NMSDK::Plot::VizKind::XYLine || kind == NMSDK::Plot::VizKind::XYScatter))
        {
            // Keep existing time series as-is; panel-level viz is a default for new series.
        }
    }
}

bool UWatchChart::isLegendVisible() const
{
    return m_legendVisible;
}

void UWatchChart::setLegendVisible(bool visible)
{
    m_legendVisible = visible;
    if (chart && chart->legend())
        chart->legend()->setVisible(visible);
}

bool UWatchChart::isTitleVisible() const
{
    return m_titleVisible;
}

void UWatchChart::setTitleVisible(bool visible)
{
    m_titleVisible = visible;
    if (!chart)
        return;
    if (!visible)
        chart->setTitle(QString());
}

void UWatchChart::setInteractionTrackLatest(bool track)
{
    isAxisXtrackable = track;
    if (actTrack)
        actTrack->setChecked(track);
}

void UWatchChart::setInteractionPan(bool pan)
{
    if (!chartView)
        return;
    if (pan)
    {
        chartView->setRubberBand(QChartView::NoRubberBand);
        chartView->setDragMode(QGraphicsView::ScrollHandDrag);
        if (actPan)
            actPan->setChecked(true);
        if (actBoxZoom)
            actBoxZoom->setChecked(false);
    }
    else
    {
        chartView->setDragMode(QGraphicsView::NoDrag);
        chartView->setRubberBand(QChartView::RectangleRubberBand);
        if (actPan)
            actPan->setChecked(false);
        if (actBoxZoom)
            actBoxZoom->setChecked(true);
    }
}

void UWatchChart::resetViewport()
{
    restoreInitialAxesState();
    isAxisXtrackable = true;
    if (actTrack)
        actTrack->setChecked(true);
    emit UpdateTabGuiSignal(false);
}

void UWatchChart::onModePan()
{
    setInteractionPan(true);
}

void UWatchChart::onModeBoxZoom()
{
    setInteractionPan(false);
}

void UWatchChart::onModeTrack()
{
    setInteractionTrackLatest(actTrack && actTrack->isChecked());
}

void UWatchChart::onModeReset()
{
    resetViewport();
}

void UWatchChart::connectSerieTooltip(UWatchSerie* serie)
{
    if (!serie)
        return;
    connect(serie, &QXYSeries::hovered, this, [this, serie](const QPointF& point, bool state) {
        if (!state || !chartView)
        {
            if (chartView)
                chartView->setToolTip(QString());
            return;
        }
        QString tip = QStringLiteral("%1\nX=%2  Y=%3")
                          .arg(serie->name())
                          .arg(point.x(), 0, 'g', 6)
                          .arg(point.y(), 0, 'g', 6);
        tip += QStringLiteral("\nY: %1.%2[%3,%4]")
                   .arg(serie->nameComponent, serie->nameProperty)
                   .arg(serie->Jx)
                   .arg(serie->Jy);
        if (!serie->xNameComponent.isEmpty())
        {
            tip += QStringLiteral("\nX: %1.%2[%3,%4]")
                       .arg(serie->xNameComponent, serie->xNameProperty)
                       .arg(serie->xJx)
                       .arg(serie->xJy);
        }
        else
        {
            tip += QStringLiteral("\nX: time");
        }
        chartView->setToolTip(tip);
    });
}

NMSDK::Plot::PlotPanel UWatchChart::toPlotPanel() const
{
    NMSDK::Plot::PlotPanel panel;
    panel.id = QStringLiteral("graph_%1").arg(chartIndex);
    panel.viz = vizKind;
    panel.title = chart ? chart->title() : QString();
    panel.axisXName = axisX ? axisX->titleText() : QString();
    panel.axisYName = axisY ? axisY->titleText() : QString();
    panel.axisYMin = axisY ? axisY->min() : -1.0;
    panel.axisYMax = axisY ? axisY->max() : 1.0;
    panel.axisXMin = axisX ? axisX->min() : 0.0;
    panel.axisXMax = axisX ? axisX->max() : 1.0;
    panel.axisXRange = axisXrange;
    panel.legendVisible = m_legendVisible;
    panel.titleVisible = m_titleVisible;
    panel.trackLatest = isAxisXtrackable;
    if (chartView && chartView->dragMode() == QGraphicsView::ScrollHandDrag)
        panel.interaction = NMSDK::Plot::InteractionMode::Pan;
    else if (isAxisXtrackable)
        panel.interaction = NMSDK::Plot::InteractionMode::TrackLatest;
    else
        panel.interaction = NMSDK::Plot::InteractionMode::FrozenZoom;
    for (int i = 0; i < series.size(); ++i)
    {
        if (series[i])
            panel.series.push_back(series[i]->toPlotSeries());
    }
    return panel;
}

void UWatchChart::applyPlotPanelMeta(const NMSDK::Plot::PlotPanel& panel)
{
    setVizKind(panel.viz);
    setChartTitle(panel.title);
    setAxisXname(panel.axisXName);
    setAxisYname(panel.axisYName);
    setAxisYmin(panel.axisYMin);
    setAxisYmax(panel.axisYMax);
    setAxisXrange(panel.axisXRange);
    const bool xy = panel.viz == NMSDK::Plot::VizKind::XYLine
                    || panel.viz == NMSDK::Plot::VizKind::XYScatter;
    if (xy && panel.axisXMax > panel.axisXMin)
    {
        setAxisXmin(panel.axisXMin);
        setAxisXmax(panel.axisXMax);
    }
    setLegendVisible(panel.legendVisible);
    setTitleVisible(panel.titleVisible);
    isAxisXtrackable = panel.trackLatest;
    if (panel.interaction == NMSDK::Plot::InteractionMode::Pan)
        setInteractionPan(true);
    else
        setInteractionPan(false);
    if (actTrack)
        actTrack->setChecked(panel.trackLatest || panel.interaction == NMSDK::Plot::InteractionMode::TrackLatest);
    fixInitialAxesState();
}

