#include "UClassesListWidget.h"
#include "ui_UClassesListWidget.h"

#include <QMimeData>
#include <QDragMoveEvent>
#include <QDrag>
#include <QDebug>

UClassesListWidget::UClassesListWidget(QWidget *parent, RDK::UApplication *app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UClassesListWidget)
{
    ui->setupUi(this);
    UpdateInterval = 0; // обновление по системным тикам не происходит
    setAccessibleName("UClassesListWidget"); // имя класса для сериализации

    //инициализация спика по всем классам
    const char * stringBuff = Storage_GetClassesNameList();
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    ui->listWidgetStorageByName->addItems(componentNames);
    ui->listWidgetStorageByName->sortItems(Qt::AscendingOrder);

    //нициализация древовидного списка по библиотекам
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

    //инициализация runtime-библиотек
    auto storage = RDK::GetStorageLock();
    int lib_num = storage->GetNumCollections();
    componentNames.clear();
    for (int i = 0; i <lib_num;i++)
    {
        storage->GetCollection(i);
        RDK::UEPtr<RDK::ULibrary> lib = storage->GetCollection(i);
        if(lib && lib->GetType() == 2)
        {
            componentNames.append(QString::fromStdString(lib->GetName()));
        }
    }

    ui->listWidgetRTlibs->addItems(componentNames);
    ui->listWidgetRTlibs->sortItems(Qt::AscendingOrder);


    //связь нажатия на компонент для события перетаскивания
    connect(ui->treeWidgetStorageByLibs, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));
    connect(ui->listWidgetStorageByName, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));

    //связи на внешний сигнал изменения выделения компонента
    connect(ui->treeWidgetStorageByLibs, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), this, SIGNAL(classSelectionChanged()));
    connect(ui->listWidgetStorageByName, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SIGNAL(classSelectionChanged()));

}

UClassesListWidget::~UClassesListWidget()
{
  delete ui;
}

QString UClassesListWidget::selctedClass() const
{
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

void UClassesListWidget::on_lineEdit_textChanged(const QString &arg1)
{
    ui->treeWidgetStorageByLibs->clear();

    //нициализация древовидного списка по библиотекам
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

    //вот тут менять
    /*QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Component with this name is exists. Continue by adding the next number?", QMessageBox::Yes|QMessageBox::Cancel);
    if (reply == QMessageBox::Cancel) return;*/
}

void UClassesListWidget::on_CreateRTlibButton_clicked()
{
    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
     return;

    std::string lib_name = ui->lineEdit->text().toUtf8().data();

    storage->CreateRuntimeCollection(lib_name);

    AUpdateInterface();
}

void UClassesListWidget::on_AddNewClassButton_clicked()
{
    RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();

    if(!engine)
     return;

    std::string lib_name        = ui->lineEdit->text().toUtf8().data();
    //std::string new_comp_name   = ui->CompName->text().toUtf8().data();


    auto container = engine->GetModel()->GetComponent("Capture", true);

    engine->GetEnvironment()->
            GetStorage()->AddClassToCollection(lib_name,true,container,"NewLiba");

}

void UClassesListWidget::on_DeleteClassButton_clicked()
{
    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
     return;

    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    QString lib_name = item->text();

    item = ui->listWidgetRTlibClasses->currentItem();
    QString class_name = item->text();
    //TODO удаление + удаление файла
    storage->DelClassFromCollection(class_name.toUtf8().data(), lib_name.toUtf8().data());
    AUpdateInterface();
}

void UClassesListWidget::on_listWidgetRTlibs_itemSelectionChanged()
{
    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    QString lib_name = item->text().toUtf8().data();

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

void UClassesListWidget::on_DeleteLibButton_clicked()
{
    QListWidgetItem* item = ui->listWidgetRTlibs->currentItem();
    QString lib_name = item->text().toUtf8().data();

    RDK::UELockPtr<RDK::UStorage> storage=RDK::GetStorageLock();

    if(!storage)
     return;

    storage->DelCollection(lib_name.toLocal8Bit().data());

    AUpdateInterface();
}


