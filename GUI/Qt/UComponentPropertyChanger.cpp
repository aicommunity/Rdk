#include "UComponentPropertyChanger.h"
#include "UComponentPropertySelectionWidget.h"
#include "ui_UComponentPropertyChanger.h"

UComponentPropertyChanger::UComponentPropertyChanger(QWidget *parent, RDK::UApplication *app) :
  UVisualControllerWidget(parent, app),
  ui(new Ui::UComponentPropertyChanger)
{
  ui->setupUi(this);

  UpdateInterval = 0;
  setAccessibleName("UComponentPropertyChanger");
  propertyXML = NULL;

  componentsList = new UComponentsListWidgetModern(this, application);
  componentsList->setAccessibleName(accessibleName() + "_componentsList");
  ui->verticalLayoutComponentsList->addWidget(componentsList);

  // Тулбар внизу: отдельные действия без кнопки Set (значение правится в дереве)
  ui->toolBarControll->addAction(ui->actionSetGlobal);
  ui->toolBarControll->addAction(ui->actionSetGlobalOwner);
  ui->toolBarControll->addAction(ui->actionReload);
  ui->toolBarControll->addAction(ui->actionShowXML);
  connect(ui->actionReload, SIGNAL(triggered()), componentsList, SLOT(reloadPropertys()));
  connect(ui->actionSetGlobal, SIGNAL(triggered()), this, SLOT(actionSetGlobal()));
  connect(ui->actionSetGlobalOwner, SIGNAL(triggered()), this, SLOT(actionSetGlobalOwner()));
  connect(ui->actionShowXML, SIGNAL(triggered()), this, SLOT(actionShowXML()));

  connect(componentsList, SIGNAL(selectedPropertyValue(QString)), this, SLOT(updateCurrentPropertyValue(QString)));

  ALoadParameters();
}

UComponentPropertyChanger::~UComponentPropertyChanger()
{
  delete ui;
}

void UComponentPropertyChanger::ASaveParameters()
{
  if(!application) return;

  if(componentsList)
    componentsList->ASaveParameters();
}

void UComponentPropertyChanger::ALoadParameters()
{
  if(!application) return;

  if(componentsList)
    componentsList->ALoadParameters();
}

void UComponentPropertyChanger::actionSetGlobal()
{
  if(emptySeletion())
    return;

  UComponentPropertySelectionWidget dialog(this, 3, application);
  QString selected_component_name;
  dialog.setModal(true);
  if (dialog.exec())
  {
       selected_component_name = dialog.componentsList->getSelectedComponentLongName();
  }
  else
   return;

  const char *className = Model_GetComponentClassName(componentName.toLocal8Bit());

  Model_SetGlobalComponentPropertyValue(
              selected_component_name.toLocal8Bit(),
              className,
              propertyName.toLocal8Bit(),
              m_currentPropertyValue.toLocal8Bit());

  Engine_FreeBufString(className);

  componentsList->reloadPropertys();
}

void UComponentPropertyChanger::actionSetGlobalOwner()
{
  if(emptySeletion())
    return;

  UComponentPropertySelectionWidget dialog(this, 3, application);
  QString selected_component_name;
  dialog.setModal(true);
  if (dialog.exec())
  {
       selected_component_name = dialog.componentsList->getSelectedComponentLongName();
  }
  else
   return;


  const char *pClassName = Model_GetComponentClassName(componentName.toLocal8Bit());
  std::string className = pClassName;
  Engine_FreeBufString(pClassName);

  QStringList list = componentName.split(".");
  list.pop_back();
  QString ownerComponentName = list.join(".");

  const char *pOwnerClassName = Model_GetComponentClassName(ownerComponentName.toLocal8Bit());
  std::string ownerClassName = pOwnerClassName;
  Engine_FreeBufString(pOwnerClassName);


  Model_SetGlobalOwnerComponentPropertyValue(
              selected_component_name.toLocal8Bit(),
              className.c_str(),
              ownerClassName.c_str(),
              propertyName.toLocal8Bit(),
              m_currentPropertyValue.toLocal8Bit());



  componentsList->reloadPropertys();
}

void UComponentPropertyChanger::actionShowXML()
{
  componentName = componentsList->getSelectedComponentLongName();

  if(componentName.isEmpty())
    return;

  if(!propertyXML)
  {
    propertyXML = new UPropertyXMLWidget(this);
    connect(propertyXML, &UPropertyXMLWidget::propertiesApplied, this, [this]() {
      if(componentsList)
        componentsList->reloadPropertys(true);
    });
  }
  propertyXML->initWidget(componentName, componentsList->currentPropertyXmlMask());
  propertyXML->show();
}

void UComponentPropertyChanger::updateCurrentPropertyValue(QString value)
{
  m_currentPropertyValue = value;
}

bool UComponentPropertyChanger::emptySeletion()
{
  componentName = componentsList->getSelectedComponentLongName();
  propertyName = componentsList->getSelectedPropertyName();

  if(propertyName.isEmpty())
    return true;

  return false;
}
