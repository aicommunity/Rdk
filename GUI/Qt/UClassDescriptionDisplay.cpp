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

    connect(ui->comboBoxProperties, SIGNAL(currentTextChanged(const QString&)), this, SLOT(UpdateProperty(const QString&)));
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
        UpdateProperties();
    }
    else
    {
        if(ClassDescription!=NULL)
        {
            delete ClassDescription;
            ClassDescription = NULL;
        }
        ClassDescription = new RDK::UContainerDescription();
        ClassDescription->SetStorage(storage.Get());
        ClassDescription->SetClassNameValue(ClassName);
        UpdateProperties();
    }
}

void UClassDescriptionDisplay::UpdateProperties()
{
    std::map<std::string, RDK::UPropertyDescription> props = ClassDescription->GetProperties();
    ui->comboBoxProperties->clear();
    for (auto i = props.begin(); i != props.end(); i++)
    {
        ui->comboBoxProperties->addItem(QString::fromStdString(i->first));
    }
}

void UClassDescriptionDisplay::UpdateProperty(const QString& prop_name)
{
    RDK::UPropertyDescription prop_desc = ClassDescription->GetPropertyDescription(prop_name.toStdString());

    ui->textEditHeaderProp->setText(QString::fromStdString(prop_desc.Header));
    ui->textEditDescProp->setText(QString::fromStdString(prop_desc.Description));

    // 0 - произвольные данные
    // 1 - Checkbox
    // 2 - Диапазон
    // 3 - Список вариантов
    // 4 - Диапазон с заданным шагом
    ui->spinBoxDataSelecType->setValue(prop_desc.DataSelectionType);
    if(prop_desc.DataSelectionType==0)
        ui->labelDataSelecTypeDesc->setText("Arbitrary data");
    if(prop_desc.DataSelectionType==1)
        ui->labelDataSelecTypeDesc->setText("Checkbox");
    if(prop_desc.DataSelectionType==2)
        ui->labelDataSelecTypeDesc->setText("Range");
    if(prop_desc.DataSelectionType==3)
        ui->labelDataSelecTypeDesc->setText("List of options");
    if(prop_desc.DataSelectionType==4)
        ui->labelDataSelecTypeDesc->setText("Range with a given step");


    QString val_list;
    for(auto i = prop_desc.ValueList.begin(); i != prop_desc.ValueList.end(); ++i)
    {
        if(!val_list.isEmpty())
             val_list += ",";
        val_list += QString::fromStdString(*i);
    }
    ui->lineEditValList->setText(val_list);

    ui->lineEditStep->setText(QString::fromStdString(prop_desc.Step));
    ui->labelPropType->setText("Property Type: " + QString::fromStdString(prop_desc.Type));
}

const Ui::UClassDescriptionDisplay* UClassDescriptionDisplay::GetUi() const
{
    return ui;
}

UClassDescriptionDisplay::~UClassDescriptionDisplay()
{
    delete ui;
}
