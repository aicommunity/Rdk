#include "UClassesListWidget.h"
#include "ui_UClassesListWidget.h"
#include <QMimeData>
#include <QDragMoveEvent>
#include <QDrag>
#include <QDebug>
#include <QPushButton>
#include <QBrush>
#include <QHash>
#include "../../Core/Engine/UStorage.h"
#include "../../Core/Engine/UContainerDescription.h"
#include "../../Core/Engine/UComponentFactory.h"
#include "../../Deploy/Include/rdk_init.h"
#include "../../../Libraries/Nmsdk-PulseLib/Core/NPulseNeuron.h"
#include "../../../Libraries/Nmsdk-PulseLib/Core/NPulseSynapseCommon.h"
#include "../../../Libraries/Nmsdk-PulseLib/Core/NPulseMembraneCommon.h"
#include "../../../Libraries/Nmsdk-PulseLib/Core/NPulseChannelCommon.h"
#include "../../../Libraries/Nmsdk-PulseLib/Core/NConstGenerator.h"
#include "../../../Libraries/Nmsdk-PulseLib/Core/NPulseLTZoneCommon.h"

UClassesListWidget::UClassesListWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent, app), ModelScheme(nullptr),
    ui(new Ui::UClassesListWidget)
{
    ui->setupUi(this);

    UpdateInterval = 0; // обновление по системным тикам не происходит
    setAccessibleName("UClassesListWidget"); // имя класса для сериализации

    // Список RT библиотек
    auto storage = RDK::GetStorageLock();
    std::string buff;
    storage->GetLibsNameListByType(buff,2);
    QStringList RTlibsNames = QString(buff.c_str()).split(",");

    // Список всех компонентов из RT библиотек
    QStringList RTclassesNames;
    QString str;
    foreach(str, RTlibsNames)
    {
        const char * stringBuff;
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        // Если нет классов
        if((stringBuff[0] == '\0'))
        {
            Engine_FreeBufString(stringBuff);
            continue;
        }
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);
        RTclassesNames += libClasses;
    }

    //инициализация списка по всем классам
    const char * stringBuff = Storage_GetClassesNameList();
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    foreach(str, componentNames)
    {
        QListWidgetItem* item = new QListWidgetItem(ui->listWidgetStorageByName);
        if(RTclassesNames.indexOf(str)!=-1)
            item->setForeground(QBrush(Qt::darkCyan));
        item->setText(str);
        ui->listWidgetStorageByName->addItem(item);
        item->setToolTip(GetClassTooltip(str));
    }
    ui->listWidgetStorageByName->sortItems(Qt::AscendingOrder);

    //инициализация древовидного списка по библиотекам (с использованием BuildGroupedTree)
    BuildGroupedTree("");

    //инициализация runtime-библиотек
    ui->listWidgetRTlibs->addItems(RTlibsNames);
    ui->listWidgetRTlibs->sortItems(Qt::AscendingOrder);

    //связь нажатия на компонент для события перетаскивания
    connect(ui->treeWidgetStorageByLibs, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));
    connect(ui->listWidgetStorageByName, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));
    connect(ui->listWidgetRTlibClasses, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));


    //связи на внешний сигнал изменения выделения компонента
    connect(ui->treeWidgetStorageByLibs, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SIGNAL(classSelectionChanged()));
    connect(ui->listWidgetStorageByName, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SIGNAL(classSelectionChanged()));

    // Меню области списка библиотек
    ui->listWidgetRTlibs->addAction(ui->actionCreateRuntimeLibrary);
    ui->listWidgetRTlibs->addAction(ui->actionDeleteRuntimeLibrary);
    ui->listWidgetRTlibs->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(ui->actionCreateRuntimeLibrary, SIGNAL(triggered()), this, SLOT(CreateRTlibrary()));
    connect(ui->actionDeleteRuntimeLibrary, SIGNAL(triggered()), this, SLOT(DeleteRTlibrary()));

    ui->listWidgetRTlibs->setContextMenuPolicy(Qt::ActionsContextMenu);

    // Меню области списка компонентов
    ui->listWidgetRTlibClasses->addAction(ui->actionAddNewClass);
    ui->listWidgetRTlibClasses->addAction(ui->actionDeleteClass);

    connect(ui->actionAddNewClass, SIGNAL(triggered()), this, SLOT(AddNewClass()));
    connect(ui->actionDeleteClass, SIGNAL(triggered()), this, SLOT(DeleteClass()));

    ui->listWidgetRTlibClasses->setContextMenuPolicy(Qt::ActionsContextMenu);

    // Class description menu and action
    QAction *action_display_class_description =  new QAction("Class description", this);

    ui->listWidgetStorageByName->addAction(action_display_class_description);
    ui->treeWidgetStorageByLibs->addAction(action_display_class_description);

    ui->listWidgetStorageByName->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeWidgetStorageByLibs->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(action_display_class_description, SIGNAL(triggered()), this, SLOT(on_action_cl_desc_triggered()));

    // Инициализация combobox для выбора метода группировки
    ui->comboBoxGroupingMethod->addItem("No Grouping", static_cast<int>(GroupingMethod::None));
    ui->comboBoxGroupingMethod->addItem("By Description", static_cast<int>(GroupingMethod::ByDescription));
    ui->comboBoxGroupingMethod->addItem("By Inheritance", static_cast<int>(GroupingMethod::ByInheritance));
    ui->comboBoxGroupingMethod->addItem("By Base Component", static_cast<int>(GroupingMethod::ByBaseComponent));
    ui->comboBoxGroupingMethod->setCurrentIndex(0); // По умолчанию без группировки

    connect(ui->comboBoxGroupingMethod, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBoxGroupingMethod_currentIndexChanged(int)));

    setAcceptDrops(true);
}

