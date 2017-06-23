#include "UCreateTestWidget.h"
#include "ui_UCreateTestWidget.h"

#include <QTreeWidgetItem>

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
  QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetSelectedProperties);
  QString componentName = componentsList->getSelectedComponentLongName();
  QString propertyName = componentsList->getSelectedPropertyName();
  item->setText(0, componentName); // component
  item->setText(1, propertyName); // property
  const char *componentValue = Model_GetComponentPropertyValue(componentName.toLocal8Bit(), propertyName.toLocal8Bit());
  item->setText(2, QString::fromLocal8Bit(componentValue)); // value
  Engine_FreeBufString(componentValue);
  item->setText(3, ui->lineEditDelta->text()); // delta
}

void UCreateTestWidget::createTest()
{

}
