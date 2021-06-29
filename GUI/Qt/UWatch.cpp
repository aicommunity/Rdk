#include "UWatch.h"
#include "ui_UWatch.h"

UWatch::UWatch(QWidget *parent, RDK::UApplication* app)
    : UVisualControllerMainWidget(parent, app), ui(new Ui::UWatch)
{
    ui->setupUi(this);
    setAccessibleName("UWatch");
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


void UWatch::on_actionCreate_tab_triggered()
{
    createTab();
}

void UWatch::on_actionSeries_option_triggered()
{
    if(!getCurrentTab())
     return;
    seriesOption = new UWatchSeriesOption(this);
    seriesOption->setWindowTitle("Series option");
    //seriesOption->setModal(true);
    seriesOption->show();

    //UWatchSeriesOption seriesOption(this);
    //seriesOption.setModal(true);
    //seriesOption.show();
    //seriesOption.exec();
}

void UWatch::on_actionCharts_option_triggered()
{
    if(!getCurrentTab())
     return;

    chartOption = new UWatchChartOption(this);
    chartOption->setModal(true);
    chartOption->setWindowTitle("Charts option");
    chartOption->show();
    //chartOption->exec();

}

void UWatch::createTab()
{
    //создаем каждую новую вкладку с именем tab + номер
    tab.push_back(new UWatchTab(this));
    ui->tabWidget->addTab(tab.last(), QString("tab_%0").arg(ui->tabWidget->count()+1));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    tab.last()->setAccessibleName(QString("tab_%0").arg(ui->tabWidget->count()));
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

void UWatch::on_actionTake_screenshot_triggered()
{
 if(!getCurrentTab())
  return;
    QPixmap screenShot;
    screenShot = getCurrentTab()->grab(); //захватываем только текущую вкладку
    QString currentDate = QDateTime::currentDateTime().toString("dd-MM-yy HH-mm"); //текущая дата

    //работа с путем к папке screenshot
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();

    if(!tab.count())
    {
        //если нет вкладок, вежливо просим ее слздать
        QMessageBox messageBox;
        messageBox.setText("Ohhh, smth goes wrong...");
        messageBox.setInformativeText("Create tab and try again");
        messageBox.setWindowTitle("Screenshot");
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setStandardButtons(QMessageBox::Cancel);
        messageBox.exec();
        return;
    }

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

    if(screenShot.save((dir.path()+"/screenshots/screenshot " + currentDate + ".jpeg")))
    {
        //говорим что все хорошо и где натйи скриншот
        //std::cout<<(dir.path()+"/screenshots/screenshot " + currentDate + ".jpeg").toStdString()<<std::endl;
        std::cout<<"screenshot taken succes"<<std::endl;
        QMessageBox messageBox;
        messageBox.setText("Screenshot taken successfully!");
        messageBox.setInformativeText("saved in folder \"screenshots\"");
        messageBox.setWindowTitle("Screenshot");
        messageBox.setIcon(QMessageBox::Information);
        messageBox.setStandardButtons(QMessageBox::Cancel);
        messageBox.exec();
    }
    else std::cout<<"screenshot taken not succes"<<std::endl;  //что-то пошло не так

}

// Обновление интерфейса
void UWatch::AUpdateInterface(void){}

// Возврат интерфейса в исходное состояние
void UWatch::AClearInterface(void){}

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
    xml.WriteInteger("TabCount", tab.count());
    xml.SelectNodeForce("Tabs");

    for(int i=0; i < tab.count(); i++)
    {
        if(ui->tabWidget->indexOf(tab.at(i)) == -1)
            continue;
        QString tab_name = ui->tabWidget->tabText(ui->tabWidget->indexOf(tab.at(i)));
        xml.WriteString("name_"+RDK::sntoa(i+1), tab_name.toStdString().c_str());
    }
    xml.SelectUp();
}

// Загружает параметры интерфейса из xml
void UWatch::ALoadParameters(RDK::USerStorageXML &xml)
{
    // Очистка существующих табов
    int tab_size = tab.size();
    for(int i=0; i < tab_size; i++)
    {
        ui->tabWidget->removeTab(0);
        delete tab[0];
        tab.remove(0);
    }

    int count=xml.ReadInteger("TabCount", 0);

    for(int i=0; i < count; i++)
        createTab();

    xml.SelectNodeForce("Tabs");
    for(int i=0; i < tab.count(); i++)
    {
        QString tab_name = xml.ReadString("tab_"+RDK::sntoa(i+1), "tab_" + RDK::sntoa(i+1)).c_str();
        ui->tabWidget->setTabText(i, tab_name);
        tab.at(i)->setAccessibleName(tab_name);
    }
    xml.SelectUp();
}