UClassesListWidget::~UClassesListWidget()
{
  delete ui;

}

QString UClassesListWidget::selctedClass() const
{
//  if(ui->listWidgetStorageByName->selectedItems().size() ==0)
//      return QString();

  switch(ui->tabWidget->currentIndex())
  {
    case 0:
      if(ui->listWidgetStorageByName->currentItem())
        return ui->listWidgetStorageByName->currentItem()->text();
    break;
    case 1:
    {
      QTreeWidgetItem* currentItem = ui->treeWidgetStorageByLibs->currentItem();
      if (currentItem)
      {
        // Класс - это элемент без детей (лист дерева)
        // В трехуровневой структуре: Библиотека -> Группа -> Класс
        // В двухуровневой структуре: Библиотека -> Класс
        if (currentItem->childCount() == 0)
        {
          return currentItem->text(0);
        }
      }
    }
    break;
    case 2:
      if(ui->listWidgetRTlibClasses->currentItem())
        return ui->listWidgetRTlibClasses->currentItem()->text();
    break;
    default:
      return QString();
  }
  return QString();
}

void UClassesListWidget::AUpdateLibsView(QString lib_name)
{
    // обновление runtime-библиотек
    // Storage
    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();
    if(!storage)
        return;

    std::string buff;
    storage->GetLibsNameListByType(buff,2);
    QStringList RTlibsNames = QString(buff.c_str()).split(",");

    ui->listWidgetRTlibs->clear();
    ui->listWidgetRTlibs->addItems(RTlibsNames);
    ui->listWidgetRTlibs->sortItems(Qt::AscendingOrder);
    auto chosen = ui->listWidgetRTlibs->findItems(lib_name, Qt::MatchExactly);
    if(chosen.size()==1)
        ui->listWidgetRTlibs->setCurrentItem(chosen.at(0));
}

void UClassesListWidget::SetModelScheme(UDrawEngineImageWidget* model)
{
    ModelScheme = model;
}

void UClassesListWidget::dropEvent(QDropEvent *event)
{
    /*
    QByteArray itemData = event->mimeData()->data("ComponentName");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);
    QString compname;
    dataStream >> compname;
    */

    // Определение координат дропа в систему координат списка библиотек
    QString lib = "";
    QPoint globalPos = this->mapToGlobal(event->pos());
    QPoint RTlibsPos = ui->listWidgetRTlibs->mapFromGlobal(globalPos);

    QListWidgetItem* item = ui->listWidgetRTlibs->itemAt(RTlibsPos);
    if(item)
        lib = item->text();

    AddNewClass(lib);
}

void UClassesListWidget::dragEnterEvent(QDragEnterEvent *event)
{

//    QPoint globalPos = this->mapToGlobal(event->pos());
//    QPoint GBoxLibsPos = ui->groupBoxLibList->mapFromGlobal(globalPos);
//    QPoint GBoxCompsPos = ui->groupBoxLibComps->mapFromGlobal(globalPos);

    if(event->mimeData()->hasFormat("ComponentName") && (ui->tabWidget->currentIndex() == 2))
           // && (ui->groupBoxLibList->rect().contains(GBoxLibsPos) || ui->groupBoxLibComps->rect().contains(GBoxCompsPos)))
    {
      event->setDropAction(Qt::MoveAction);
      event->accept();
    }
    else
    {
      event->ignore();
    }
}

void UClassesListWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasFormat("ComponentName") && (ui->tabWidget->currentIndex() == 2))
    {
      event->setDropAction(Qt::MoveAction);
      event->accept();
    }
    else
    {
      event->ignore();
    }
}

void UClassesListWidget::dragEvent(QModelIndex index)
{
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << index.data().toString();

    QMimeData *mimeData = new QMimeData();
    mimeData->setData("Component", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    drag->exec(Qt::CopyAction, Qt::CopyAction);
}

void UClassesListWidget::on_listWidgetRTlibs_itemSelectionChanged()
{
    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    if(!item)
        return;
    QString lib_name = item->text();

    // Заполнение списка компонентов библиотеки
    const char* stringBuff = Storage_GetLibraryClassNames(lib_name.toLocal8Bit());
    QStringList libClasses = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);

    QString className;
    ui->listWidgetRTlibClasses->clear();
    foreach(className, libClasses)
    {
        if(className != "")
        {
             QListWidgetItem* newItem = new QListWidgetItem(className);
             ui->listWidgetRTlibClasses->addItem(newItem);
             newItem->setToolTip(GetClassTooltip(className));
        }
    }
}

