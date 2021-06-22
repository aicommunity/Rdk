#include "UWatch.h"
#include "ui_UWatch.h"

UWatch::UWatch(QWidget *parent, RDK::UApplication* app)
    : ui(new Ui::UWatch)
{
    ui->setupUi(this);
    setAccessibleName("UWatch");
    RDK::UIVisualControllerStorage::AddInterface(this);
    //������� ������ �������
    //createTab();
}

UWatch::~UWatch()
{
    delete ui;
}

UWatchTab *UWatch::getCurrentTab()
{
    return tab[ui->tabWidget->currentIndex()];
}


void UWatch::on_actionCreate_tab_triggered()
{
    createTab();
}

void UWatch::on_actionSeries_option_triggered()
{

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
    chartOption = new UWatchChartOption(this);
    chartOption->setModal(true);
    chartOption->setWindowTitle("Charts option");
    chartOption->show();
    //chartOption->exec();

}

void UWatch::createTab()
{
    //������� ������ ����� ������� � ������ tab + �����
    tab.push_back(new UWatchTab(this));
    ui->tabWidget->addTab(tab.last(), QString("tab_%0").arg(ui->tabWidget->count()+1));
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    tab.last()->setAccessibleName(QString("tab_%0").arg(ui->tabWidget->count()));
}

void UWatch::deleteTab(int index)
{
    //���������� ����� ����� �� �� ������ � �������� �������
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
    QPixmap screenShot;
    screenShot = getCurrentTab()->grab(); //����������� ������ ������� �������
    QString currentDate = QDateTime::currentDateTime().toString("dd-MM-yy HH-mm"); //������� ����

    //������ � ����� � ����� screenshot
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();

    if(!tab.count())
    {
        //���� ��� �������, ������� ������ �� �������
        QMessageBox messageBox;
        messageBox.setText("Ohhh, smth goes wrong...");
        messageBox.setInformativeText("Create tab and try again");
        messageBox.setWindowTitle("Screenshot");
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setStandardButtons(QMessageBox::Cancel);
        messageBox.exec();
        return;
    }

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

    if(screenShot.save((dir.path()+"/screenshots/screenshot " + currentDate + ".jpeg")))
    {
        //������� ��� ��� ������ � ��� ����� ��������
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
    else std::cout<<"screenshot taken not succes"<<std::endl;  //���-�� ����� �� ���

}

// ���������� ����������
void UWatch::UpdateInterface(bool force_update){}

// ������� ���������� � �������� ���������
void UWatch::ClearInterface(void){}

// ���������� �������� ���������� ����������
long UWatch::GetUpdateInterval(void){return 0;}

// ������ �������� ���������� ����������
bool UWatch::SetUpdateInterval(long value){return true;}

// ���������� ���� ���������� ���������� ���������� ���� ���� �� �� �����
bool UWatch::GetAlwaysUpdateFlag(void){return true;}

// ��������� ������ ���������� �����������
/// ���������� ���� ��������� ����������� � ���� �������� �����
void UWatch::ResetCalculationStepUpdatedFlag(void){}

/// ���������� ���� ��������� ����������� � ���� �������� �����
void UWatch::SetCalculationStepUpdatedFlag(void){}

/// ���������� ��������� ����� ��������� ����������� � ���� �������� �����
bool UWatch::GetCalculationStepUpdatedFlag(void){return true;}

/// ���������� ����� ���������� ���������� (��)
unsigned long long UWatch::GetUpdateTime(void){return 0;}

// �����, ���������� ����� �������� �������
void UWatch::AfterLoadProject(void){}

// �����, ���������� ����� ��������� �������
void UWatch::BeforeCloseProject(void){}

// �����, ���������� ����� ������� ������
void UWatch::BeforeReset(void){}

// �����, ���������� ����� ������ ������
void UWatch::AfterReset(void){}

// �����, ���������� ����� ����� �������
void UWatch::BeforeCalculate(void){}

// �����, ���������� ����� ���� �������
void UWatch::AfterCalculate(void){}

// ���������� ���������� ��� ����������
std::string UWatch::GetName(void){return "";}

// ���������� ������ ���������� ��� ����������
std::string UWatch::CalcFullName(void){return "";}

// ���������� ��� ������ ����������
std::string UWatch::GetClassName(void){return "";}

// ��������� ��������� ���������� � xml
void UWatch::SaveParameters(RDK::USerStorageXML &xml)
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

// ��������� ��������� ���������� �� xml
void UWatch::LoadParameters(RDK::USerStorageXML &xml)
{
    // ������� ������������ �����
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
