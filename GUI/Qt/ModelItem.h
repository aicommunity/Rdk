#ifndef MODELITEM_H
#define MODELITEM_H

#include <QStandardItem>

class ModelItem : public QStandardItem
{
  //Q_OBJECT

public:
    ModelItem();
    ModelItem(const QString & text);
    ~ModelItem();

private:
    
};

#endif // MODELITEM_H
