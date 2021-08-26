#include "UClassDescriptionDisplay.h"
#include "ui_UClassDescriptionDisplay.h"
#include "UClassFavoritesEditor.h"

UClassDescriptionDisplay::UClassDescriptionDisplay(std::string class_name, QWidget *parent, RDK::UApplication *app):
    UVisualControllerWidget(parent, app),
    ClassDescription(NULL),
    clFavEditor(NULL),
    ui(new Ui::UClassDescriptionDisplay)
{
    ui->setupUi(this);

    ChangeClassDescription(class_name);

    connect(ui->pushButtonCancel, &QPushButton::clicked, this, &QWidget::close);
    connect(ui->pushButtonSave,   &QPushButton::clicked, this, &UClassDescriptionDisplay::SaveDescription);

    ui->lineEditStep->setValidator(new QRegExpValidator(QRegExp("[+-]?\\d*\\.?\\d+"), this));

    // actions для списка избранных
    QAction * createNewFavorite = new QAction("Create New", this);
    QAction * deleteFavorite =    new QAction("Delete", this);

    ui->treeWidgetFavorites->addAction(createNewFavorite);
    ui->treeWidgetFavorites->addAction(deleteFavorite);

    connect(createNewFavorite, SIGNAL(triggered()), this, SLOT(createNewFavoriteSlot()));
    connect(deleteFavorite,    SIGNAL(triggered()), this, SLOT(deleteFavoriteSlot()));


    ui->treeWidgetFavorites->setContextMenuPolicy(Qt::ActionsContextMenu);
}

UClassDescriptionDisplay::~UClassDescriptionDisplay()
{
    if(clFavEditor != NULL)
    {
        delete clFavEditor;
    }
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
    auto storage = RDK::GetStorageLock();

    if(ClassName == class_name)
        return;

    // Если предыдущее описание не сохранено в хранилище
    if(!(storage->GetClassDescription(ClassName, true)))
    {
        if(ClassDescription!=NULL)
        {
            delete ClassDescription;
            ClassDescription = NULL;
        }
    }
    else
    {
        ClassDescription = NULL;
    }

    ClassName = class_name;

    ui->labelClassNamVal->setText(QString::fromStdString(ClassName));

    if(ClassName.empty())
    {
        DefaultGUIState();
        if(clFavEditor != NULL)
            clFavEditor->ChangeClass(ClassName);
        return;
    }

    ClassDescription = storage->GetClassDescription(ClassName, true);

    if(ClassDescription)
    {
        ui->textEditHeader->setText(ClassDescription->GetHeader().c_str());
        ui->textEditDescription->setText(ClassDescription->GetDescription().c_str());
        FillProperties();
    }
    else
    {
        DefaultGUIState();
        ClassDescription = new RDK::UContainerDescription();
        ClassDescription->SetStorage(storage.Get());
        ClassDescription->SetClassNameValue(ClassName);
        ui->labelClassNamVal->setText(QString::fromStdString(ClassName));
        FillProperties();
    }

    if(clFavEditor != NULL)
        clFavEditor->ChangeClass(ClassName);
}

void UClassDescriptionDisplay::FillProperties()
{
    ui->listWidgetProperties->clear();
    const std::map<std::string, RDK::UPropertyDescription>& props = ClassDescription->GetProperties();

    for (auto i = props.begin(); i != props.end(); i++)
    {
        ui->listWidgetProperties->addItem(QString::fromStdString(i->first));
    }

    if(ui->listWidgetProperties->count()>0)
        ui->listWidgetProperties->setCurrentRow(0);

    FillFavorites();
}

void UClassDescriptionDisplay::FillFavorites()
{
    ui->treeWidgetFavorites->clear();
    const std::map<std::string, std::string>& favs = ClassDescription->GetFavorites();

    for (auto i = favs.begin(); i != favs.end(); i++)
    {
        QTreeWidgetItem* favoriteItem = new QTreeWidgetItem(ui->treeWidgetFavorites);
        QString name = QString::fromStdString(i->first);
        QString path = QString::fromStdString(i->second);

        favoriteItem->setText(0, name);
        favoriteItem->setText(1, path);

        favoriteItem->setToolTip(0, path);
        favoriteItem->setToolTip(1, path);
    }

    if(ui->treeWidgetFavorites->topLevelItemCount()>0)
        ui->treeWidgetFavorites->topLevelItem(0)->setSelected(true);
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
    ui->treeWidgetFavorites->clear();

    ui->spinBoxDataSelecType->setValue(0);
    ui->labelDataSelecTypeDesc->clear();
    ui->labelPropTypeVal->clear();

    ui->lineEditValList->clear();
    ui->lineEditStep->clear();

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

    CurrentProp.second.DataSelectionType = ui->spinBoxDataSelecType->value();
    ClassDescription->SetPropertyDescription(CurrentProp.first, CurrentProp.second);

    // Отключаем ввод в switch включаем необходимое
    ui->lineEditValList->setEnabled(false);
    ui->lineEditStep->setEnabled(false);

    switch (type)
    {
        case 0:
            ui->labelDataSelecTypeDesc->setText("Arbitrary data");
            ui->lineEditValList->setEnabled(true);
            ui->lineEditStep->clear();
            break;
        case 1:
            ui->labelDataSelecTypeDesc->setText("Checkbox");
            ui->lineEditStep->clear();
            ui->lineEditValList->clear();
            break;
        case 2:
            ui->labelDataSelecTypeDesc->setText("Range");
            ui->lineEditValList->setEnabled(true);
            ui->lineEditStep->clear();
            break;
        case 3:
            ui->labelDataSelecTypeDesc->setText("List of options");
            ui->lineEditValList->setEnabled(true);
            ui->lineEditStep->clear();
            break;
        case 4:
            ui->labelDataSelecTypeDesc->setText("Range with a given step");
            ui->lineEditValList->setEnabled(true);
            ui->lineEditStep->setEnabled(true);
            break;
        default:
            ui->labelDataSelecTypeDesc->setText("Unknown type");
            ui->lineEditStep->clear();
            ui->lineEditValList->clear();
            break;
    }
}