// Поиск компонентов в разных полях
void UClassesListWidget::tab0_textChanged(const QString &arg1)
{
    ui->listWidgetStorageByName->clear();

    // Список RT библиотек
    auto storage = RDK::GetStorageLock();
    std::string buff;
    storage->GetLibsNameListByType(buff,2);
    QStringList RTlibsNames = QString(buff.c_str()).split(",");

    // Список всех компонентов из RT библиотек
    QStringList RTclassesNames;
    QString str;
    foreach(str, RTlibsNames)
    {
        const char * stringBuff;
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        // Если нет классов
        if((stringBuff[0] == '\0'))
        {
            Engine_FreeBufString(stringBuff);
            continue;
        }
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);
        RTclassesNames += libClasses;
    }

    // Список Mock библиотек
    storage->GetLibsNameListByType(buff,3);
    QStringList MockLibsNames = QString(buff.c_str()).split(",");

    // Список всех компонентов из Mock библиотек
    QStringList MockClassesNames;

    foreach(str, MockLibsNames)
    {
        const char * stringBuff;
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        // Если нет классов
        if((stringBuff[0] == '\0'))
        {
            Engine_FreeBufString(stringBuff);
            continue;
        }
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);
        MockClassesNames += libClasses;
    }


    // список всех классов
    const char * stringBuff = Storage_GetClassesNameList();
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);

    foreach(str, componentNames)
    {
        if(str != "")
        {
            if (str.contains(arg1, Qt::CaseInsensitive))
            {
                QListWidgetItem* item = new QListWidgetItem(ui->listWidgetStorageByName);

                if(MockClassesNames.indexOf(str)!=-1)
                    item->setForeground(QBrush(Qt::darkYellow));

                if(RTclassesNames.indexOf(str)!=-1)
                    item->setForeground(QBrush(Qt::darkCyan));
                item->setText(str);
                ui->listWidgetStorageByName->addItem(item);
                item->setToolTip(GetClassTooltip(str));
            }
        }
    }
    ui->listWidgetStorageByName->sortItems(Qt::AscendingOrder);
}

void UClassesListWidget::tab1_textChanged(const QString &arg1)
{
    // Используем BuildGroupedTree для построения дерева с учетом поиска
    BuildGroupedTree(arg1);
}

void UClassesListWidget::tab2_textChanged(const QString &arg1)
{
    ui->listWidgetRTlibs->clear();

    // поиск имен runtime-библиотек
    auto storage = RDK::GetStorageLock();
    std::string buff;
    storage->GetLibsNameListByType(buff,2);
    QStringList componentNames = QString(buff.c_str()).split(",");

    QString str;

    foreach(str, componentNames)
    {
        if(str != "")
        {
            if (str.contains(arg1, Qt::CaseInsensitive))
            {
                 ui->listWidgetRTlibs->addItem(str);
            }
        }
    }

    ui->listWidgetRTlibs->sortItems(Qt::AscendingOrder);
}

void UClassesListWidget::on_lineEditSearch_textChanged(const QString &arg1)
{
    switch(ui->tabWidget->currentIndex())
    {
    case 0:
        {
            tab0_textChanged(arg1);
            break;
        }
    case 1:
        {
            tab1_textChanged(arg1);
            break;
        }
    case 2:
        {
            tab2_textChanged(arg1);
            break;
        }
    }

}

// Actions
void UClassesListWidget::CreateRTlibrary()
{
    // Storage
    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();
    if(!storage)
        return;

    CrLibDialog* dialog = new CrLibDialog;
    if(dialog->exec() == QDialog::Accepted)
    {
        std::string lib_name = dialog->GetLibName();

        if(!storage->CreateRuntimeCollection(lib_name))
        {
            QMessageBox::warning(this, "Error",
                         "An error occurred while creating library \"" + QString::fromStdString(dialog->GetLibName())+"\"");
        }

        AUpdateLibsView(QString::fromStdString(lib_name));
    }
    delete dialog;
}

void UClassesListWidget::DeleteRTlibrary()
{
    // Если никакая библиотека не выбрана
    if(ui->listWidgetRTlibs->selectedItems().size() == 0)
        return;

    // Storage
    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();
    if(!storage)
        return;

    // Имя бибиотеки
    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    QString lib_name = item->text().toUtf8().data();

    // Диалоговое окно удаления
    QString message = "Deleting library \""+ lib_name + "\"";

    DeleteDialog* dialog = new DeleteDialog("Delete Library", message);

    if(dialog->exec() == QDialog::Accepted)
    {
        if(!storage->DeleteRuntimeCollection(lib_name.toLocal8Bit().data()))
        {
            QMessageBox::warning(this, "Error",
                         "An error occurred while deleting library \"" + lib_name+"\"");
        }

        AUpdateLibsView("");
    }
    delete dialog;

}

void UClassesListWidget::AddNewClass(QString cur_lib)
{
    RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();

    // Если нет модели
    if(!engine || !engine->GetModel() || !engine->GetModel()->GetStorage())
         return;

    // Выделенный компонент
    RDK::UEPtr<RDK::UContainer> container = engine->GetModel()
                                ->GetComponentL(ModelScheme->GetLongName(), true);
    // Если компонент не выделен
    if(!container)
        return;

    // Имя текущей выбранной библиотеки (если выбрана)
    QString lib_name = cur_lib;
    if(ui->listWidgetRTlibs->currentItem() && lib_name.isEmpty())
        lib_name  = ui->listWidgetRTlibs->currentItem()->text();

    std::string buff;
    engine->GetModel()->GetStorage()->GetLibsNameListByType(buff,2);
    QStringList libs_names = QString(buff.c_str()).split(",");

    CrClassDialog* dialog = new CrClassDialog(libs_names, lib_name, QString::fromStdString(container->GetName()));
    if(dialog->exec() == QDialog::Accepted)
    {
        if(!engine->GetEnvironment()->
                GetStorage()->AddClassToCollection(dialog->GetClassName(), dialog->GetCompName(),
                                                   dialog->GetReplace(), container, dialog->GetLibName()))
        {
            QMessageBox::warning(this, "Error",
                         "An error occurred while adding new class \"" + QString::fromStdString(dialog->GetClassName())
                         + "\" to library \"" +  QString::fromStdString(dialog->GetLibName()) +"\"");
        }
        AUpdateLibsView(QString::fromStdString(dialog->GetLibName()));
    }

    delete dialog;
}

