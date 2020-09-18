#include "UClassesListWidget.h"
#include "ui_UClassesListWidget.h"

#include <QMimeData>
#include <QDragMoveEvent>
#include <QDrag>
#include <QDebug>
#include <QPushButton>

UClassesListWidget::UClassesListWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UClassesListWidget)
{
    ui->setupUi(this);

    UpdateInterval = 0; // обновление по системным тикам не происходит
    setAccessibleName("UClassesListWidget"); // им€ класса дл€ сериализации

    //инициализаци€ спика по всем классам
    const char * stringBuff = Storage_GetClassesNameList();
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    ui->listWidgetStorageByName->addItems(componentNames);
    ui->listWidgetStorageByName->sortItems(Qt::AscendingOrder);

    //нициализаци€ древовидного списка по библиотекам
    stringBuff = Storage_GetClassLibrariesList();
    componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    QString str;
    foreach(str, componentNames)
    {
        if(str != "")
        {
            QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetStorageByLibs);
            item->setExpanded(true);
            item->setText(0, str);
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
                }
            }
        }
    }
    ui->treeWidgetStorageByLibs->sortItems(0, Qt::AscendingOrder);

    //инициализаци€ runtime-библиотек
    auto storage = RDK::GetStorageLock();
    std::string buff;
    storage->GetRTlibsNameList(buff);
    componentNames = QString(buff.c_str()).split(",");

    ui->listWidgetRTlibs->addItems(componentNames);
    ui->listWidgetRTlibs->sortItems(Qt::AscendingOrder);

    //св€зь нажати€ на компонент дл€ событи€ перетаскивани€
    connect(ui->treeWidgetStorageByLibs, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));
    connect(ui->listWidgetStorageByName, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));

    //св€зи на внешний сигнал изменени€ выделени€ компонента
    connect(ui->treeWidgetStorageByLibs, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SIGNAL(classSelectionChanged()));
    connect(ui->listWidgetStorageByName, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SIGNAL(classSelectionChanged()));

    // ћеню области списка библиотек
    ui->listWidgetRTlibs->addAction(ui->actionCreateRuntimeLibrary);
    ui->listWidgetRTlibs->addAction(ui->actionDeleteRuntimeLibrary);
    ui->listWidgetRTlibs->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(ui->actionCreateRuntimeLibrary, SIGNAL(triggered()), this, SLOT(CreateRTlibrary()));
    connect(ui->actionDeleteRuntimeLibrary, SIGNAL(triggered()), this, SLOT(DeleteRTlibrary()));

    ui->listWidgetRTlibs->setContextMenuPolicy(Qt::ActionsContextMenu);

    // ћеню области списка компонентов
    ui->listWidgetRTlibClasses->addAction(ui->actionAddNewClass);
    ui->listWidgetRTlibClasses->addAction(ui->actionDeleteClass);

    connect(ui->actionAddNewClass, SIGNAL(triggered()), this, SLOT(AddNewClass()));
    connect(ui->actionDeleteClass, SIGNAL(triggered()), this, SLOT(DeleteClass()));

    ui->listWidgetRTlibClasses->setContextMenuPolicy(Qt::ActionsContextMenu);
}

UClassesListWidget::~UClassesListWidget()
{
  delete ui;
}

QString UClassesListWidget::selctedClass() const
{
  // непон€тна€ заглушка от sigfault
  if(ui->listWidgetStorageByName->selectedItems().size() ==0)
      return QString();

  switch(ui->tabWidget->currentIndex())
  {
    case 0:
      return ui->listWidgetStorageByName->currentItem()->text();
    case 1:
      if (ui->treeWidgetStorageByLibs->currentItem() && ui->treeWidgetStorageByLibs->currentItem()->childCount() == 0)
        return ui->treeWidgetStorageByLibs->currentItem()->text(0);
    default:
      return QString();
  }
}

