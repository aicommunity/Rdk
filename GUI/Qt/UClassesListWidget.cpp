#include "UClassesListWidget.h"
#include "ui_UClassesListWidget.h"

#include <QMimeData>
#include <QDragMoveEvent>
#include <QDrag>
#include <QDebug>

UClassesListWidget::UClassesListWidget(QWidget *parent) :
    UVisualControllerWidget(parent),
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