void UClassesListWidget::DeleteClass()
{
    // Если никакая библиотека не выбрана или никакой класс не выбран
    if(ui->listWidgetRTlibs->selectedItems().empty() || ui->listWidgetRTlibClasses->selectedItems().empty())
        return;

    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
        return;

    // Имя библиотеки
    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    QString lib_name = item->text();

    // Имя класса
    item = ui->listWidgetRTlibClasses->currentItem();
    QString class_name = item->text();

    // Диалоговое окно удаления
    QString message = "Deleting class \""+ class_name + "\" from library \"" + lib_name + "\"";

    DeleteDialog* dialog = new DeleteDialog("Delete Class", message);

    if(dialog->exec() == QDialog::Accepted)
    {
        if(!storage->DelClassFromCollection(class_name.toUtf8().data(), lib_name.toUtf8().data()))
        {
            QMessageBox::warning(this, "Error",
                         "An error occurred while delecting class \"" + class_name +"\"");
        }

        AUpdateLibsView(lib_name);
    }
    delete dialog;
}

// Диалоговое окно для создания библиотеки
CrLibDialog::CrLibDialog(QWidget* pwgt)
{
    setWindowTitle("Create New Library");

    InputLibName = new QLineEdit;

    Message = new QLabel("Enter Library Name");
    AddButton = new QPushButton("Ok");
    AddButton->setEnabled(false);
    QPushButton* cancel_button = new QPushButton("Cancel");

    connect(AddButton,SIGNAL(clicked()),this,SLOT(accept()));
    connect(cancel_button,SIGNAL(clicked()),this, SLOT(reject()));
    connect(InputLibName,SIGNAL(textChanged(const QString &)),this,SLOT(ProcessInput()));

    QHBoxLayout* hbox_layout = new QHBoxLayout;
    hbox_layout->addWidget(AddButton);
    hbox_layout->addWidget(cancel_button);

    QVBoxLayout* vbox_layout = new QVBoxLayout;
    vbox_layout->addWidget(Message);
    vbox_layout->addWidget(InputLibName);
    vbox_layout->addLayout(hbox_layout);
    setLayout(vbox_layout);
}

void CrLibDialog::ProcessInput()
{
    // Если ввод пустой
    if(InputLibName->text().isEmpty())
    {
        Message->setText("Enter Library Name");
        AddButton->setEnabled(false);
        return;
    }

    std::string lib_name = InputLibName->text().toUtf8().data();

    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
        return;

    // Если библиотеки с таким именем нет
    if(!storage->GetCollection(lib_name))
    {
        Message->setText("Enter Library name");
        AddButton->setEnabled(true);
    }
    else
    {
        AddButton->setEnabled(false);
        Message->setText("Library with this name already exists");
    }


}

const std::string CrLibDialog::GetLibName() const
{
    return InputLibName->text().toUtf8().data();
}

// Диалоговое окно для создания класса
CrClassDialog::CrClassDialog(QStringList libs, QString cur_lib, QString cur_comp_name, QWidget* pwgt)
{
    setWindowTitle("Add New Class");

    MessageLib = new QLabel("Select the library where to add the component");

    // Список библиотек и установка текущей той, которая выбрана в окне библиотек (если выбрана)
    Libraries = new QComboBox;
    Libraries->addItems(libs);
    int index = Libraries->findText(cur_lib);
    if ( index != -1 ) { // -1 Если не найдено
       Libraries->setCurrentIndex(index);
    }

    MessageComp = new QLabel("Enter New Component Name");

    InputCompName = new QLineEdit(cur_comp_name);

    MessageClass = new QLabel("Enter Class Name");

    InputClassName = new QLineEdit;

    AddButton = new QPushButton("Add");
    ReplaceButton = new QPushButton("Replace");
    AddButton->setEnabled(false);
    ReplaceButton->setEnabled(false);
    QPushButton* cancel_button = new QPushButton("Cancel");

    connect(AddButton,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ReplaceButton,SIGNAL(clicked()),this,SLOT(ReplaceClicked()));
    connect(cancel_button,SIGNAL(clicked()),this, SLOT(reject()));
    connect(InputClassName,SIGNAL(textChanged(const QString &)),this,SLOT(ProcessInput()));

    QHBoxLayout* hbox_layout = new QHBoxLayout;
    hbox_layout->addWidget(AddButton);
    hbox_layout->addWidget(ReplaceButton);
    hbox_layout->addWidget(cancel_button);

    QVBoxLayout* vbox_layout = new QVBoxLayout;
    vbox_layout->addWidget(MessageLib);
    vbox_layout->addWidget(Libraries);
    vbox_layout->addWidget(MessageComp);
    vbox_layout->addWidget(InputCompName);
    vbox_layout->addWidget(MessageClass);
    vbox_layout->addWidget(InputClassName);
    vbox_layout->addLayout(hbox_layout);
    setLayout(vbox_layout);
}

