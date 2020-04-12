#include "UWatchSettingsDialog.h"
#include "ui_UWatchSettingsDialog.h"

UWatchSettingsDialog::UWatchSettingsDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::UWatchSettingsDialog)
{
 ui->setupUi(this);
}

UWatchSettingsDialog::~UWatchSettingsDialog()
{
 delete ui;
}
