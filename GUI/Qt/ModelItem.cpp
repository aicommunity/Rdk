#include "../../../Rtv/Deploy/Include/rtv_initdll.h"

#include <QStringList>

#include "ModelItem.h"

ModelItem::ModelItem()
  : QStandardItem()
{
  
}

ModelItem::ModelItem(const QString & text)
{
  setText(text);
  if(Model_GetNumComponents(text.toLocal8Bit()) > 0)
  {
    // create its children
    QStringList componentNames = QString(Model_GetComponentsNameList(text.toLocal8Bit())).split(",");
    QString str;
    QList<QStandardItem *> itemList;
    foreach(str, componentNames)
    {
      itemList << new ModelItem(str);
    }
    appendColumn(itemList);
  }
}

ModelItem::~ModelItem()
{

}