void CrClassDialog::ProcessInput()
{
    // Если ввод пустой
    if(InputClassName->text().isEmpty() || InputCompName->text().isEmpty())
    {
        MessageClass->setText("Enter Class Name");
        AddButton->setEnabled(false);
        ReplaceButton->setEnabled(false);
        Replace = false;
        return;
    }

    AddButton->setEnabled(false);
    ReplaceButton->setEnabled(false);
    Replace = false;

    QString class_name = InputClassName->text();

    // Если ввод пустой
    if(class_name.isEmpty())
        return;

    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
        return;

    // Если класс не существует -> создание класса
    if(!storage->CheckClass(class_name.toUtf8().data()))
    {
        MessageClass->setText("Enter Class Name");
        AddButton->setEnabled(true);
    }
    else// Если существует - предупреждение
    {
        std::string lib_name = storage->FindCollection(class_name.toUtf8().data())->GetName();
        MessageClass->setText("Class \"" + class_name +"\" already exists in \"" + QString::fromStdString(lib_name) + "\" library");
        ReplaceButton->setEnabled(true);
        Replace = true;
    }
}

void CrClassDialog::ReplaceClicked()
{
    // Диалоговое окно подтверждения замены класса
    QString message =  MessageClass->text() +
                       "\n" + "You are going to replace class \"" + InputClassName->text() + "\"";

    DeleteDialog* dialog = new DeleteDialog("Replacing class", message);

    // При подтверджении замены класса
    if(dialog->exec() == QDialog::Accepted)
    {
        accept();
    }
    delete dialog;

}

bool CrClassDialog::GetReplace() const
{
    return Replace;
}

const std::string CrClassDialog::GetClassName() const
{
    return InputClassName->text().toUtf8().data();
}

const std::string CrClassDialog::GetCompName() const
{
    return InputCompName->text().toUtf8().data();
}

const std::string CrClassDialog::GetLibName() const
{
    return Libraries->currentText().toUtf8().data();
}

// Диалог для удаления библиотеки/класса
DeleteDialog::DeleteDialog(QString title, QString message, QWidget* pwgt)
{
    setWindowTitle(title);

    QLabel* info_message = new QLabel(message);

    QPushButton* ok_button = new QPushButton("Ok");
    QPushButton* cancel_button = new QPushButton("Cancel");

    connect(ok_button, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancel_button, SIGNAL(clicked()), this, SLOT(reject()));

    QHBoxLayout* hbox_layout = new QHBoxLayout;
    hbox_layout->addWidget(ok_button);
    hbox_layout->addWidget(cancel_button);

    QVBoxLayout* vbox_layout = new QVBoxLayout;
    vbox_layout->addWidget(info_message);
    vbox_layout->addLayout(hbox_layout);
    setLayout(vbox_layout);
}

void UClassesListWidget::on_tabWidget_currentChanged(int index)
{
    on_lineEditSearch_textChanged("");
}

void UClassesListWidget::on_action_cl_desc_triggered()
{
    switch(ui->tabWidget->currentIndex())
    {
    case 0:
        {
            QListWidgetItem* item = ui->listWidgetStorageByName->currentItem();
            if(!item)
                return;
            ModelScheme->classDescription(item->text().toStdString());
            break;
        }
    case 1:
        {
            QTreeWidgetItem* item = ui->treeWidgetStorageByLibs->currentItem();
            if(!item)
                return;
            // Класс - это элемент без детей (лист дерева)
            // В трехуровневой структуре: Библиотека -> Группа -> Класс
            // В двухуровневой структуре: Библиотека -> Класс
            if(item->childCount() == 0)
                ModelScheme->classDescription(item->text(0).toStdString());
            break;
        }
    case 2:
        {
            QListWidgetItem* item = ui->listWidgetRTlibClasses->currentItem();
            if(!item)
                return;
            ModelScheme->classDescription(item->text().toStdString());
            break;
        }
    }
}

void UClassesListWidget::disable_cl_desc_popup_menu()
{
    QList<QAction*> actions = ui->listWidgetStorageByName->actions();
    if(actions.size()>0 && actions.at(0)->text() == "Class description")
    {
        disconnect(actions.at(0), SIGNAL(triggered()), this, SLOT(on_action_cl_desc_triggered()));
        ui->listWidgetStorageByName->removeAction(actions.at(0));
        ui->treeWidgetStorageByLibs->removeAction(actions.at(0));
        delete actions.at(0);
    }
    ui->listWidgetStorageByName->setContextMenuPolicy(Qt::NoContextMenu);
    ui->treeWidgetStorageByLibs->setContextMenuPolicy(Qt::NoContextMenu);
}

void UClassesListWidget::removeTab(int index)
{
    ui->tabWidget->removeTab(index);
}


void UClassesListWidget::on_treeWidgetStorageByLibs_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    on_action_cl_desc_triggered();
}


void UClassesListWidget::on_listWidgetStorageByName_itemDoubleClicked(QListWidgetItem *item)
{
    on_action_cl_desc_triggered();
}


void UClassesListWidget::on_listWidgetRTlibClasses_itemDoubleClicked(QListWidgetItem *item)
{
    on_action_cl_desc_triggered();
}

// Реализация методов группировки

GroupingMethod UClassesListWidget::GetCurrentGroupingMethod() const
{
    int index = ui->comboBoxGroupingMethod->currentIndex();
    if (index >= 0 && index < ui->comboBoxGroupingMethod->count())
    {
        return static_cast<GroupingMethod>(ui->comboBoxGroupingMethod->itemData(index).toInt());
    }
    return GroupingMethod::None;
}

