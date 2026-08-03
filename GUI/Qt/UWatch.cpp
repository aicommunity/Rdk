#include "UWatch.h"
#include "ui_UWatch.h"
#include <QDebug>
#include <QStringList>
#include <QStyle>
#include <QTabBar>
#include <QTimer>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>

UWatch::UWatch(QWidget *parent, RDK::UApplication* app)
    : UVisualControllerMainWidget(parent, app), ui(new Ui::UWatch)
{
    ui->setupUi(this);
    setAccessibleName("UWatch");
    
    // Стили для табов теперь применяются через глобальные стили из QSS
    // Это позволяет правильно работать с темной и светлой темами
    
    //создаем первую вкладку
    //createTab();
}

UWatch::~UWatch()
{
    delete ui;
}

UWatchTab *UWatch::getCurrentTab()
{
 if(ui->tabWidget->currentIndex()<0 || ui->tabWidget->currentIndex()>=tab.size())
  return 0;
 return tab[ui->tabWidget->currentIndex()];
}

UWatchTab *UWatch::ensureCurrentTab()
{
    if(UWatchTab* cur = getCurrentTab())
        return cur;
    createTab();
    return getCurrentTab();
}


void UWatch::on_actionCreate_tab_triggered()
{
    createTab();
}

void UWatch::on_actionLayout_settings_triggered()
{
    UWatchTab* current_tab = getCurrentTab();
    if(!current_tab)
     return;

    current_tab->layoutOptionTriggered();
}

void UWatch::on_actionChart_settings_triggered()
{
    UWatchTab* current_tab = getCurrentTab();
    if(!current_tab)
     return;

    current_tab->chartsOptionTriggered();
}

void UWatch::on_actionSeries_settings_triggered()
{
    UWatchTab* current_tab = getCurrentTab();
    if(!current_tab)
     return;

    current_tab->seriesOptionTriggered();
}

void UWatch::on_actionSave_chart_triggered()
{
    UWatchTab* current_tab = getCurrentTab();
    if (!current_tab || current_tab->countGraphs() <= 0)
        return;
    current_tab->onSaveChartAsRequested(current_tab->activeChartIndex());
}

void UWatch::on_actionSave_all_charts_triggered()
{
    UWatchTab* current_tab = getCurrentTab();
    if (!current_tab || current_tab->countGraphs() <= 0)
        return;

    QString startDir = current_tab->savedWatchesRoot();
    if (startDir.isEmpty())
    {
        QMessageBox::warning(this, tr("Save all charts"),
                             tr("Open a project first so charts can be saved under the configuration folder."));
        return;
    }
    QDir().mkpath(startDir);
    const QString dir = QFileDialog::getExistingDirectory(
        this,
        tr("Save all charts"),
        startDir,
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;

    const int n = current_tab->exportAllChartsToDirectory(dir, QStringLiteral("png"));
    if (n <= 0)
        QMessageBox::warning(this, tr("Save all charts"), tr("No charts were saved."));
}

void UWatch::on_actionQuick_save_triggered()
{
    UWatchTab* current_tab = getCurrentTab();
    if (!current_tab || current_tab->countGraphs() <= 0)
        return;
    const int n = current_tab->quickSaveAllCharts();
    if (n <= 0)
        return; // errors already shown when path missing
}

void UWatch::createTab()
{
    // пока что так
    int index = 1;
    if(!tab.empty())
        index = tab.last()->accessibleName().replace("tab_","").toInt()+1;

    const QString tabKey = QString("tab_%0").arg(index);

    //создаем каждую новую вкладку с именем tab + номер
    // accessibleName задаём сразу: CalcFullName() → ключ XML Interface.xml
    tab.push_back(new UWatchTab(this));
    tab.last()->setAccessibleName(tabKey);

    ui->tabWidget->addTab(tab.last(), tabKey);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);

    // Обновляем стили табов после создания новой вкладки
    QTimer::singleShot(0, this, [this]() {
        if(ui && ui->tabWidget)
        {
            QTabBar* tabBar = ui->tabWidget->tabBar();
            if(tabBar)
            {
                tabBar->setStyleSheet("");
                tabBar->style()->unpolish(tabBar);
                tabBar->style()->polish(tabBar);
                tabBar->update();
            }
        }
    });
}

