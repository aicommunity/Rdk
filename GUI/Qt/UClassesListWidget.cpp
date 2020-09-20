#include "UClassesListWidget.h"
#include "ui_UClassesListWidget.h"
#include "UDrawEngineWidget.h"
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

    UpdateInterval = 0; // ���������� �� ��������� ����� �� ����������
    setAccessibleName("UClassesListWidget"); // ��� ������ ��� ������������

    //������������� ����� �� ���� �������
    const char * stringBuff = Storage_GetClassesNameList();
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    ui->listWidgetStorageByName->addItems(componentNames);
    ui->listWidgetStorageByName->sortItems(Qt::AscendingOrder);

    //������������ ������������ ������ �� �����������
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

    //������������� runtime-���������
    auto storage = RDK::GetStorageLock();
    std::string buff;
    storage->GetRTlibsNameList(buff);
    componentNames = QString(buff.c_str()).split(",");

    ui->listWidgetRTlibs->addItems(componentNames);
    ui->listWidgetRTlibs->sortItems(Qt::AscendingOrder);

    //����� ������� �� ��������� ��� ������� ��������������
    connect(ui->treeWidgetStorageByLibs, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));
    connect(ui->listWidgetStorageByName, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));

    //����� �� ������� ������ ��������� ��������� ����������
    connect(ui->treeWidgetStorageByLibs, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SIGNAL(classSelectionChanged()));
    connect(ui->listWidgetStorageByName, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SIGNAL(classSelectionChanged()));

    // ���� ������� ������ ���������
    ui->listWidgetRTlibs->addAction(ui->actionCreateRuntimeLibrary);
    ui->listWidgetRTlibs->addAction(ui->actionDeleteRuntimeLibrary);
    ui->listWidgetRTlibs->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(ui->actionCreateRuntimeLibrary, SIGNAL(triggered()), this, SLOT(CreateRTlibrary()));
    connect(ui->actionDeleteRuntimeLibrary, SIGNAL(triggered()), this, SLOT(DeleteRTlibrary()));

    ui->listWidgetRTlibs->setContextMenuPolicy(Qt::ActionsContextMenu);

    // ���� ������� ������ �����������
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
  // ���������� �������� �� sigfault
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
    // ���������� runtime-���������
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

// ����� ����������� � ������ �����
void UClassesListWidget::on_listWidgetRTlibs_itemSelectionChanged()
{
    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    QString lib_name = item->text();

    // ���������� ������ ����������� ����������
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

    // ������ ���� �������
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

    //������������ ������������ ������ �� �����������
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

    // ����� ���� runtime-���������
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
    // ���� ������� ���������� �� �������
    if(ui->listWidgetRTlibs->selectedItems().size() == 0)
        return;

    // Storage
    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
        return;

    // ��� ���������
    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    QString lib_name = item->text().toUtf8().data();

    // ���������� ���� ��������
    QString message = "Deleting library \""+ lib_name + "\"";

    DeleteDialog* dialog = new DeleteDialog("Delete Library", message);

    if(dialog->exec() == QDialog::Accepted)
    {

        storage->DeleteRuntimeCollection(lib_name.toLocal8Bit().data());

        AUpdateInterface();

    }
    delete dialog;

}