QString UClassesListWidget::GetClassGroup(const QString& className, GroupingMethod method) const
{
    // Проверка кэша
    QString cacheKey = QString("%1_%2").arg(className).arg(static_cast<int>(method));
    if (GroupingCache.contains(cacheKey))
    {
        return GroupingCache[cacheKey];
    }

    QString group;
    switch (method)
    {
        case GroupingMethod::ByDescription:
            group = GroupByDescription(className);
            break;
        case GroupingMethod::ByInheritance:
            group = GroupByInheritance(className);
            break;
        case GroupingMethod::ByBaseComponent:
            group = GroupByBaseComponent(className);
            break;
        case GroupingMethod::None:
        default:
            group = "";
            break;
    }

    // Сохранение в кэш
    GroupingCache[cacheKey] = group;
    return group;
}

QString UClassesListWidget::GroupByDescription(const QString& className) const
{
    auto storage = RDK::GetStorageLock();
    if (!storage)
        return "Other";

    RDK::UEPtr<RDK::UContainerDescription> desc = storage->GetClassDescription(className.toStdString(), true);
    if (!desc)
        return "Other";

    QString header = QString::fromStdString(desc->GetHeader()).toLower();
    QString description = QString::fromStdString(desc->GetDescription()).toLower();
    QString combined = header + " " + description;

    // Поиск ключевых слов
    if (combined.contains("нейрон", Qt::CaseInsensitive) || combined.contains("neuron", Qt::CaseInsensitive))
        return "Neurons";
    if (combined.contains("синапс", Qt::CaseInsensitive) || combined.contains("synapse", Qt::CaseInsensitive))
        return "Synapses";
    if (combined.contains("мембрана", Qt::CaseInsensitive) || combined.contains("membrane", Qt::CaseInsensitive))
        return "Membranes";
    if (combined.contains("канал", Qt::CaseInsensitive) || combined.contains("channel", Qt::CaseInsensitive))
        return "Channels";
    if (combined.contains("генератор", Qt::CaseInsensitive) || combined.contains("generator", Qt::CaseInsensitive))
        return "Generators";
    if (combined.contains("зона", Qt::CaseInsensitive) || combined.contains("zone", Qt::CaseInsensitive))
        return "Zones";
    if (combined.contains("слой", Qt::CaseInsensitive) || combined.contains("layer", Qt::CaseInsensitive))
        return "Layers";
    if (combined.contains("модель", Qt::CaseInsensitive) || combined.contains("model", Qt::CaseInsensitive))
        return "Models";
    if (combined.contains("сеть", Qt::CaseInsensitive) || combined.contains("net", Qt::CaseInsensitive))
        return "Networks";

    return "Other";
}

QString UClassesListWidget::GroupByInheritance(const QString& className) const
{
    // Используем анализ имени класса вместо создания объекта
    // Это безопаснее, так как не требует инициализации объекта и его связей
    QString classNameLower = className.toLower();

    // Анализ имен классов из NPulseLib и других библиотек
    // Нейроны
    if (classNameLower.contains("neuron", Qt::CaseInsensitive) ||
        classNameLower.contains("нейрон", Qt::CaseInsensitive))
    {
        // Исключаем мембраны нейронов и другие компоненты
        if (!classNameLower.contains("membrane", Qt::CaseInsensitive) &&
            !classNameLower.contains("мембрана", Qt::CaseInsensitive))
        {
            return "Neurons";
        }
    }

    // Синапсы
    if (classNameLower.contains("synapse", Qt::CaseInsensitive) ||
        classNameLower.contains("синапс", Qt::CaseInsensitive))
    {
        return "Synapses";
    }

    // Мембраны
    if (classNameLower.contains("membrane", Qt::CaseInsensitive) ||
        classNameLower.contains("мембрана", Qt::CaseInsensitive))
    {
        return "Membranes";
    }

    // Каналы
    if (classNameLower.contains("channel", Qt::CaseInsensitive) ||
        classNameLower.contains("канал", Qt::CaseInsensitive))
    {
        return "Channels";
    }

    // Генераторы
    if (classNameLower.contains("generator", Qt::CaseInsensitive) ||
        classNameLower.contains("генератор", Qt::CaseInsensitive))
    {
        return "Generators";
    }

    // Зоны
    if (classNameLower.contains("zone", Qt::CaseInsensitive) ||
        classNameLower.contains("зона", Qt::CaseInsensitive) ||
        classNameLower.contains("ltzone", Qt::CaseInsensitive))
    {
        return "Zones";
    }

    // Слои
    if (classNameLower.contains("layer", Qt::CaseInsensitive) ||
        classNameLower.contains("слой", Qt::CaseInsensitive))
    {
        return "Layers";
    }

    // Попытка определить через паттерны имен классов из известных библиотек
    // NPulseLib паттерны
    if (classNameLower.startsWith("np") || classNameLower.startsWith("nc"))
    {
        // NP - обычно нейроны в PulseLib
        if (classNameLower.startsWith("np") &&
            (classNameLower.contains("neuron") ||
             classNameLower.contains("hebb") ||
             classNameLower.contains("afferent")))
        {
            return "Neurons";
        }
        // NC - обычно компоненты кабелей
        if (classNameLower.startsWith("nc"))
        {
            if (classNameLower.contains("synapse") || classNameLower.contains("syn"))
                return "Synapses";
            if (classNameLower.contains("membrane") || classNameLower.contains("mem"))
                return "Membranes";
            if (classNameLower.contains("channel") || classNameLower.contains("chan"))
                return "Channels";
            if (classNameLower.contains("generator") || classNameLower.contains("gen"))
                return "Generators";
            if (classNameLower.contains("neuron"))
                return "Neurons";
        }
    }

    return "Other";
}

