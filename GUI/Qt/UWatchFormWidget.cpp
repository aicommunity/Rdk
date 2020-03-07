#include "UWatchFormWidget.h"
#include "ui_UWatchFormWidget.h"

UWatchWidgetForm::UWatchWidgetForm(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::UWatchFormWidget)
{
 ui->setupUi(this);
}

UWatchWidgetForm::~UWatchWidgetForm()
{
 delete ui;
}
