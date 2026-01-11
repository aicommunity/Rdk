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
    
    // Версия приложения
    ui->label_2->setText(QCoreApplication::applicationVersion());

    // Версия сборки
    ui->label_4->setText(RDK_APP_VERSION);

    // Пользователь
    QString user_text;
    if(!app->GetUserName().empty())
    {
     user_text = app->GetUserName().c_str();
     if(app->GetUserId()>=0)
      user_text += ":" + QString::fromStdString(RDK::sntoa(app->GetUserId()));
    }
    else
     user_text = "Unknown";
    ui->label_3->setText(user_text);

#ifndef RDK_APP_URL
#define RDK_APP_URL ""
#endif
#ifndef RDK_APP_LICENSE
#define RDK_APP_LICENSE ""
#endif
    // URL как кликабельная ссылка
    QString url_text = QString(RDK_APP_URL);
    if(!url_text.isEmpty())
    {
     url_text = QString("<a href=\"%1\">%1</a>").arg(url_text);
    }
    ui->labelUrl->setText(url_text);
    
    // Лицензия
    ui->labelLicense->setText(RDK_APP_LICENSE);
}

UAboutDialog::~UAboutDialog()
{
    delete ui;
}
