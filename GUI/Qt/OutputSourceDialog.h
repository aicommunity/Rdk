#ifndef OUTPUTSOURCEDIALOG_H
#define OUTPUTSOURCEDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QItemSelectionModel>
#include <QSplitter>
#include <QStringListModel>
#include <QTreeView>
#include <QTableView>
#include <QVBoxLayout>

#include "ModelStructure.h"

class OutputSourceDialog : public QDialog
{
 Q_OBJECT

public:
 OutputSourceDialog(QWidget *parent);
 OutputSourceDialog(QPair<QString, QString>& changes, QWidget *parent);
 ~OutputSourceDialog();

private:
 ModelStructure* model;
 QTreeView* modelStructure;
 QTableView* outputs;
 QDialogButtonBox* buttonBox;
 QVBoxLayout* mainLayout;
 QSplitter* splitter;
 QStringListModel* outputsModel;
 QItemSelectionModel *structureSelection;
 QItemSelectionModel *outputSelection;
 QString currentComponent;
 QPair<QString, QString>* newOutput;

 QSize sizeHint() const;

private slots:
 void ModelSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
 void OutputSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
};

#endif // OUTPUTSOURCEDIALOG_H
