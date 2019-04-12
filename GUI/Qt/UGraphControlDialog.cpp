#include "UGraphControlDialog.h"
#include "ui_UGraphControlDialog.h"
#include <iostream>
#include <QDoubleValidator>

UGraphControlDialog::UGraphControlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UGraphControlDialog)
{
    ui->setupUi(this);
    color = -1;
    click = false;
    connect(ui->checkBoxAxisX, SIGNAL(toggled(bool)), this, SLOT(isActiveDoubleSpinBox(bool)));

}

UGraphControlDialog::~UGraphControlDialog()
{
    delete ui;
}

int UGraphControlDialog::getDoubleSpinBoxData() const
{
    return (ui->doubleSpinBox->value());
}

int UGraphControlDialog::getAxisXChangeData() const
{
    if (ui->checkBoxAxisX->isChecked()==false)
        return -1;
    return 1;
}

int UGraphControlDialog::getAxisMaxYChangeData() const
{
    if (ui->checkBoxAxisMaxY->isChecked()==false)
        return -1;
    return 1;
}

int UGraphControlDialog::getAxisMinYChangeData() const
{
    if (ui->checkBoxAxisMinY->isChecked()==false)
        return -1;
    return 1;
}

int UGraphControlDialog::getColor() const
{
    return color;
}

QString UGraphControlDialog::getLableX() const
{
    QString a = ui->textEditlableX->toPlainText();
    return a;

}

QString UGraphControlDialog::getLableY() const
{
    QString a = ui->textEditLableY->toPlainText();
    return a;
    //return "a";
}

bool UGraphControlDialog::getDelInf() const
{
    return click;
}

void UGraphControlDialog::setAxisXChangeData(int axisX) const
{
    if (axisX!=-1)
        ui->checkBoxAxisX->setChecked(true);
    else
        ui->checkBoxAxisX->setChecked(false);
}

void UGraphControlDialog::setAxisMaxYChangeData(int axisY) const
{
    if (axisY!=-1)
        ui->checkBoxAxisMaxY->setChecked(true);
    else
        ui->checkBoxAxisMaxY->setChecked(false);
}
void UGraphControlDialog::setAxisMinYChangeData(int axisY) const
{
    if (axisY!=-1)
        ui->checkBoxAxisMinY->setChecked(true);
    else
        ui->checkBoxAxisMinY->setChecked(false);
}

void UGraphControlDialog::setDecimalPlaces(int i) const
{
    ui->doubleSpinBox->setDecimals(i);
}

void UGraphControlDialog::setDisplayInterval(int i) const
{
    ui->doubleSpinBox->setValue(i);
}

void UGraphControlDialog::setLableX(QString lableX) const
{
    ui->textEditlableX->setText(lableX);
}

void UGraphControlDialog::setLableY(QString lableY) const
{
    ui->textEditLableY->setText(lableY);
}

void UGraphControlDialog::isActiveDoubleSpinBox(bool b)
{
    if (b==true)
    {
        ui->doubleSpinBox->setEnabled(true);
    }
    else
    {
        ui->doubleSpinBox->setEnabled(false);
    }
}

void UGraphControlDialog::on_colorListWidget_currentRowChanged(int currentRow)
{
    color = currentRow;
}

void UGraphControlDialog::on_pushButton_clicked()
{
    click = true;
}