void UClassesListWidget::AUpdateInterface(void)
{
    // обновление runtime-библиотек
    auto storage = RDK::GetStorageLock();
    int lib_num = storage->GetNumCollections();
    QStringList componentNames;
    for (int i = 0; i <lib_num;i++)
    {
        storage->GetCollection(i);
        RDK::UEPtr<RDK::ULibrary> lib = storage->GetCollection(i);
        if(lib && lib->GetType() == 2)
        {
            componentNames.append(QString::fromStdString(lib->GetName()));
        }
    }
    ui->listWidgetRTlibs->clear();
    ui->listWidgetRTlibs->addItems(componentNames);
    ui->listWidgetRTlibs->sortItems(Qt::AscendingOrder);
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

// ѕоиск компонентов в разных пол€х
void UClassesListWidget::on_listWidgetRTlibs_itemSelectionChanged()
{
    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    QString lib_name = item->text();

    // «аполнение списка компонентов библиотеки
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

void UClassesListWidget::tab0_textChanged(const QString &arg1)
{
    ui->listWidgetStorageByName->clear();

    // список всех классов
    const char * stringBuff = Storage_GetClassesNameList();
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    QString str;

    foreach(str, componentNames)
    {
        if(str != "")
        {
            if (str.contains(arg1, Qt::CaseInsensitive))
            {
                 ui->listWidgetStorageByName->addItem(str);
            }
        }
    }
    ui->listWidgetStorageByName->sortItems(Qt::AscendingOrder);
}

void UClassesListWidget::tab1_textChanged(const QString &arg1)
{
    ui->treeWidgetStorageByLibs->clear();

    //нициализаци€ древовидного списка по библиотекам
    const char * stringBuff = Storage_GetClassLibrariesList();
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    QString str;
    foreach(str, componentNames)
    {
        if(str != "")
        {
            if (str.contains(arg1, Qt::CaseInsensitive))
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetStorageByLibs);
                item->setExpanded(true);
                item->setText(0, str);
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
                    }
                }

            }
            else
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetStorageByLibs);
                bool isLibFind = false;
                item->setExpanded(true);
                item->setText(0, str);
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
    // TODO сделать метод в Storage возвращающий список
    auto storage = RDK::GetStorageLock();
    std::string buff;
    storage->GetRTlibsNameList(buff);
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
    CrLibDialog* dialog = new CrLibDialog;
    if(dialog->exec() == QDialog::Accepted)
    {
        RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

        if(!storage)
         return;

        std::string lib_name = dialog->GetLibName();

        storage->CreateRuntimeCollection(lib_name);

        AUpdateInterface();
    }
    delete dialog;
}

void UClassesListWidget::DeleteRTlibrary()
{
    // ≈сли никака€ библиотека не выбрана
    if(ui->listWidgetRTlibs->selectedItems().size() == 0)
    {
        return;
    }

    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();

    QString lib_name = item->text().toUtf8().data();

    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
     return;

    storage->DeleteRuntimeCollection(lib_name.toLocal8Bit().data());

    AUpdateInterface();
}

void UClassesListWidget::AddNewClass()
{
    // ≈сли никака€ библиотека не выбрана
    if(ui->listWidgetRTlibs->selectedItems().size() == 0)
    {
        return;
    }

    RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();

    if(!engine)
         return;

    // ≈сли не существует модели -> предупреждение
    if(!engine->GetModel())
    {
        return;
    }

    QString lib_name  = ui->listWidgetRTlibs->currentItem()->text();

    CrClassDialog* dialog = new CrClassDialog(lib_name);
    if(dialog->exec() == QDialog::Accepted)
    {
        RDK::UEPtr<RDK::UContainer> container = engine->GetModel()->GetComponent(dialog->GetCompName(), true);
        // если объект не существует
        if(!container)
        {
            delete dialog;
            return;
        }
        engine->GetEnvironment()->
                GetStorage()->AddClassToCollection(dialog->GetClassName(),dialog->GetReplace(),
                                                   container,lib_name.toUtf8().data());

        AUpdateInterface();
    }

    delete dialog;
}

void UClassesListWidget::DeleteClass()
{
    // ≈сли никака€ библиотека не выбрана или никакой класс не выбран
    if(ui->listWidgetRTlibs->selectedItems().empty() || ui->listWidgetRTlibClasses->selectedItems().empty())
    {
        return;
    }

    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
     return;

    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    QString lib_name = item->text();

    item = ui->listWidgetRTlibClasses->currentItem();
    QString class_name = item->text();

    // ”даление
    storage->DelClassFromCollection(class_name.toUtf8().data(), lib_name.toUtf8().data());

    AUpdateInterface();
}

