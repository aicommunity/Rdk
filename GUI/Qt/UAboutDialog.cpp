#include "UAboutDialog.h"
#include "ui_UAboutDialog.h"
#include "rdk.h"

UAboutDialog::UAboutDialog(QWidget *parent, RDK::UApplication *app) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    if(!app)
     return;
    if(!app->GetProgramName().empty())
     ui->labelProgramName->setText(app->GetProgramName().c_str());
    else
     ui->labelProgramName->setText("Rdk Core");
    ui->label_2->setText("Version: "+QCoreApplication::applicationVersion());

    QString user_label="User: ";
    if(!app->GetUserName().empty())
    {
     user_label = user_label+app->GetUserName().c_str();
     if(app->GetUserId()>=0)
      user_label = user_label+":"+RDK::sntoa(app->GetUserId()).c_str();
    }
    else
     user_label = user_label + "Unknown";
    ui->label_3->setText(user_label);

    QString build_version="Build rev: ";
    build_version += "app=";
    build_version += RDK::sntoa(RDK_APP_VERSION).c_str();

    ui->label_4->setText(build_version);

    ui->labelUrl->setText(RDK_APP_URL);
    ui->labelLicense->setText(RDK_APP_LICENSE);
}

UAboutDialog::~UAboutDialog()
{
    delete ui;
}
