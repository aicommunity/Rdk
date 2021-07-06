#include "UWatch.h"
#include "ui_UWatch.h"

UWatch::UWatch(QWidget *parent, RDK::UApplication* app)
    : UVisualControllerMainWidget(parent, app), ui(new Ui::UWatch)
{
    ui->setupUi(this);
    setAccessibleName("UWatch");
    //������� ������ �������
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
    UWatchTab* current_tab = getCurrentTab();
    if(!current_tab)
     return;

    current_tab->seriesOptionTriggered();
}

void UWatch::on_actionCharts_option_triggered()
{
    UWatchTab* current_tab = getCurrentTab();
    if(!current_tab)
     return;

    current_tab->chartsOptionTriggered();
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


// ���������� ����������
void UWatch::AUpdateInterface(void){}

// ������� ���������� � �������� ���������
void UWatch::AClearInterface(void){}

// �����, ���������� ����� �������� �������
void UWatch::AAfterLoadProject(void){}

// �����, ���������� ����� ��������� �������
void UWatch::ABeforeCloseProject(void){}

// �����, ���������� ����� ������� ������
void UWatch::ABeforeReset(void){}

// �����, ���������� ����� ������ ������
void UWatch::AAfterReset(void){}

// �����, ���������� ����� ����� �������
void UWatch::ABeforeCalculate(void){}

// �����, ���������� ����� ���� �������
void UWatch::AAfterCalculate(void){}

// ��������� ��������� ���������� � xml
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

// ��������� ��������� ���������� �� xml
void UWatch::ALoadParameters(RDK::USerStorageXML &xml)
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
