#include "UComponentPropertyChanger.h"
#include "ui_UComponentPropertyChanger.h"

#include <QMenu>
#include <QToolButton>

UComponentPropertyChanger::UComponentPropertyChanger(QWidget *parent, QString settingsFile, QString settingsGroup) :
  QWidget(parent),
  ui(new Ui::UComponentPropertyChanger)
{
  ui->setupUi(this);

  propertyXML = NULL;

  componentsList = new UComponentsListWidget(this, settingsFile, settingsGroup+"_componentsList");
  ui->verticalLayoutComponentsList->addWidget(componentsList);

  ui->splitter->setStretchFactor(0, 10);
  ui->splitter->setStretchFactor(1, 1);

  readSettings(settingsFile, settingsGroup);

  //кнопки управления для фиксации Property компонента
  //Property tab
  QMenu *setPropertyMenu = new QMenu(this);
  setPropertyMenu->addAction(ui->actionSetGlobal);
  setPropertyMenu->addAction(ui->actionSetGlobalOwner);
  QToolButton *setProperty = new QToolButton(this);
  setProperty->setText("Set");
  setProperty->setMenu(setPropertyMenu);
  setProperty->setPopupMode(QToolButton::MenuButtonPopup);
  ui->toolBarControll->addWidget(setProperty);
  ui->toolBarControll->addAction(ui->actionReload);
  ui->toolBarControll->addAction(ui->actionDefaultAll);
  ui->toolBarControll->addAction(ui->actionShowXML);
  connect(setProperty, SIGNAL(clicked()), this, SLOT(actionSet()));
  connect(ui->actionReload, SIGNAL(triggered()), componentsList, SLOT(reloadPropertys()));
  connect(ui->actionSetGlobal, SIGNAL(triggered()), this, SLOT(actionSetGlobal()));
  connect(ui->actionSetGlobalOwner, SIGNAL(triggered()), this, SLOT(actionSetGlobalOwner()));
  connect(ui->actionDefaultAll, SIGNAL(triggered()), this, SLOT(actionDefaultAll()));
  connect(ui->actionShowXML, SIGNAL(triggered()), this, SLOT(actionShowXML()));

  connect(componentsList, SIGNAL(selectedPropertyValue(QString)), this, SLOT(updateCurrentPropertyValue(QString)));
}

UComponentPropertyChanger::~UComponentPropertyChanger()
{
  delete ui;
}

void UComponentPropertyChanger::actionSet()
{
  if(emptySeletion())
    return;

  Model_SetComponentPropertyValue(
              componentName.toLocal8Bit(),
              propertyName.toLocal8Bit(),
              ui->plainTextEditValue->toPlainText().toLocal8Bit());
  componentsList->reloadPropertys();
}

void UComponentPropertyChanger::actionSetGlobal()
{
  if(emptySeletion())
    return;

  const char *className = Model_GetComponentClassName(componentName.toLocal8Bit());

  Model_SetGlobalComponentPropertyValue(
              componentName.toLocal8Bit(),
              className,
              propertyName.toLocal8Bit(),
              ui->plainTextEditValue->toPlainText().toLocal8Bit());

  Engine_FreeBufString(className);

  componentsList->reloadPropertys();
}

void UComponentPropertyChanger::actionSetGlobalOwner()
{
  if(emptySeletion())
    return;

  const char *className = Model_GetComponentClassName(componentName.toLocal8Bit());

  QStringList list = componentName.split(".");
  list.pop_back();
  QString ownerComponentName = list.join(".");

  Model_SetGlobalOwnerComponentPropertyValue(
              componentName.toLocal8Bit(),
              className,
              Model_GetComponentClassName(ownerComponentName.toLocal8Bit()), //тут вопрос о владельце
              propertyName.toLocal8Bit(),
              ui->plainTextEditValue->toPlainText().toLocal8Bit());

  Engine_FreeBufString(className);

  componentsList->reloadPropertys();
}

void UComponentPropertyChanger::actionDefaultAll()
{
  Env_Default(componentName.toLocal8Bit(), false);
}

void UComponentPropertyChanger::actionShowXML()
{
  componentName = componentsList->getSelectedComponentLongName();

  if(!propertyXML) propertyXML = new UPropertyXMLWidget(this);
  switch(componentsList->currentTabIndex())
  {
    case 0:
      propertyXML->initWidget(componentName, ptPubParameter);
      break;

    case 1:
      propertyXML->initWidget(componentName, ptPubState);
      break;

    case 2:
      propertyXML->initWidget(componentName, ptPubInput);
      break;

    case 3:
      propertyXML->initWidget(componentName, ptPubOutput);
      break;

    default:
      propertyXML->initWidget(componentName, ptPubParameter);
      break;
  }

  propertyXML->show();
}

void UComponentPropertyChanger::updateCurrentPropertyValue(QString value)
{
//  ui->plainTextEditValue->clear();
  ui->plainTextEditValue->setPlainText(value);
}

void UComponentPropertyChanger::readSettings(QString file, QString group)
{
  QSettings settings(file, QSettings::IniFormat);
  settings.beginGroup(group);
  ui->splitter->restoreState(settings.value("splitterState").toByteArray());
  settings.endGroup();

  if(componentsList) componentsList->readSettings(file, group+"_componentsList");
}

void UComponentPropertyChanger::writeSettings(QString file, QString group)
{
  QSettings settings(file, QSettings::IniFormat);
  settings.beginGroup(group);
  settings.setValue("splitterState", ui->splitter->saveState());
  settings.endGroup();

  if(componentsList) componentsList->writeSettings(file, group+"_componentsList");
}

bool UComponentPropertyChanger::emptySeletion()
{
  componentName = componentsList->getSelectedComponentLongName();
  propertyName = componentsList->getSelectedPropertyName();

  if(propertyName.isEmpty())
    return true;

  return false;
}
