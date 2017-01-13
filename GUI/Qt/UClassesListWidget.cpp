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

    //����� ������� �� ��������� ��� ������� ��������������
    connect(ui->treeWidgetStorageByLibs, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));
    connect(ui->listWidgetStorageByName, SIGNAL(pressed(QModelIndex)), this, SLOT(dragEvent(QModelIndex)));
}

UClassesListWidget::~UClassesListWidget()
{
    delete ui;
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
