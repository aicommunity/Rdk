#include "UClassDescriptionDisplay.h"
#include "ui_UClassDescriptionDisplay.h"


UClassDescriptionDisplay::UClassDescriptionDisplay(std::string class_name, QWidget *parent) :
    ClassName(class_name),
    QDialog(parent),
    ui(new Ui::UClassDescriptionDisplay)
{
    ui->setupUi(this);
    ui->labelClassName->setText(ClassName.c_str());

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

void UClassDescriptionDisplay::SaveDescription()
{
    ClassDescription->SetHeader(ui->textEditHeader->toPlainText().toStdString());
    ClassDescription->SetDescription(ui->textEditDescription->toPlainText().toStdString());

    auto storage = RDK::GetStorageLock();
    storage->SaveClassDescriptionToFile(ClassName);
}

UClassDescriptionDisplay::~UClassDescriptionDisplay()
{
    delete ui;
}