void UWatch::deleteTab(int index)
{
    //спрашиваем юзера точно ли он уверен в закрытие вкладки
    QMessageBox messageBox;
    messageBox.setText("Are you sure you want to close the tab?");
    messageBox.setInformativeText("All data will be lost");
    messageBox.setWindowTitle("Closing tab");
    messageBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
    messageBox.setIcon(QMessageBox::Question);

    if(messageBox.exec() == QMessageBox::Yes)
    {
        ui->tabWidget->removeTab(index);
        delete tab[index];
        tab.remove(index);
    }
    else return;
}

void UWatch::on_tabWidget_tabCloseRequested(int index)
{
    deleteTab(index);
}


// Обновление интерфейса
void UWatch::AUpdateInterface(void){}

// Возврат интерфейса в исходное состояние
void UWatch::AClearInterface(void)
{
    int count=tab.count();
    for(int i=count-1;i>=0;i--)
    {
        ui->tabWidget->removeTab(i);
        delete tab[i];
        tab.remove(i);
    }
}

// Метод, вызываемый после загрузки проекта
void UWatch::AAfterLoadProject(void){}

// Метод, вызываемый перед закрытием проекта
void UWatch::ABeforeCloseProject(void){}

// Метод, вызываемый перед сбросом модели
void UWatch::ABeforeReset(void){}

// Метод, вызываемый после сброса модели
void UWatch::AAfterReset(void){}

// Метод, вызываемый перед шагом расчета
void UWatch::ABeforeCalculate(void){}

// Метод, вызываемый после шага расчета
void UWatch::AAfterCalculate(void){}

// Сохраняет параметры интерфейса в xml
void UWatch::ASaveParameters(RDK::USerStorageXML &xml)
{
    // Штатно: родитель пишет только реестр вкладок.
    // Тело (сетка/серии) сохраняет каждый UWatchTab через
    // UIVisualControllerStorage → SaveParameters → ASaveParameters.
    xml.WriteInteger("TabCount", tab.count());
    xml.SelectNodeForce("Tabs");

    for(int i=0; i < tab.count(); i++)
    {
        if(!tab.at(i) || ui->tabWidget->indexOf(tab.at(i)) == -1)
            continue;
        // Ключ XML = accessibleName (CalcFullName у вкладки).
        QString tab_key = tab.at(i)->accessibleName();
        if(tab_key.isEmpty())
            tab_key = ui->tabWidget->tabText(ui->tabWidget->indexOf(tab.at(i)));
        xml.WriteString("name_"+RDK::sntoa(i+1), tab_key.toStdString().c_str());
    }
    xml.SelectUp();
}

// Загружает параметры интерфейса из xml
void UWatch::ALoadParameters(RDK::USerStorageXML &xml)
{
    // Штатно: пересоздаём вкладки и выставляем accessibleName.
    // PlotDocument каждой вкладки подтянет storage на следующем проходе
    // (см. UIVisualControllerStorage::LoadParameters).
    int tab_size = tab.size();
    for(int i=0; i < tab_size; i++)
    {
        ui->tabWidget->removeTab(0);
        delete tab[0];
        tab.remove(0);
    }

    int count=xml.ReadInteger("TabCount", 0);

    xml.SelectNodeForce("Tabs");
    QStringList names;
    names.reserve(count);
    for(int i=0; i < count; i++)
    {
        names << QString::fromStdString(
            xml.ReadString("name_"+RDK::sntoa(i+1), "tab_" + RDK::sntoa(i+1)));
    }
    xml.SelectUp();

    for(int i=0; i < count; i++)
    {
        createTab();
        const QString& tab_key = names.at(i);
        ui->tabWidget->setTabText(i, tab_key);
        tab.at(i)->setAccessibleName(tab_key);
    }
}

void UWatch::on_tabWidget_currentChanged(int index)
{
 if(index >=0 && index<tab.size())
  tab[index]->UpdateInterface();
}

void UWatch::updateTheme()
{
    // Обновляем стили табов
    if(ui && ui->tabWidget)
    {
        QTabBar* tabBar = ui->tabWidget->tabBar();
        if(tabBar)
        {
            // Очищаем локальные стили, чтобы применились глобальные из QSS
            tabBar->setStyleSheet("");
            tabBar->style()->unpolish(tabBar);
            tabBar->style()->polish(tabBar);
            tabBar->update();
        }
    }
    
    // Обновляем стили всех табов и их графиков
    for(int i = 0; i < tab.count(); i++)
    {
        if(tab[i])
        {
            tab[i]->updateTheme();
        }
    }
}

