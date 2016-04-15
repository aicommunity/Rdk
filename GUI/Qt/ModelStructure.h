#ifndef MODELSTRUCTURE_H
#define MODELSTRUCTURE_H

#include <QStandardItemModel>

class ModelStructure : public QStandardItemModel
{
 Q_OBJECT

public:
    ModelStructure(QObject *parent);
    ~ModelStructure();

    QString GetComponentLongName(const QModelIndex index);

private:
    
};

#endif // MODELSTRUCTURE_H
