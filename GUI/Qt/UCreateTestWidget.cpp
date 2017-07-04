#include "UCreateTestWidget.h"
#include "ui_UCreateTestWidget.h"

#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QFileDialog>
#include <QDir>

UCreateTestWidget::UCreateTestWidget(QWidget *parent, QString settingsFile, QString settingsGroup) :
  UVisualControllerWidget(parent),
  ui(new Ui::UCreateTestWidget)
{
  ui->setupUi(this);
  componentsList = NULL;

  readSettings(settingsFile, settingsGroup);

  componentsList = new UComponentsListWidget(this, settingsFile, settingsGroup+"_componentsList");
  componentsList->setVerticalOrientation(false);
  componentsList->openTabN(1);
  componentsList->setEnableTabN(2, false);
  ui->verticalLayoutComponentsList->addWidget(componentsList);

  connect(ui->pushButtonAddProperty, SIGNAL(pressed()), this, SLOT(addProperty()));
  connect(ui->pushButtonCreateTest, SIGNAL(pressed()), this, SLOT(createTest()));
  connect(ui->pushButtonCancel, SIGNAL(pressed()), this, SIGNAL(closeWindow()));
}

UCreateTestWidget::~UCreateTestWidget()
{
  delete ui;
}

void UCreateTestWidget::readSettings(QString file, QString group)
{
  QSettings settings(file, QSettings::IniFormat);
  settings.beginGroup(group);
  restoreGeometry(settings.value("geometry").toByteArray());
  ui->splitter->restoreState(settings.value("splitterState").toByteArray());
  ui->treeWidgetSelectedProperties->header()->restoreState(settings.value("treeWidgetSelectedProperties").toByteArray());
  settings.endGroup();

  if(componentsList) componentsList->readSettings(file, group+"_componentsList");
}

void UCreateTestWidget::writeSettings(QString file, QString group)
{
  QSettings settings(file, QSettings::IniFormat);
  settings.beginGroup(group);
  settings.setValue("geometry", saveGeometry());
  settings.setValue("splitterState", ui->splitter->saveState());
  settings.setValue("treeWidgetSelectedProperties", ui->treeWidgetSelectedProperties->header()->saveState());
  settings.endGroup();

  if(componentsList) componentsList->writeSettings(file, group+"_componentsList");
}

void UCreateTestWidget::addProperty()
{
  QString componentName = componentsList->getSelectedComponentLongName();
  QString propertyName = componentsList->getSelectedPropertyName();

  if(propertyName.isEmpty())
    return;

  try
  {
    RDK::UELockPtr<RDK::UContainer> model = GetModelLock(Core_GetSelectedChannelIndex());

    RDK::UEPtr<RDK::UComponent> component;
    if (componentName.isEmpty())
        component = model;
    else
        component = model->GetComponentL(componentName.toLocal8Bit().constData(), true);

    if(!component) return;

    RDK::UEPtr<RDK::UIProperty> property = component->FindProperty(propertyName.toLocal8Bit().constData());

    if(!property) return;

    std::string buffer;
    QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetSelectedProperties);

    item->setText(0, componentName); // component
    item->setText(1, propertyName); // property
    item->setText(2, QString::fromLocal8Bit(property->GetLanguageType().name())); // type
    item->setText(3, QString::fromStdString(component->GetPropertyValue(propertyName.toLocal8Bit().constData(), buffer))); // value
    item->setText(4, ui->lineEditDelta->text()); // delta
  }
  catch (RDK::UException &exception)
  {
    Log_LogMessage(exception.GetType(), (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
  }
  catch (std::exception &exception)
  {
    Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
  }
}

void UCreateTestWidget::createTest()
{
  RDK::USerStorageXML storage;
  storage.Create("TestCase");

  storage.AddNode("Header");

  storage.AddNode("CalculateDuration");
  if(ui->radioButtonMilliseconds->isChecked())
    storage.SetNodeAttribute("Milliseconds", ui->lineEditPeriodSize->text().toLocal8Bit().constData());
  else
    storage.SetNodeAttribute("Steps", ui->lineEditPeriodSize->text().toLocal8Bit().constData());

  storage.SelectRoot();

  storage.AddNode("TestProperties");

  for(QTreeWidgetItemIterator it(ui->treeWidgetSelectedProperties); *it; ++it)
  {
    storage.AddNode("Property");
    storage.SetNodeAttribute("Component", (*it)->data(0, Qt::DisplayRole).toString().toLocal8Bit().constData());
    storage.SetNodeAttribute("PropertyName", (*it)->data(1, Qt::DisplayRole).toString().toLocal8Bit().constData());
    storage.SetNodeAttribute("type", (*it)->data(2, Qt::DisplayRole).toString().toLocal8Bit().constData());
    storage.SetNodeAttribute("Delta", (*it)->data(4, Qt::DisplayRole).toString().toLocal8Bit().constData());
    storage.SetNodeText((*it)->data(3, Qt::DisplayRole).toString().toLocal8Bit().constData());
    storage.SelectUp();
  }

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save test XML"), QApplication::applicationDirPath()+"/../../../Configs");

  storage.SelectRoot();
  storage.SelectNode("Header");
  storage.AddNode("ConfigFilePath");

  QString relativeFilePath = QDir::current().relativeFilePath(fileName);

  storage.SetNodeText(relativeFilePath.toLocal8Bit().constData());

  storage.SaveToFile(fileName.toLocal8Bit().constData());
}