// ƒиалоговое окно дл€ создани€ библиотеки
CrLibDialog::CrLibDialog(QWidget* pwgt)
{
    setWindowTitle("Create New Library");

    InputLibName = new QLineEdit;

    Message = new QLabel("Enter library name");
    QPushButton* ok_button = new QPushButton("Ok");
    QPushButton* cancel_button = new QPushButton("Cancel");

    connect(ok_button,SIGNAL(clicked()),this,SLOT(ProcessInput()));
    connect(cancel_button,SIGNAL(clicked()),SLOT(reject()));

    QHBoxLayout* hbox_layout = new QHBoxLayout;
    hbox_layout->addWidget(ok_button);
    hbox_layout->addWidget(cancel_button);

    QVBoxLayout* vbox_layout = new QVBoxLayout;
    vbox_layout->addWidget(Message);
    vbox_layout->addWidget(InputLibName);
    vbox_layout->addLayout(hbox_layout);
    setLayout(vbox_layout);
}

void CrLibDialog::ProcessInput()
{
    // ≈сли ввод пустой
    if(InputLibName->text().isEmpty())
    {
        return;
    }

    std::string lib_name = InputLibName->text().toUtf8().data();

    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
     return;

    // ≈сли библиотеки с таким именем нет
    if(!storage->GetCollection(lib_name))
        accept(); // метод-слот успешного завершени€ диалогового окна
    else
        Message->setText("Library with this name already exists");


}

const std::string CrLibDialog::GetLibName() const
{
    return InputLibName->text().toUtf8().data();
}

// ƒиалоговое окно дл€ создани€ класса
CrClassDialog::CrClassDialog(QString lib_name, QWidget* pwgt) :Replace(false)
{
    setWindowTitle("Add New Class");

    Info = new QLabel("Adding Class to \"" + lib_name + "\" library");
    MessageClass = new QLabel("Enter Class name");
    InputClassName = new QLineEdit;
    MessageComp = new QLabel("Enter Comp name");
    ComponentName = new QLineEdit;
    QPushButton* ok_button = new QPushButton("Ok");
    QPushButton* cancel_button = new QPushButton("Cancel");

    connect(ok_button,SIGNAL(clicked()),this,SLOT(ProcessInput()));
    connect(cancel_button,SIGNAL(clicked()),SLOT(reject()));

    QHBoxLayout* hbox_layout = new QHBoxLayout;
    hbox_layout->addWidget(ok_button);
    hbox_layout->addWidget(cancel_button);

    QVBoxLayout* vbox_layout = new QVBoxLayout;
    vbox_layout->addWidget(Info);
    vbox_layout->addWidget(MessageClass);
    vbox_layout->addWidget(InputClassName);
    vbox_layout->addWidget(MessageComp);
    vbox_layout->addWidget(ComponentName);
    vbox_layout->addLayout(hbox_layout);
    setLayout(vbox_layout);
}

void CrClassDialog::ProcessInput()
{
    QString class_name = InputClassName->text();

    // ≈сли ввод пустой
    if(class_name.isEmpty())
    {
        return;
    }

    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
        return;

    // ѕроверка на существование компонента
    RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();

    if(!engine)
         return;

    RDK::UEPtr<RDK::UContainer> container = engine->GetModel()->GetComponent(ComponentName->text().toUtf8().data(), true);

    if(!container)
    {
        MessageComp->setText("Component with this name doesn't exist in the model");
        return;
    }

    // ≈сли класс не существует - завершаем диалог -> создание класса
    if(!storage->CheckClass(class_name.toUtf8().data()))
    {
        accept();
    }
    else
    {
        QString oldMessage = MessageClass->text();
        std::string lib_name = storage->FindCollection(class_name.toUtf8().data())->GetName();
        MessageClass->setText("Class \"" + class_name +"\" already exists in \"" + QString::fromStdString(lib_name) + "\" library"
                         +"\n" + "If you want to replace it, press \"Ok\" again");
        //≈сли Message идентичный -> значит это второе нажатие "ќк" -> замена класса
        if(oldMessage == MessageClass->text())
        {
            Replace = true;
            accept();
        }
    }

}

const bool CrClassDialog::GetReplace() const
{
    return Replace;
}

const std::string CrClassDialog::GetClassName() const
{
    return InputClassName->text().toUtf8().data();
}

const std::string CrClassDialog::GetCompName() const
{
    return ComponentName->text().toUtf8().data();
}

