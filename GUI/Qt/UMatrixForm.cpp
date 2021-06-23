#include "UMatrixForm.h"
#include "ui_UMatrixForm.h"

UMatrixForm::UMatrixForm(QWidget *parent, RDK::UApplication* app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UMatrixForm)
{
    ui->setupUi(this);

    connect(ui->tableWidget->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(onItemSelected()));


    AlwaysUpdateFlag = true;
    UpdateInterval=30;
    CheckModelFlag=true;
    SelectedRow=SelectedCol=-1;
    PropertyType=0;
}

UMatrixForm::~UMatrixForm()
{
    delete ui;
}

// ���� force_update == true, �� ��������� �����������
// ��� ����������� �� UpdateInterval
void UMatrixForm::AUpdateInterface(void)
{
    if(ComponentMatrixName.empty())
        return;

    if(PropertyMatrixName.empty())
        return;

    if(PropertyType <1 || PropertyType>4)
        return;

    ui->label->setText((ComponentMatrixName+std::string(":")+PropertyMatrixName).c_str());

    QItemSelectionModel *sel_m = ui->tableWidget->selectionModel();

    int row_id=-1;
    int col_id=-1;

    if(sel_m->hasSelection())
    {
        QModelIndexList ids = sel_m->selectedIndexes();
        if(ids.size()>0)
        {
            row_id = ids.first().row();
            col_id = ids.first().column();
        }
    }


    switch(PropertyType)
    {
    case 1:
    {
        RDK::MDMatrix<double> m;
        {
            RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();
            m=engine->Model_GetComponentPropertyData<RDK::MDMatrix<double> >(ComponentMatrixName.c_str(), PropertyMatrixName.c_str());
        }

        ui->tableWidget->clear();
        ui->tableWidget->setRowCount(m.GetRows());
        ui->tableWidget->setColumnCount(m.GetCols());

        QStringList labels;

        for(int k=0; k<m.GetRows();k++)
            labels << QString::number(k);
        ui->tableWidget->setVerticalHeaderLabels(labels);

        labels.clear();

        for(int k=0; k<m.GetCols();k++)
            labels << QString::number(k);
        ui->tableWidget->setHorizontalHeaderLabels(labels);

        for(int i=0;i<m.GetRows();i++)
        {
            for(int j=0;j<m.GetCols();j++)
            {
                ui->tableWidget->setItem(i, j, new QTableWidgetItem(QString::number(m(i,j))));
            }
        }
    }
    break;

    case 2:
    {
        RDK::MDMatrix<int> m;
        {
            RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();
            m=engine->Model_GetComponentPropertyData<RDK::MDMatrix<int> >(ComponentMatrixName.c_str(), PropertyMatrixName.c_str());
        }

        ui->tableWidget->clear();
        ui->tableWidget->setColumnCount(m.GetCols());
        ui->tableWidget->setRowCount(m.GetRows());

        QStringList labels;

        for(int k=0; k<m.GetRows();k++)
            labels << QString::number(k);
        ui->tableWidget->setVerticalHeaderLabels(labels);

        labels.clear();

        for(int k=0; k<m.GetCols();k++)
            labels << QString::number(k);
        ui->tableWidget->setHorizontalHeaderLabels(labels);


        for(int i=0;i<m.GetRows();i++)
        {
            for(int j=0;j<m.GetCols();j++)
            {
                ui->tableWidget->setItem(i, j, new QTableWidgetItem(QString::number(m(i,j))));
            }
        }
    }
    break;
    /*
    case 3:
    {
        RDK::MDVector<double> m;
        {
            RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();
            m=engine->Model_GetComponentPropertyData<RDK::MDVector<double> >(ComponentMatrixName.c_str(), PropertyMatrixName.c_str());
        }
        //ReadParameterValue(ComponentMatrixName, PropertyMatrixName, m);

        StringGrid->ColCount=m.GetCols()+1;
        StringGrid->RowCount=2;
        StringGrid->Cells[0][1]="0";
        for(int j=0;j<m.GetCols();j++)
        {
            StringGrid->Cells[j+1][0]=IntToStr(j);
            StringGrid->Cells[j+1][1]=FloatToStr(m(j));
        }
    }
    break;

    case 4:
    {
        RDK::MDVector<int> m;
        {
            RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();
            m=engine->Model_GetComponentPropertyData<RDK::MDVector<int> >(ComponentMatrixName.c_str(), PropertyMatrixName.c_str());
        }
        //ReadParameterValue(ComponentMatrixName, PropertyMatrixName, m);

        StringGrid->ColCount=m.GetCols()+1;
        StringGrid->RowCount=2;
        StringGrid->Cells[0][1]="0";
        for(int j=0;j<m.GetCols();j++)
        {
            StringGrid->Cells[j+1][0]=IntToStr(j);
            StringGrid->Cells[j+1][1]=FloatToStr(m(j));
        }
    }
    break;*/
    }


    if(row_id>=0 && col_id >=0)
    {
        QModelIndex temp = ui->tableWidget->model()->index(row_id, col_id);
        sel_m->select(temp, QItemSelectionModel::Select);
    }
}

// ������� ���������� � �������� ���������
void UMatrixForm::AClearInterface(void)
{
    ComponentMatrixName="";
    PropertyMatrixName="";
    SelectedRow=SelectedCol=-1;
    PropertyType=0;
}

/// �������� ������� ��� ����������
bool UMatrixForm::SelectMatrix(const std::string &comp_name, const std::string &prop_name)
{
    RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock<RDK::UNet>();
    if(!model)
        return false;

    RDK::UEPtr<RDK::UNet> comp;
    if(comp_name.empty())
    {
        comp=RDK::GetModel<RDK::UNet>();
    }
    else
    {
        comp=model->GetComponentL<RDK::UNet>(comp_name,true);
        if(!comp)
            return false;
    }

    RDK::UIProperty* prop=comp->FindProperty(prop_name);

    if(!prop)
     return false;

    if(prop->GetLanguageType() == typeid(RDK::MDMatrix<double>))
    {
        PropertyType=1;
    }
    else
    if(prop->GetLanguageType() == typeid(RDK::MDMatrix<int>))
    {
        PropertyType=2;
    }
    else
    if(prop->GetLanguageType() == typeid(RDK::MDVector<double>))
    {
        PropertyType=3;
    }
    else
    if(prop->GetLanguageType() == typeid(RDK::MDVector<int>))
    {
        PropertyType=4;
    }
    else
    {
        return false;
    }

    ComponentMatrixName=comp_name;
    PropertyMatrixName=prop_name;
    SelectedRow=SelectedCol=0;

    UpdateInterface(true);
    return true;
}
