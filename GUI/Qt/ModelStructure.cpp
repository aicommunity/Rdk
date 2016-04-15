#include "ModelStructure.h"
#include "ModelItem.h"

ModelStructure::ModelStructure(QObject *parent)
 : QStandardItemModel(parent)
{
 QStandardItem *modelRoot = invisibleRootItem();
 ModelItem *item = new ModelItem("");
 QList <QStandardItem *> items;
 items << item;
 modelRoot->appendColumn(items);
}

ModelStructure::~ModelStructure()
{

}

QString ModelStructure::GetComponentLongName(const QModelIndex index)
{
 QString longName = index.data(Qt::DisplayRole).toString();

 QModelIndex seekRoot = index;
 while(seekRoot.parent().data(Qt::DisplayRole).toString() != "")
 {
  seekRoot = seekRoot.parent();
  longName = QString(seekRoot.data(Qt::DisplayRole).toString() + "." + longName);
 }

 return longName;
}