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

    UpdateInterval = 0; // ���������� �� ��������� ����� �� ����������
    setAccessibleName("UClassesListWidget"); // ��� ������ ��� ������������

    // ������ RT ���������
    auto storage = RDK::GetStorageLock();
    std::string buff;
    storage->GetLibsNameListByType(buff,2);
    QStringList RTlibsNames = QString(buff.c_str()).split(",");

    // ������ ���� ����������� �� RT ���������
    QStringList RTclassesNames;
    QString str;
    foreach(str, RTlibsNames)
    {
        const char * stringBuff;
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        // ���� ��� �������
        if((stringBuff[0] == '\0'))
        {
            Engine_FreeBufString(stringBuff);
            continue;
        }
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);
        RTclassesNames += libClasses;
    }

    //������������� ������ �� ���� �������
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

    //������������� ������������ ������ �� �����������
    stringBuff = Storage_GetClassLibrariesList();
    componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    bool isRTlib = false;
    foreach(str, componentNames)
    {
        if(str != "")
        {
            isRTlib = false;
            // ���� ��� RT ����������
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

    //������������� runtime-���������
    ui->listWidgetRTlibs->addItems(RTlibsNames);
    ui->listWidgetRTlibs->sortItems(Qt::AscendingOrder);

    //����� ������� �� ��������� ��� ������� ��������������
    connect(ui->treeWidgetStorageByLibs, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));
    connect(ui->listWidgetStorageByName, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));
    connect(ui->listWidgetRTlibClasses, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));


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

    // Class description menu and action
    QAction *action_display_class_description =  new QAction("Class description", this);

    ui->listWidgetStorageByName->addAction(action_display_class_description);
    ui->treeWidgetStorageByLibs->addAction(action_display_class_description);

    ui->listWidgetStorageByName->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeWidgetStorageByLibs->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(action_display_class_description, SIGNAL(triggered()), this, SLOT(on_action_cl_desc_triggered()));

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
    // ���������� runtime-���������
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

    // ����������� ��������� ����� � ������� ��������� ������ ���������
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

// ����� ����������� � ������ �����
void UClassesListWidget::tab0_textChanged(const QString &arg1)
{
    ui->listWidgetStorageByName->clear();

    // ������ RT ���������
    auto storage = RDK::GetStorageLock();
    std::string buff;
    storage->GetLibsNameListByType(buff,2);
    QStringList RTlibsNames = QString(buff.c_str()).split(",");

    // ������ ���� ����������� �� RT ���������
    QStringList RTclassesNames;
    QString str;
    foreach(str, RTlibsNames)
    {
        const char * stringBuff;
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        // ���� ��� �������
        if((stringBuff[0] == '\0'))
        {
            Engine_FreeBufString(stringBuff);
            continue;
        }
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);
        RTclassesNames += libClasses;
    }

    // ������ Mock ���������
    storage->GetLibsNameListByType(buff,3);
    QStringList MockLibsNames = QString(buff.c_str()).split(",");

    // ������ ���� ����������� �� Mock ���������
    QStringList MockClassesNames;

    foreach(str, MockLibsNames)
    {
        const char * stringBuff;
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        // ���� ��� �������
        if((stringBuff[0] == '\0'))
        {
            Engine_FreeBufString(stringBuff);
            continue;
        }
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);
        MockClassesNames += libClasses;
    }


    // ������ ���� �������
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

    // ������ RT ���������
    auto storage = RDK::GetStorageLock();
    std::string buff;
    storage->GetLibsNameListByType(buff,2);
    QStringList RTlibsNames = QString(buff.c_str()).split(",");

    // ������ ���� ����������� �� RT ���������
    QStringList RTclassesNames;
    QString str;
    foreach(str, RTlibsNames)
    {
        const char * stringBuff;
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        // ���� ��� �������
        if((stringBuff[0] == '\0'))
        {
            Engine_FreeBufString(stringBuff);
            continue;
        }
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);
        RTclassesNames += libClasses;
    }


    // ������ Mock ���������
    storage->GetLibsNameListByType(buff,3);
    QStringList MockLibsNames = QString(buff.c_str()).split(",");

    // ������ ���� ����������� �� Mock ���������
    QStringList MockClassesNames;

    foreach(str, MockLibsNames)
    {
        const char * stringBuff;
        stringBuff = Storage_GetLibraryClassNames(str.toLocal8Bit());
        // ���� ��� �������
        if((stringBuff[0] == '\0'))
        {
            Engine_FreeBufString(stringBuff);
            continue;
        }
        QStringList libClasses = QString(stringBuff).split(",");
        Engine_FreeBufString(stringBuff);
        MockClassesNames += libClasses;
    }


    //������������� ������������ ������ �� �����������
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

            // ���� ��� Mock ����������
            if(MockLibsNames.indexOf(str)!=-1)
                isMocklib = true;

            // ���� ��� RT ����������
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

    // ����� ���� runtime-���������
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

    // ���� ��� ������
    if(!engine || !engine->GetModel() || !engine->GetModel()->GetStorage())
         return;

    // ���������� ���������
    std::weak_ptr<RDK::UContainer> container_weak = engine->GetModel()
                                ->GetComponentL(ModelScheme->GetLongName(), true);
    // ���� ��������� �� �������
    if(container_weak.expired())
        return;
    std::shared_ptr<RDK::UContainer> container = container_weak.lock();
    if(!container)
        return;

    // ��� ������� ��������� ���������� (���� �������)
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
                                                   dialog->GetReplace(), container.get(), dialog->GetLibName()))
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
        if(!storage->DelClassFromCollection(class_name.toUtf8().data(), lib_name.toUtf8().data()))
        {
            QMessageBox::warning(this, "Error",
                         "An error occurred while delecting class \"" + class_name +"\"");
        }

        AUpdateLibsView(lib_name);
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

// ���������� ���� ��� �������� ������
CrClassDialog::CrClassDialog(QStringList libs, QString cur_lib, QString cur_comp_name, QWidget* pwgt)
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
    // ���� ���� ������
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
    else// ���� ���������� - ��������������
    {
        std::string lib_name = storage->FindCollection(class_name.toUtf8().data())->GetName();
        MessageClass->setText("Class \"" + class_name +"\" already exists in \"" + QString::fromStdString(lib_name) + "\" library");
        ReplaceButton->setEnabled(true);
        Replace = true;
    }
}

void CrClassDialog::ReplaceClicked()
{
    // ���������� ���� ������������� ������ ������
    QString message =  MessageClass->text() +
                       "\n" + "You are going to replace class \"" + InputClassName->text() + "\"";

    DeleteDialog* dialog = new DeleteDialog("Replacing class", message);

    // ��� ������������� ������ ������
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
            // ���� ���� �������� - ������ ������� ������ (�.�. � �������� ������ - �������� ������� ����������)
            // � �������� ���������� ����������� ������������ �������
            if(item->parent())
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