QString UClassesListWidget::GroupByBaseComponent(const QString& className) const
{
    // Анализ имени класса и библиотеки для определения базового компонента
    QString classNameLower = className.toLower();

    // Для NPulseLib: анализ паттернов имен
    if (classNameLower.contains("neuron", Qt::CaseInsensitive) ||
        classNameLower.contains("нейрон", Qt::CaseInsensitive))
    {
        // Проверяем, не является ли это конкретным типом нейрона
        if (classNameLower.contains("hebb", Qt::CaseInsensitive))
            return "Neurons";
        if (classNameLower.contains("afferent", Qt::CaseInsensitive))
            return "Neurons";
        if (classNameLower.startsWith("np") || classNameLower.startsWith("nc"))
            return "Neurons";
    }

    if (classNameLower.contains("synapse", Qt::CaseInsensitive) ||
        classNameLower.contains("синапс", Qt::CaseInsensitive))
    {
        return "Synapses";
    }

    if (classNameLower.contains("membrane", Qt::CaseInsensitive) ||
        classNameLower.contains("мембрана", Qt::CaseInsensitive))
    {
        return "Membranes";
    }

    if (classNameLower.contains("channel", Qt::CaseInsensitive) ||
        classNameLower.contains("канал", Qt::CaseInsensitive))
    {
        return "Channels";
    }

    if (classNameLower.contains("generator", Qt::CaseInsensitive) ||
        classNameLower.contains("генератор", Qt::CaseInsensitive))
    {
        return "Generators";
    }

    if (classNameLower.contains("zone", Qt::CaseInsensitive) ||
        classNameLower.contains("зона", Qt::CaseInsensitive))
    {
        return "Zones";
    }

    if (classNameLower.contains("layer", Qt::CaseInsensitive) ||
        classNameLower.contains("слой", Qt::CaseInsensitive))
    {
        return "Layers";
    }

    // Попытка определить через библиотеку
    auto storage = RDK::GetStorageLock();
    if (storage)
    {
        RDK::UEPtr<RDK::ULibrary> lib = storage->FindCollection(className.toStdString());
        if (lib)
        {
            QString libName = QString::fromStdString(lib->GetName()).toLower();
            // Для известных библиотек можно добавить специальную логику
            if (libName.contains("pulse", Qt::CaseInsensitive))
            {
                // Дополнительный анализ для PulseLib
                if (classNameLower.startsWith("np") &&
                    (classNameLower.contains("neuron") || classNameLower.contains("neuron")))
                {
                    return "Neurons";
                }
            }
        }
    }

    return "Other";
}

void UClassesListWidget::on_comboBoxGroupingMethod_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    // Очищаем кэш при изменении метода группировки
    GroupingCache.clear();
    // Перестраиваем дерево
    BuildGroupedTree(ui->lineEditSearch->text());
}

void UClassesListWidget::BuildGroupedTree(const QString& searchText)
{
    ui->treeWidgetStorageByLibs->clear();

    GroupingMethod method = GetCurrentGroupingMethod();

    // Список RT библиотек
    auto storage = RDK::GetStorageLock();
    std::string buff;
    storage->GetLibsNameListByType(buff, 2);
    QStringList RTlibsNames = QString(buff.c_str()).split(",");

    // Список всех компонентов из RT библиотек
    QStringList RTclassesNames;
    QString str;
    foreach(str, RTlibsNames)
    {
        const char * stringBuff;
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        if((stringBuff[0] == '\0'))
        {
            Engine_FreeBufString(stringBuff);
            continue;
        }
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);
        RTclassesNames += libClasses;
    }

    // Список Mock библиотек
    storage->GetLibsNameListByType(buff, 3);
    QStringList MockLibsNames = QString(buff.c_str()).split(",");

    // Список всех компонентов из Mock библиотек
    QStringList MockClassesNames;
    foreach(str, MockLibsNames)
    {
        const char * stringBuff;
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        if((stringBuff[0] == '\0'))
        {
            Engine_FreeBufString(stringBuff);
            continue;
        }
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);
        MockClassesNames += libClasses;
    }

    // Получение списка всех библиотек
    const char * stringBuff = Storage_GetClassLibrariesList();
    QStringList libraryNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);

    bool isRTlib = false;
    bool isMocklib = false;

    foreach(str, libraryNames)
    {
        if(str == "")
            continue;

        isMocklib = false;
        isRTlib = false;

        if(MockLibsNames.indexOf(str) != -1)
            isMocklib = true;
        if(RTlibsNames.indexOf(str) != -1)
            isRTlib = true;

        // Получение классов библиотеки
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);

        // Фильтрация по поисковому запросу
        QStringList filteredClasses;
        bool libMatchesSearch = str.contains(searchText, Qt::CaseInsensitive);

        foreach(QString className, libClasses)
        {
            if(className == "")
                continue;

            bool classMatchesSearch = className.contains(searchText, Qt::CaseInsensitive);

            if(searchText.isEmpty() || libMatchesSearch || classMatchesSearch)
            {
                filteredClasses.append(className);
            }
        }

        // Если нет классов после фильтрации, пропускаем библиотеку
        if(filteredClasses.isEmpty() && !libMatchesSearch)
            continue;

        // Создание элемента библиотеки
        QTreeWidgetItem* libItem = new QTreeWidgetItem(ui->treeWidgetStorageByLibs);
        libItem->setExpanded(true);
        libItem->setText(0, str);
        if(isMocklib)
            libItem->setForeground(0, QBrush(Qt::darkMagenta));
        if(isRTlib)
            libItem->setForeground(0, QBrush(Qt::darkBlue));

        if(method == GroupingMethod::None)
        {
            // Без группировки - двухуровневая структура
            foreach(QString className, filteredClasses)
            {
                QTreeWidgetItem* classItem = new QTreeWidgetItem(libItem);
                classItem->setText(0, className);
                if(isMocklib)
                    classItem->setForeground(0, QBrush(Qt::darkYellow));
                if(isRTlib)
                    classItem->setForeground(0, QBrush(Qt::darkCyan));
                classItem->setToolTip(0, GetClassTooltip(className));
            }
        }
        else
        {
            // С группировкой - трехуровневая структура
            QHash<QString, QStringList> groups;

            foreach(QString className, filteredClasses)
            {
                QString group = GetClassGroup(className, method);
                if(group.isEmpty())
                    group = "Other";

                if(!groups.contains(group))
                    groups[group] = QStringList();
                groups[group].append(className);
            }

            // Создание групп и классов
            QStringList groupNames = groups.keys();
            groupNames.sort();

            foreach(QString groupName, groupNames)
            {
                QTreeWidgetItem* groupItem = new QTreeWidgetItem(libItem);
                groupItem->setExpanded(true);
                groupItem->setText(0, groupName);

                QStringList classesInGroup = groups[groupName];
                classesInGroup.sort();

                foreach(QString className, classesInGroup)
                {
                    QTreeWidgetItem* classItem = new QTreeWidgetItem(groupItem);
                    classItem->setText(0, className);
                    if(isMocklib)
                        classItem->setForeground(0, QBrush(Qt::darkYellow));
                    if(isRTlib)
                        classItem->setForeground(0, QBrush(Qt::darkCyan));
                    classItem->setToolTip(0, GetClassTooltip(className));
                }
            }
        }
    }

    ui->treeWidgetStorageByLibs->sortItems(0, Qt::AscendingOrder);
}

