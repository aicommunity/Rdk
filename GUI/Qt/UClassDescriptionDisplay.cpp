#include "UClassDescriptionDisplay.h"
#include "ui_UClassDescriptionDisplay.h"


UClassDescriptionDisplay::UClassDescriptionDisplay(std::string class_name, QWidget *parent, RDK::UApplication *app):
    UVisualControllerWidget(parent, app),
    ClassName(class_name),
    ui(new Ui::UClassDescriptionDisplay)
{
    ui->setupUi(this);
    ui->labelClassName->setText(QString("Class Name: ") + ClassName.c_str());

    ChangeClassDescription(class_name);

    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->pushButtonSave,   &QPushButton::clicked, this, &UClassDescriptionDisplay::SaveDescription);
}

void UClassDescriptionDisplay::SaveDescription()
{
    ClassDescription->SetHeader(ui->textEditHeader->toPlainText().toStdString());
    ClassDescription->SetDescription(ui->textEditDescription->toPlainText().toStdString());

    auto storage = RDK::GetStorageLock();
    storage->SaveClassDescriptionToFile(ClassName);
}

void UClassDescriptionDisplay::ChangeClassDescription(const std::string& class_name)
{
    ClassName = class_name;

    ui->labelClassName->setText(QString("Class Name: ") + ClassName.c_str());
    ui->textEditHeader->setText("");
    ui->textEditDescription->setText("");

    auto storage = RDK::GetStorageLock();
    ClassDescription = storage->GetClassDescription(ClassName, true);

    if(ClassDescription)
    {
        ui->textEditHeader->setText(ClassDescription->GetHeader().c_str());
        ui->textEditDescription->setText(ClassDescription->GetDescription().c_str());
    }
    else
    {
        ClassDescription = new RDK::UContainerDescription();
        ClassDescription->SetClassNameValue(ClassName);
        try
        {
            storage->SetClassDescription(ClassName, ClassDescription);
        }
        catch(RDK::UException &ex)
        {
            delete ClassDescription;
        }
    }
}

const Ui::UClassDescriptionDisplay* UClassDescriptionDisplay::GetUi() const
{
    return ui;
}

UClassDescriptionDisplay::~UClassDescriptionDisplay()
{
    delete ui;
}