void UClassDescriptionDisplay::createNewFavoriteSlot()
{
    if(clFavEditor != NULL)
    {
        clFavEditor->ChangeClass(ClassName);
        clFavEditor->show();
    }
    else
    {
        clFavEditor = new UClassFavoritesEditor(ClassName);
        connect(clFavEditor, &UClassFavoritesEditor::CreateNewFavorite, this, &UClassDescriptionDisplay::createNewFavorite);
        clFavEditor->setWindowTitle("Classes Favorites");
        clFavEditor->show();
    }
}

void UClassDescriptionDisplay::createNewFavorite(QString name, QString path)
{
    ClassDescription->AddNewFavorite(name.toStdString(), path.toStdString());
    FillFavorites();
}

void UClassDescriptionDisplay::deleteFavoriteSlot()
{
    QTreeWidgetItem* item = ui->treeWidgetFavorites->currentItem();

    if(item)
    {
        ClassDescription->DeleteFavorite(item->text(0).toStdString());
        FillFavorites();
    }
}

void UClassDescriptionDisplay::on_spinBoxDataSelecType_valueChanged(int arg1)
{
    if(ClassDescription)
    {
        UpdateDataSelectionType(arg1);
    }
}

void UClassDescriptionDisplay::on_textEditHeaderProp_textChanged()
{
    if(ClassDescription)
    {
        if(ui->listWidgetProperties->currentItem())
        {
            CurrentProp.second.Header = ui->textEditHeaderProp->toPlainText().toStdString();
            ClassDescription->SetPropertyDescription(CurrentProp.first, CurrentProp.second);
        }
    }
}

void UClassDescriptionDisplay::on_textEditDescProp_textChanged()
{
    if(ClassDescription)
    {
        if(ui->listWidgetProperties->currentItem())
        {
            CurrentProp.second.Description = ui->textEditDescProp->toPlainText().toStdString();
            ClassDescription->SetPropertyDescription(CurrentProp.first, CurrentProp.second);
        }
    }
}

void UClassDescriptionDisplay::on_lineEditValList_textChanged(const QString &arg1)
{
    if(ClassDescription)
    {
        if(ui->listWidgetProperties->currentItem())
        {
            QStringList value_list = ui->lineEditValList->text().split(" ");
            std::vector<std::string> val_list;

            QString str;
            foreach (str, value_list)
                val_list.push_back(str.toStdString());

            CurrentProp.second.ValueList = val_list;
            ClassDescription->SetPropertyDescription(CurrentProp.first, CurrentProp.second);
        }
    }
}

void UClassDescriptionDisplay::on_lineEditStep_textChanged(const QString &arg1)
{
    if(ClassDescription)
    {
        if(ui->listWidgetProperties->currentItem())
        {
            CurrentProp.second.Step = ui->lineEditStep->text().toStdString();
            ClassDescription->SetPropertyDescription(CurrentProp.first, CurrentProp.second);
        }
    }
}

void UClassDescriptionDisplay::on_listWidgetProperties_currentTextChanged(const QString &currentText)
{
    if(ClassDescription)
    {
        if(currentText.isEmpty())
            return;

        CurrentProp.first = currentText.toStdString();
        CurrentProp.second = ClassDescription->GetPropertyDescription(currentText.toStdString());

        ui->textEditHeaderProp->setText(QString::fromStdString(CurrentProp.second.Header));
        ui->textEditDescProp->setText(QString::fromStdString(CurrentProp.second.Description));

        UpdateDataSelectionType(CurrentProp.second.DataSelectionType);

        QString val_list;
        for(auto i = CurrentProp.second.ValueList.begin(); i != CurrentProp.second.ValueList.end(); ++i)
        {
            if(!val_list.isEmpty())
                 val_list += " ";
            val_list += QString::fromStdString(*i);
        }
        ui->lineEditValList->setText(val_list);

        ui->lineEditStep->setText(QString::fromStdString(CurrentProp.second.Step));
        ui->labelPropTypeVal->setText(QString::fromStdString(CurrentProp.second.Type));
    }
}

void UClassDescriptionDisplay::hideEvent(QHideEvent *event)
{
    if(clFavEditor != NULL)
    {
        clFavEditor->hide();
    }
}