QString UClassesListWidget::GetClassTooltip(const QString& className) const
{
    if (className.isEmpty())
        return QString();

    auto storage = RDK::GetStorageLock();
    if (!storage)
        return QString();

    QString tooltip;
    QString header;
    QString description;
    QString defaultComponentName;

    // Пытаемся получить описание класса
    RDK::UEPtr<RDK::UContainerDescription> desc = storage->GetClassDescription(className.toStdString(), true);
    if (desc)
    {
        header = QString::fromStdString(desc->GetHeader()).trimmed();
        description = QString::fromStdString(desc->GetDescription()).trimmed();
    }

    // Получаем имя компонента по умолчанию через factory
    try
    {
        RDK::UEPtr<RDK::UComponentAbstractFactory> factory = storage->GetComponentFactory(className.toStdString());
        if (factory)
        {
            // Проверяем тип factory
            RDK::UEPtr<RDK::UVirtualMethodFactory> virtualFactory =
                RDK::dynamic_pointer_cast<RDK::UVirtualMethodFactory>(factory);
            if (virtualFactory)
            {
                RDK::UEPtr<RDK::UContainer> component = virtualFactory->GetComponent();
                if (component)
                {
                    defaultComponentName = QString::fromStdString(component->GetName());
                }
            }
            else
            {
                // Для UComponentFactoryMethod создаем временный компонент для получения имени
                RDK::UEPtr<RDK::UComponent> tempComponent = factory->New();
                if (tempComponent)
                {
                    RDK::UEPtr<RDK::UContainer> container =
                        RDK::dynamic_pointer_cast<RDK::UContainer>(tempComponent);
                    if (container)
                    {
                        defaultComponentName = QString::fromStdString(container->GetName());
                    }
                }
            }
        }
    }
    catch(...)
    {
        // Игнорируем ошибки при получении имени компонента
    }

    // Формируем tooltip с именем класса и именем компонента по умолчанию
    QStringList tooltipParts;

    // Добавляем имя класса
    tooltipParts << QString("<b>Class:</b> %1").arg(className);

    // Добавляем имя компонента по умолчанию, если оно есть
    if (!defaultComponentName.isEmpty())
    {
        tooltipParts << QString("<b>Default component name:</b> %1").arg(defaultComponentName);
    }

    // Добавляем Header и Description, если они есть
    if (!header.isEmpty() || !description.isEmpty())
    {
        if (!header.isEmpty() && !description.isEmpty())
        {
            tooltipParts << QString("<b>%1</b><br/>%2").arg(header.toHtmlEscaped(), description.toHtmlEscaped());
        }
        else if (!header.isEmpty())
        {
            tooltipParts << QString("<b>%1</b>").arg(header.toHtmlEscaped());
        }
        else
        {
            tooltipParts << description.toHtmlEscaped();
        }
    }
    else
    {
        // Если описание пустое, показываем библиотеку
        RDK::UEPtr<RDK::ULibrary> lib = storage->FindCollection(className.toStdString());
        if (lib)
        {
            QString libName = QString::fromStdString(lib->GetName());
            tooltipParts << QString("<b>Library:</b> %1").arg(libName);
        }
    }

    tooltip = tooltipParts.join("<br/>");
    return tooltip;
}

