#include "UClassesListWidget.h"
#include "ui_UClassesListWidget.h"
#include <QMimeData>
#include <QDragMoveEvent>
#include <QDrag>
#include <QDebug>
#include <QPushButton>

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
            item->setTextColor(Qt::darkCyan);
        item->setText(str);
        ui->listWidgetStorageByName->addItem(item);
    }
    ui->listWidgetStorageByName->sortItems(Qt::AscendingOrder);

    //инициализация древовидного списка по библиотекам
    stringBuff = Storage_GetClassLibrariesList();
    componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    bool isRTlib = false;
    foreach(str, componentNames)
    {
        if(str != "")
        {
            isRTlib = false;
            // Если это RT библиотека
            if(RTlibsNames.indexOf(str)!=-1)
                isRTlib = true;
            QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetStorageByLibs);
            item->setExpanded(true);
            item->setText(0, str);
            if(isRTlib)
                item->setTextColor(0,Qt::darkBlue);
            stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
            QStringList libClasses = QString(stringBuff).split(",");
            Engine_FreeBufString(stringBuff);
            QString className;
            foreach(className, libClasses)
            {
                if(className != "")
                {
                     QTreeWidgetItem* classItem = new QTreeWidgetItem(item);
                     classItem->setText(0, className);
                     if(isRTlib)
                         classItem->setTextColor(0,Qt::darkCyan);
                }
            }
        }
    }
    ui->treeWidgetStorageByLibs->sortItems(0, Qt::AscendingOrder);

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
      return ui->listWidgetStorageByName->currentItem()->text();
    break;
    case 1:
      if (ui->treeWidgetStorageByLibs->currentItem() && ui->treeWidgetStorageByLibs->currentItem()->childCount() == 0)
        return ui->treeWidgetStorageByLibs->currentItem()->text(0);
    break;
    case 2:
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
             ui->listWidgetRTlibClasses->addItem(className);
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
                    item->setTextColor(Qt::darkYellow);

                if(RTclassesNames.indexOf(str)!=-1)
                    item->setTextColor(Qt::darkCyan);
                item->setText(str);
                ui->listWidgetStorageByName->addItem(item);
            }
        }
    }
    ui->listWidgetStorageByName->sortItems(Qt::AscendingOrder);
}

void UClassesListWidget::tab1_textChanged(const QString &arg1)
{
    ui->treeWidgetStorageByLibs->clear();

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


    //инициализация древовидного списка по библиотекам
    const char * stringBuff = Storage_GetClassLibrariesList();
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);

    bool isRTlib = false;
    bool isMocklib = false;
    foreach(str, componentNames)
    {
        if(str != "")
        {
            isMocklib = false;
            isRTlib = false;

            // Если это Mock библиотека
            if(MockLibsNames.indexOf(str)!=-1)
                isMocklib = true;

            // Если это RT библиотека
            if(RTlibsNames.indexOf(str)!=-1)
                isRTlib = true;

            if (str.contains(arg1, Qt::CaseInsensitive))
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetStorageByLibs);
                item->setExpanded(true);
                item->setText(0, str);
                if(isMocklib)
                    item->setTextColor(0,Qt::darkMagenta);
                if(isRTlib)
                    item->setTextColor(0,Qt::darkBlue);
                stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
                QStringList libClasses = QString(stringBuff).split(",");
                Engine_FreeBufString(stringBuff);
                QString className;
                foreach(className, libClasses)
                {
                    if(className != "")
                    {
                         QTreeWidgetItem* classItem = new QTreeWidgetItem(item);
                         classItem->setText(0, className);
                         if(isMocklib)
                             classItem->setTextColor(0,Qt::darkYellow);
                         if(isRTlib)
                             classItem->setTextColor(0,Qt::darkCyan);
                    }
                }

            }
            else
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetStorageByLibs);
                bool isLibFind = false;
                item->setExpanded(true);
                item->setText(0, str);
                if(isMocklib)
                    item->setTextColor(0,Qt::darkMagenta);
                if(isRTlib)
                    item->setTextColor(0,Qt::darkBlue);
                stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
                QStringList libClasses = QString(stringBuff).split(",");
                Engine_FreeBufString(stringBuff);
                QString className;
                foreach(className, libClasses)
                {
                    if(className != "" && ((className.contains(arg1, Qt::CaseInsensitive))))
                    {
                         isLibFind = true;
                         QTreeWidgetItem* classItem = new QTreeWidgetItem(item);
                         classItem->setText(0, className);
                         if(isMocklib)
                             classItem->setTextColor(0,Qt::darkYellow);
                         if(isRTlib)
                             classItem->setTextColor(0,Qt::darkCyan);
                    }
                }
                if (!isLibFind)
                {
                    delete item;
                }

            }

        }
    }
    ui->treeWidgetStorageByLibs->sortItems(0, Qt::AscendingOrder);
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
