#include "UMatrixFormDialog.h"
#include "ui_UMatrixFormDialog.h"

UMatrixFormDialog::UMatrixFormDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UMatrixFormDialog)
{
    ui->setupUi(this);

    SelectedRow=SelectedCol=-1;
    PropertyType=0;

  //  connect(ui->buttonBox->Ok,SIGNAL(clicked()),this,SLOT(accept()));
 //   connect(cancel_button,SIGNAL(clicked()),this, SLOT(reject()));

    // Таймер обновления отображаемых данных (каждые 300 мс)
    updateMatrixDataTimer.setInterval(300);
    updateMatrixDataTimer.setSingleShot(false);
    connect(&updateMatrixDataTimer, SIGNAL(timeout()), this, SLOT(UpdateMatrixData()));
    updateMatrixDataTimer.start();
}

UMatrixFormDialog::~UMatrixFormDialog()
{
    updateMatrixDataTimer.stop();
    delete ui;
}

void UMatrixFormDialog::UpdateMatrixData(void)
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

    case 3:
    {
        RDK::MDVector<double> m;
        {
            RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();
            m=engine->Model_GetComponentPropertyData<RDK::MDVector<double> >(ComponentMatrixName.c_str(), PropertyMatrixName.c_str());
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

        for(int j=0;j<m.GetRows();j++)
        {
            ui->tableWidget->setItem(j, 0, new QTableWidgetItem(QString::number(m(j))));
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

        for(int j=0;j<m.GetRows();j++)
        {
            ui->tableWidget->setItem(j, 0, new QTableWidgetItem(QString::number(m(j))));
        }
    }
    break;
    }


    if(row_id>=0 && col_id >=0)
    {
        QModelIndex temp = ui->tableWidget->model()->index(row_id, col_id);
        sel_m->select(temp, QItemSelectionModel::Select);
    }
}

/// Выбирает матрицу для наблюдения
bool UMatrixFormDialog::SelectMatrix(const std::string &comp_name, const std::string &prop_name)
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

    emit UpdateMatrixData();

    return true;
}

void UMatrixFormDialog::on_buttonBox_accepted()
{
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

    SelectedRow = row_id;
    SelectedCol = col_id;
}
