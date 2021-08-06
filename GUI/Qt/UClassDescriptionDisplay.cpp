#include "UClassDescriptionDisplay.h"
#include "ui_UClassDescriptionDisplay.h"


UClassDescriptionDisplay::UClassDescriptionDisplay(std::string class_name, QWidget *parent, RDK::UApplication *app):
    UVisualControllerWidget(parent, app),
    ClassDescription(NULL),
    ui(new Ui::UClassDescriptionDisplay)
{
    ui->setupUi(this);

    ChangeClassDescription(class_name);

    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->pushButtonSave,   &QPushButton::clicked, this, &UClassDescriptionDisplay::SaveDescription);
}

UClassDescriptionDisplay::~UClassDescriptionDisplay()
{
    delete ui;
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

    DefaultGUIState();

    ClassName = class_name;

    ui->labelClassNamVal->setText(QString::fromStdString(ClassName));

    if(ClassName.empty())
        return;

    auto storage = RDK::GetStorageLock();
    ClassDescription = storage->GetClassDescription(ClassName, true);

    if(ClassDescription)
    {
        ui->textEditHeader->setText(ClassDescription->GetHeader().c_str());
        ui->textEditDescription->setText(ClassDescription->GetDescription().c_str());
        FillProperties();
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
        FillProperties();
    }
}

void UClassDescriptionDisplay::FillProperties()
{
    ui->listWidgetProperties->clear();
    std::map<std::string, RDK::UPropertyDescription> props = ClassDescription->GetProperties();

    for (auto i = props.begin(); i != props.end(); i++)
    {
        ui->listWidgetProperties->addItem(QString::fromStdString(i->first));
    }
}

const Ui::UClassDescriptionDisplay* UClassDescriptionDisplay::GetUi() const
{
    return ui;
}

void UClassDescriptionDisplay::DefaultGUIState()
{
    ui->labelClassNamVal->clear();

    ui->textEditHeader->clear();
    ui->textEditDescription->clear();

    ui->textEditHeaderProp->clear();
    ui->textEditDescProp->clear();

    ui->listWidgetProperties->clear();

    ui->spinBoxDataSelecType->setValue(0);
    ui->labelDataSelecTypeDesc->clear();
    ui->labelPropTypeVal->clear();

    ui->lineEditValList->setEnabled(false);
    ui->lineEditStep->setEnabled(false);
}

void UClassDescriptionDisplay::UpdateDataSelectionType(int type)
{
    // 0 - произвольные данные
    // 1 - Checkbox
    // 2 - Диапазон
    // 3 - Список вариантов
    // 4 - Диапазон с заданным шагом
    ui->spinBoxDataSelecType->setValue(type);

    // Отключаем ввод в switch включаем необходимое
    ui->lineEditValList->setEnabled(false);
    ui->lineEditStep->setEnabled(false);

    switch (type)
    {
        case 0:
            ui->labelDataSelecTypeDesc->setText("Arbitrary data");
            ui->lineEditValList->setEnabled(true);
            break;
        case 1:
            ui->labelDataSelecTypeDesc->setText("Checkbox");
            break;
        case 2:
            ui->labelDataSelecTypeDesc->setText("Range");
            ui->lineEditValList->setEnabled(true);
            break;
        case 3:
            ui->labelDataSelecTypeDesc->setText("List of options");
            ui->lineEditValList->setEnabled(true);
            break;
        case 4:
            ui->labelDataSelecTypeDesc->setText("Range with a given step");
            ui->lineEditValList->setEnabled(true);
            ui->lineEditStep->setEnabled(true);
            break;
        default:
            ui->labelDataSelecTypeDesc->setText("Unknown type");
            break;
    }
}

void UClassDescriptionDisplay::on_spinBoxDataSelecType_valueChanged(int arg1)
{
    UpdateDataSelectionType(arg1);
}

void UClassDescriptionDisplay::on_textEditHeaderProp_textChanged()
{
    if(ClassDescription)
    {
        if(ui->listWidgetProperties->currentItem())
        {
            RDK::UPropertyDescription& prop_desc = ClassDescription->GetPropertyDescription(ui->listWidgetProperties->currentItem()->text().toStdString());
            prop_desc.Header = ui->textEditHeaderProp->toPlainText().toStdString();
        }
    }
}

void UClassDescriptionDisplay::on_textEditDescProp_textChanged()
{
    if(ClassDescription)
    {
        if(ui->listWidgetProperties->currentItem())
        {
            RDK::UPropertyDescription& prop_desc = ClassDescription->GetPropertyDescription(ui->listWidgetProperties->currentItem()->text().toStdString());
            prop_desc.Description = ui->textEditDescProp->toPlainText().toStdString();
        }
    }
}

void UClassDescriptionDisplay::on_lineEditValList_textChanged(const QString &arg1)
{
    if(ClassDescription)
    {
        if(ui->listWidgetProperties->currentItem())
        {
            RDK::UPropertyDescription prop_desc = ClassDescription->GetPropertyDescription(ui->listWidgetProperties->currentItem()->text().toStdString());
        }
        //prop_desc.ValueList = ui->lineEditValList->toPlainText().toStdString();
    }
}

void UClassDescriptionDisplay::on_lineEditStep_textChanged(const QString &arg1)
{
    if(ClassDescription)
    {
        if(ui->listWidgetProperties->currentItem())
        {
            RDK::UPropertyDescription& prop_desc = ClassDescription->GetPropertyDescription(ui->listWidgetProperties->currentItem()->text().toStdString());
            prop_desc.Step = ui->lineEditStep->text().toStdString();
        }
    }
}

void UClassDescriptionDisplay::on_listWidgetProperties_currentTextChanged(const QString &currentText)
{
    RDK::UPropertyDescription prop_desc = ClassDescription->GetPropertyDescription(currentText.toStdString());

    ui->textEditHeaderProp->setText(QString::fromStdString(prop_desc.Header));
    ui->textEditDescProp->setText(QString::fromStdString(prop_desc.Description));

    UpdateDataSelectionType(prop_desc.DataSelectionType);

    QString val_list;
    for(auto i = prop_desc.ValueList.begin(); i != prop_desc.ValueList.end(); ++i)
    {
        if(!val_list.isEmpty())
             val_list += ",";
        val_list += QString::fromStdString(*i);
    }
    ui->lineEditValList->setText(val_list);

    ui->lineEditStep->setText(QString::fromStdString(prop_desc.Step));
    ui->labelPropTypeVal->setText(QString::fromStdString(prop_desc.Type));
}
