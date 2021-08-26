#include "UClassFavoritesEditor.h"
#include "ui_UClassFavoritesEditor.h"


UClassFavoritesEditor::UClassFavoritesEditor(std::string class_name, QWidget *parent, RDK::UApplication *app):
    UVisualControllerWidget(parent, app),
    ClassName(class_name),
    ui(new Ui::UClassFavoritesEditor)
{
    ui->setupUi(this);

    componentList = new USingleClassListWidget(ClassName, this, application);
    componentList->setEnableTabN(1, false);
    componentList->setEnableTabN(2, false);
    componentList->setEnableTabN(3, false);

    connect(componentList, &USingleClassListWidget::parameterChanged, ui->labelFullPath, &QLabel::setText);

    ui->labelClassName->setText(QString::fromStdString(ClassName));

    ui->verticalLayoutClassProp->addWidget(componentList);

    connect(ui->pushButtonClose, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->pushButtonCreate,   &QPushButton::clicked, this, &UClassFavoritesEditor::CreateFavorite);
}

UClassFavoritesEditor::~UClassFavoritesEditor()
{
    delete ui;
}

void UClassFavoritesEditor::ChangeClass(std::string class_name)
{
    ClassName = class_name;
    ui->labelClassName->setText(QString::fromStdString(ClassName));
    ui->lineEditFavName->clear();
    ui->labelFullPath->clear();
    componentList->ChangeClass(class_name);
}

void UClassFavoritesEditor::CreateFavorite()
{
    if(ui->labelFullPath->text().isEmpty() && ui->lineEditFavName->text().isEmpty())
    {
       return;
    }
    else
    {
        emit CreateNewFavorite(ui->lineEditFavName->text(), ui->labelFullPath->text());
    }
}

