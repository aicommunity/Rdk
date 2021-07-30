#include "UClassDescriptionDisplay.h"
#include "ui_UClassDescriptionDisplay.h"


UClassDescriptionDisplay::UClassDescriptionDisplay(std::string class_name, QWidget *parent, RDK::UApplication *app):
    UVisualControllerWidget(parent, app),
    ClassName(class_name),
    ClassDescription(NULL),
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

    // Если описания не существовало
    if(!storage->GetClassDescription(ClassName, true))
    {
        if(QMessageBox::question(this, "Information",
                                 "There is no description for this class. \n Create a description?",
                                 QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
        {
            storage->SetClassDescription(ClassName, ClassDescription);
            storage->SaveClassDescriptionToFile(ClassName);
        }
    }
    // Если существовало, просто сохраняем
    else
    {
        storage->SaveClassDescriptionToFile(ClassName);
    }
}

void UClassDescriptionDisplay::ChangeClassDescription(const std::string& class_name)
{
    if(ClassName == class_name)
        return;

    ClassName = class_name;

    ui->labelClassName->setText(QString("Class Name: ") + ClassName.c_str());
    ui->textEditHeader->setText("");
    ui->textEditDescription->setText("");

    if(ClassName.empty())
        return;

    auto storage = RDK::GetStorageLock();
    ClassDescription = storage->GetClassDescription(ClassName, true);

    if(ClassDescription)
    {
        ui->textEditHeader->setText(ClassDescription->GetHeader().c_str());
        ui->textEditDescription->setText(ClassDescription->GetDescription().c_str());
    }
    else
    {

        if(ClassDescription!=NULL)
        {
            delete ClassDescription;
            ClassDescription = NULL;
        }
        ClassDescription = new RDK::UContainerDescription();
        ClassDescription->SetClassNameValue(ClassName);
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
