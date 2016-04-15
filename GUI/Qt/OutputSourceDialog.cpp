#include "../../../Rtv/Deploy/Include/rtv_initdll.h"

#include "OutputSourceDialog.h"
#include "MainWindow.h"

#include <QDebug>

//#include <qstringlist>


OutputSourceDialog::OutputSourceDialog(QWidget *parent)
 : QDialog(parent)
{

}

OutputSourceDialog::OutputSourceDialog(QPair<QString, QString>& changes, QWidget *parent)
 : QDialog(parent)
{
 newOutput = &changes;
 //setAttribute(Qt::WA_DeleteOnClose, true);

 QObject* searchMainWindow = this;
 while(searchMainWindow->parent() != 0)
 {
  searchMainWindow = searchMainWindow->parent();
 }
 model = qobject_cast<MainWindow *>(searchMainWindow)->GetModel();
 qDebug() << "Reached: " << searchMainWindow;

 modelStructure = new QTreeView(this);
 modelStructure->setHeaderHidden(true);
 modelStructure->setModel(model);
 modelStructure->expandAll();
 modelStructure->setEditTriggers(QAbstractItemView::NoEditTriggers);

 structureSelection = modelStructure->selectionModel();
 connect(structureSelection, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
         this, SLOT(ModelSelectionChanged(const QItemSelection &, const QItemSelection &)));
 
 outputsModel = new QStringListModel(this);

 outputs = new QTableView(this);
 outputs->setModel(outputsModel);
 outputs->setEditTriggers(QAbstractItemView::NoEditTriggers);
 
 outputSelection = outputs->selectionModel();
 connect(outputSelection, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
         this, SLOT(OutputSelectionChanged(const QItemSelection &, const QItemSelection &)));
 
 buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
 connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
 connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
 
 splitter = new QSplitter(this);
 splitter->setChildrenCollapsible(false);
 splitter->addWidget(modelStructure);
 splitter->addWidget(outputs);

 mainLayout = new QVBoxLayout(this);
 mainLayout->addWidget(splitter);
 mainLayout->addWidget(buttonBox);
}

OutputSourceDialog::~OutputSourceDialog()
{

}

QSize OutputSourceDialog::sizeHint() const
{
 return QSize(1000, 900);
}

void OutputSourceDialog::ModelSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
 currentComponent = model->GetComponentLongName(modelStructure->currentIndex());

 // is it OK that it's created here?
 QStringList outputsList;
 int numberOfOutputs = Model_GetComponentNumOutputs(currentComponent.toLocal8Bit());
 for(int i = 0; i < numberOfOutputs; i++)
 {
  outputsList << QString("%1").arg(i);
 }
 outputsModel->setStringList(outputsList);
}

void OutputSourceDialog::OutputSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
 newOutput->first = currentComponent;
 const QModelIndex index = outputs->currentIndex();
 newOutput->second = index.data(Qt::DisplayRole).toString();
}