void UClassesListWidget::AddNewClass()
{
    RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();

    // ����
    if(!engine || !engine->GetModel())
         return;

    // ��� ������� ��������� ���������� (���� �������)
    QString lib_name = "";
    if(ui->listWidgetRTlibs->currentItem())
        lib_name  = ui->listWidgetRTlibs->currentItem()->text();

    std::string buff;
    engine->GetModel()->GetStorage()->GetRTlibsNameList(buff);
    QStringList libs_names = QString(buff.c_str()).split(",");

    CrClassDialog* dialog = new CrClassDialog(libs_names,lib_name);
    if(dialog->exec() == QDialog::Accepted)
    {
        // ���������� ���������
        RDK::UEPtr<RDK::UContainer> container = engine->GetModel()
                                    ->GetComponent(static_cast<UDrawEngineWidget*>(this->parentWidget()->parentWidget()->parentWidget())->GetSelectedCmponent(), true);
        // ���� ������ �� ����������
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
    // ���� ������� ���������� �� ������� ��� ������� ����� �� ������
    if(ui->listWidgetRTlibs->selectedItems().empty() || ui->listWidgetRTlibClasses->selectedItems().empty())
        return;

    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
        return;

    // ��� ����������
    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    QString lib_name = item->text();

    // ��� ������
    item = ui->listWidgetRTlibClasses->currentItem();
    QString class_name = item->text();

    // ���������� ���� ��������
    QString message = "Deleting class \""+ class_name + "\" from library \"" + lib_name + "\"";

    DeleteDialog* dialog = new DeleteDialog("Delete Class", message);

    if(dialog->exec() == QDialog::Accepted)
    {
        storage->DelClassFromCollection(class_name.toUtf8().data(), lib_name.toUtf8().data());

        AUpdateInterface();
    }
    delete dialog;
}

// ���������� ���� ��� �������� ����������
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
    // ���� ���� ������
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

    // ���� ���������� � ����� ������ ���
    if(!storage->GetCollection(lib_name))
    {
        Message->setText("Enter library name");
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

// ���������� ���� ��� �������� ������
CrClassDialog::CrClassDialog(QStringList libs, QString cur_lib, QWidget* pwgt)
{
    setWindowTitle("Add New Class");

    MessageLib = new QLabel("Select the library where to add the component");

    // ������ ��������� � ��������� ������� ���, ������� ������� � ���� ��������� (���� �������)
    Libraries = new QComboBox;
    Libraries->addItems(libs);
    int index = Libraries->findText(cur_lib);
    if ( index != -1 ) { // -1 ���� �� �������
       Libraries->setCurrentIndex(index);
    }

    MessageClass = new QLabel("Enter Class Name");

    InputClassName = new QLineEdit;

    AddButton = new QPushButton("Add");
    ReplaceButton = new QPushButton("Replace");
    AddButton->setEnabled(false);
    ReplaceButton->setEnabled(false);
    QPushButton* cancel_button = new QPushButton("Cancel");

    connect(AddButton,SIGNAL(clicked()),this,SLOT(accept()));
    connect(ReplaceButton,SIGNAL(clicked()),this,SLOT(accept()));
    connect(cancel_button,SIGNAL(clicked()),this, SLOT(reject()));
    connect(InputClassName,SIGNAL(textChanged(const QString &)),this,SLOT(ProcessInput()));

    QHBoxLayout* hbox_layout = new QHBoxLayout;
    hbox_layout->addWidget(AddButton);
    hbox_layout->addWidget(ReplaceButton);
    hbox_layout->addWidget(cancel_button);

    QVBoxLayout* vbox_layout = new QVBoxLayout;
    vbox_layout->addWidget(MessageLib);
    vbox_layout->addWidget(Libraries);
    vbox_layout->addWidget(MessageClass);
    vbox_layout->addWidget(InputClassName);
    vbox_layout->addLayout(hbox_layout);
    setLayout(vbox_layout);
}

void CrClassDialog::ProcessInput()
{
    // ���� ���� ������
    if(InputClassName->text().isEmpty())
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

    // ���� ���� ������
    if(class_name.isEmpty())
        return;

    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
        return;

    // ���� ����� �� ���������� -> �������� ������
    if(!storage->CheckClass(class_name.toUtf8().data()))
    {
        MessageClass->setText("Enter Class Name");
        AddButton->setEnabled(true);
    }
    else // ���� ���������� - ��������������
    {
        std::string lib_name = storage->FindCollection(class_name.toUtf8().data())->GetName();
        MessageClass->setText("Class \"" + class_name +"\" already exists in \"" + QString::fromStdString(lib_name) + "\" library");
        ReplaceButton->setEnabled(true);
        Replace = true;
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

const std::string CrClassDialog::GetLibName() const
{
    return Libraries->currentText().toUtf8().data();
}

// ������ ��� �������� ����������/������
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
