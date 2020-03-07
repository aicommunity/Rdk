#include "UWatchFormWidget.h"
#include "ui_UWatchFormWidget.h"

UWatchFormWidget::UWatchFormWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::UWatchFormWidget)
{
 ui->setupUi(this);
}

UWatchFormWidget::~UWatchFormWidget()
{
 delete ui;
}

void UWatchFormWidget::on_pushButton_2_clicked()
{

}
