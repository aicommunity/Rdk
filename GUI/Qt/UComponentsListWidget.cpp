#include "UComponentsListWidget.h"
#include "ui_UComponentsListWidget.h"

#include <QDebug>
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include <QList>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QScrollBar>

UComponentsListWidget::UComponentsListWidget(QWidget *parent, RDK::UApplication *app, int channel_mode) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UComponentsListWidget)
{
    channelMode=channel_mode;
    CheckModelFlag=false;
    ui->setupUi(this);

    componentsTree = new UComponentListTreeWidget(this);
    //ui->verticalLayoutTreeWidget->setMargin(0);
    ui->horizontalLayoutTreeWidget->addWidget(componentsTree);
    connect(componentsTree, SIGNAL(moveComponentUp()), this, SLOT(componentMoveUp()));
    connect(componentsTree, SIGNAL(moveComponentDown()), this, SLOT(componentMoveDown()));

    currentChannel = Core_GetSelectedChannelIndex();
    ui->listWidgetChannelSelection->hide();
    channelsSelectionVisible = false;

    UpdateInterval = -1;
    setAccessibleName("UComponentsListWidget"); // им€ класса дл€ сериализации
    //readSettings(app, settingsGroup);

    UpdateInterface(true);

    //выделение элемента списка
    connect(componentsTree, SIGNAL(itemSelectionChanged()),
            this, SLOT(componentListItemSelectionChanged()));

    //отрисовка выделенного элемента списка по даблклику
    connect(componentsTree, SIGNAL(doubleClicked(QModelIndex)),
            this, SLOT(drawSelectedComponent(QModelIndex)));

    //выделение элменетов propertys
    connect(ui->treeWidgetParameters, SIGNAL(itemSelectionChanged()),
            this, SLOT(parametersListSelectionChanged()));
    connect(ui->treeWidgetState, SIGNAL(itemSelectionChanged()),
            this, SLOT(stateListSelectionChanged()));
    connect(ui->treeWidgetInputs, SIGNAL(itemSelectionChanged()),
            this, SLOT(inputsListSelectionChanged()));
    connect(ui->treeWidgetOutputs, SIGNAL(itemSelectionChanged()),
            this, SLOT(outputsListSelectionChanged()));
    connect(ui->treeWidgetFavorites, SIGNAL(itemSelectionChanged()),
            this, SLOT(favoritesListSelectionChanged()));

    connect(ui->treeWidgetParameters, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
            this, SLOT(parametersListItemChanged(QTreeWidgetItem *, int )));
    connect(ui->treeWidgetFavorites, SIGNAL(itemChanged(QTreeWidgetItem *, int )),
            this, SLOT(favoritesListItemChanged(QTreeWidgetItem *, int )));

    //выделение канала
    connect(ui->listWidgetChannelSelection, SIGNAL(itemSelectionChanged()), this, SLOT(channelsListSelectionChanged()));

    //контекстное меню дл€ дерева компонентов
    QAction *actionSeparator1 = new QAction(this);
    actionSeparator1->setSeparator(true);
    QAction *actionSeparator2 = new QAction(this);
    actionSeparator2->setSeparator(true);
    QAction *actionSeparator3 = new QAction(this);
    actionSeparator3->setSeparator(true);
    QAction *actionSeparator4 = new QAction(this);
    actionSeparator4->setSeparator(true);
    QAction *actionSeparator5 = new QAction(this);
    actionSeparator5->setSeparator(true);
    componentsTree->addAction(ui->actionComponentMoveUp);
    componentsTree->addAction(ui->actionComponentMoveDown);
    componentsTree->addAction(actionSeparator1);
    componentsTree->addAction(ui->actionComponentRename);
    componentsTree->addAction(ui->actionComponentDelete);
    componentsTree->addAction(actionSeparator2);
    componentsTree->addAction(ui->actionComponentReset);
    componentsTree->addAction(ui->actionComponentCalculate);
    componentsTree->addAction(ui->actionComponentInit);
    componentsTree->addAction(ui->actionComponentUnInit);
    componentsTree->addAction(actionSeparator3);
    componentsTree->addAction(ui->actionCopyNameToClipboard);
    componentsTree->addAction(ui->actionCopyLongNameToClipboard);
    componentsTree->addAction(ui->actionCopyClassNameToClipboard);
    componentsTree->addAction(actionSeparator4);
    componentsTree->addAction(ui->actionComponentGUI);
    componentsTree->addAction(actionSeparator5);
    componentsTree->addAction(ui->actionReloadTree);
    ui->actionComponentGUI->setEnabled(false); // нет реализации, по этому ставим в enable
    connect(ui->actionComponentMoveUp, SIGNAL(triggered()), this, SLOT(componentMoveUp()));
    connect(ui->actionComponentMoveDown, SIGNAL(triggered()), this, SLOT(componentMoveDown()));
    connect(ui->actionComponentRename, SIGNAL(triggered()), this, SLOT(componentRename()));
    connect(ui->actionComponentDelete, SIGNAL(triggered()), this, SLOT(componentDelete()));
    connect(ui->actionComponentReset, SIGNAL(triggered()), this, SLOT(componentReset()));
    connect(ui->actionComponentCalculate, SIGNAL(triggered()), this, SLOT(componentCalculate()));
    connect(ui->actionComponentInit, SIGNAL(triggered()), this, SLOT(componentInit()));
    connect(ui->actionComponentUnInit, SIGNAL(triggered()), this, SLOT(componentUnInit()));
    connect(ui->actionCopyNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyNameToClipboard()));
    connect(ui->actionCopyLongNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyLongNameToClipboard()));
    connect(ui->actionCopyClassNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyClassNameToClipboard()));
    connect(ui->actionComponentGUI, SIGNAL(triggered()), this, SLOT(componentGUI()));

    QAction *actionSeparatorProperty1 = new QAction(this);
    actionSeparatorProperty1->setSeparator(true);
    ui->treeWidgetParameters->addAction(ui->actionCopyPropertyNameToClipboard);
    ui->treeWidgetState->addAction(ui->actionCopyPropertyNameToClipboard);
    ui->treeWidgetInputs->addAction(ui->actionCopyPropertyNameToClipboard);
    ui->treeWidgetOutputs->addAction(ui->actionCopyPropertyNameToClipboard);
    connect(ui->actionCopyPropertyNameToClipboard, SIGNAL(triggered()), this, SLOT(propertyCopyNameToClipboard()));

    QAction *actionSeparatorProperty2 = new QAction(this);
    actionSeparatorProperty2->setSeparator(true);
    ui->treeWidgetParameters->addAction(ui->actionCopyPropertyValueToClipboard);
    ui->treeWidgetState->addAction(ui->actionCopyPropertyValueToClipboard);
    ui->treeWidgetInputs->addAction(ui->actionCopyPropertyValueToClipboard);
    ui->treeWidgetOutputs->addAction(ui->actionCopyPropertyValueToClipboard);
    connect(ui->actionCopyPropertyValueToClipboard, SIGNAL(triggered()), this, SLOT(propertyCopyValueToClipboard()));

    QAction *actionSeparatorProperty3 = new QAction(this);
    actionSeparatorProperty3->setSeparator(true);
    ui->treeWidgetParameters->addAction(ui->actionPastePropertyValueFromClipboard);
    ui->treeWidgetState->addAction(ui->actionPastePropertyValueFromClipboard);
    ui->treeWidgetInputs->addAction(ui->actionPastePropertyValueFromClipboard);
    ui->treeWidgetOutputs->addAction(ui->actionPastePropertyValueFromClipboard);
    connect(ui->actionPastePropertyValueFromClipboard, SIGNAL(triggered()), this, SLOT(propertyPasteValueFromClipboard()));
}

UComponentsListWidget::~UComponentsListWidget()
{
    delete ui;
}

void UComponentsListWidget::updateComponentsListFromScheme()
{
    UpdateInterface(true);
}

void UComponentsListWidget::AUpdateInterface()
{
    QString oldRootItem = currentDrawComponentName,
            oldSelectedItem = selectedComponentLongName;

    // чтоб не бегали скролы на treeWidget'ах
    int componentsListScrollMaximum = componentsTree->verticalScrollBar()->maximum();
    int componentsListScrollPosition = componentsTree->verticalScrollBar()->value();

    componentsTree->clear();

    //«аполнение списка
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(componentsTree);
    rootItem->setText(0, "Model");
    rootItem->setExpanded(false);
    addComponentSons("", rootItem, oldRootItem, oldSelectedItem);

    // чтоб не бегали скролы на treeWidget'ах
    componentsTree->verticalScrollBar()->setMaximum(componentsListScrollMaximum);
    componentsTree->verticalScrollBar()->setValue(componentsListScrollPosition);

    if(channelsSelectionVisible)
    {
      redrawChannelsList();
    }
}

void UComponentsListWidget::AClearInterface()
{
 componentsTree->clear();
}

void UComponentsListWidget::ASaveParameters()
{
    if(!application) return;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    settings.setValue("splitterState", ui->splitter->saveState());
    settings.setValue("treeWidgetInputsHeader", ui->treeWidgetInputs->header()->saveState());
    settings.setValue("treeWidgetOutputsHeader", ui->treeWidgetOutputs->header()->saveState());
    settings.setValue("treeWidgetParameters", ui->treeWidgetParameters->header()->saveState());
    settings.setValue("treeWidgetState", ui->treeWidgetState->header()->saveState());
    settings.endGroup();
}

void UComponentsListWidget::ALoadParameters()
{
    if(!application) return;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    ui->splitter->restoreState(settings.value("splitterState").toByteArray());
    ui->treeWidgetInputs->header()->restoreState(settings.value("treeWidgetInputsHeader").toByteArray());
    ui->treeWidgetOutputs->header()->restoreState(settings.value("treeWidgetOutputsHeader").toByteArray());
    ui->treeWidgetParameters->header()->restoreState(settings.value("treeWidgetParameters").toByteArray());
    ui->treeWidgetState->header()->restoreState(settings.value("treeWidgetState").toByteArray());
    settings.endGroup();
}

void UComponentsListWidget::setVerticalOrientation(bool vertical)
{
    if (vertical)
        ui->splitter->setOrientation(Qt::Vertical);
    else
        ui->splitter->setOrientation(Qt::Horizontal);
}

QString UComponentsListWidget::getSelectedComponentLongName()
{
    if(!componentsTree->currentItem())
      return "";
    else
    {
      if(ui->tabWidgetComponentInfo->currentWidget() == ui->tabFavorites)
      {
        if(ui->treeWidgetFavorites->currentItem())
            return ui->treeWidgetFavorites->currentItem()->data(1, Qt::UserRole).toString();
        else
            return "";
      }
      else
      {
        return componentsTree->currentItem()->data(0, Qt::UserRole).toString();
      }
    }

}

void UComponentsListWidget::openTabN(int n)
{
    if(n > 0 && n < 4)
      ui->tabWidgetComponentInfo->setCurrentIndex(n);
}

int UComponentsListWidget::currentTabIndex()
{
    return ui->tabWidgetComponentInfo->currentIndex();
}

QString UComponentsListWidget::getSelectedPropertyName()
{
  switch(ui->tabWidgetComponentInfo->currentIndex())
  {
    case 0:
      if(!ui->treeWidgetParameters->currentItem())
        return "";
      else
        return ui->treeWidgetParameters->currentItem()->data(0, Qt::DisplayRole).toString();

    case 1:
      if(!ui->treeWidgetState->currentItem())
        return "";
      else
        return ui->treeWidgetState->currentItem()->data(0, Qt::DisplayRole).toString();

    case 2:
      if(!ui->treeWidgetInputs->currentItem())
        return "";
      else
        return ui->treeWidgetInputs->currentItem()->data(0, Qt::DisplayRole).toString();

    case 3:
      if(!ui->treeWidgetOutputs->currentItem())
        return "";
      else
        return ui->treeWidgetOutputs->currentItem()->data(0, Qt::DisplayRole).toString();
    case 4:
      if(!ui->treeWidgetFavorites->currentItem())
        return "";
      else
        return ui->treeWidgetFavorites->currentItem()->data(0, Qt::UserRole).toString();
    default:
      return "";
  }
}

int UComponentsListWidget::getSelectedChannelIndex()
{
    return currentChannel;
}

/// –ежим выбора канала
/// 0 - всегда работа с текущим каналом
/// 1 - работа с изначально заданным каналом
void UComponentsListWidget::setChannelMode(int mode)
{
 channelMode=mode;
}

/// ¬озвращает номер рабочего канала
/// используемый при отображении информации
int UComponentsListWidget::getWorkChannelIndex()
{
 return (channelMode == 0)?Core_GetSelectedChannelIndex():currentChannel;
}


void UComponentsListWidget::setEnableTabN(int n, bool enable)
{
    if(n >= 0 && n < 4)
      ui->tabWidgetComponentInfo->setTabEnabled(n, enable);
}

void UComponentsListWidget::setChannelsListVisible(bool value)
{
    ui->listWidgetChannelSelection->setVisible(value);
    channelsSelectionVisible = value;
    redrawChannelsList();
}

void UComponentsListWidget::componentListItemSelectionChanged()
{
    QTreeWidgetItem * item = componentsTree->currentItem();
    if(!item) return;
    
    selectedComponentLongName = item->data(0,Qt::UserRole).toString();

    reloadPropertys();

    // говорим схеме выделить елемент
    emit componentSelected(selectedComponentLongName);
}

void UComponentsListWidget::reloadPropertys(bool forceReload)
{
    if(currentDrawPropertyComponentName == selectedComponentLongName && !forceReload)
      return;

    currentDrawPropertyComponentName = selectedComponentLongName;

    std::map<std::string, std::string> Favorites;

    //Class
    const char *className=MModel_GetComponentClassName(getWorkChannelIndex(), currentDrawPropertyComponentName.toLocal8Bit());

    if(className)
    {
        auto class_desc = RDK::GetStorageLock()->GetClassDescription(className, true);

        if(class_desc)
            Favorites = class_desc->GetFavorites();

        ui->labelComponentClassName->setText(className);
    }
    Engine_FreeBufString(className);

    ui->treeWidgetParameters->clear();
    ui->treeWidgetState->clear();
    ui->treeWidgetInputs->clear();
    ui->treeWidgetOutputs->clear();
    ui->treeWidgetFavorites->clear();

    // чтоб не бегали скролы на treeWidget'ах
    int paramScrollPosition = ui->treeWidgetParameters->verticalScrollBar()->value(),
        stateScrollPosition = ui->treeWidgetState->verticalScrollBar()->value(),
        inputsScrollPosition = ui->treeWidgetInputs->verticalScrollBar()->value(),
        outputsScrollPosition = ui->treeWidgetOutputs->verticalScrollBar()->value(),
        favoritesScrollPosition = ui->treeWidgetFavorites->verticalScrollBar()->value();

    try
    {
     UpdateInterfaceFlag=true;
        RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock(getWorkChannelIndex());

        RDK::UEPtr<RDK::UContainer> cont;
        if (currentDrawPropertyComponentName.isEmpty())
            cont = model.Get();
        else
            cont = model->GetComponentL(currentDrawPropertyComponentName.toLocal8Bit().constData(), true);

        if(!cont)
        {
         UpdateInterfaceFlag=false;
         return;
        }
        RDK::UComponent::VariableMapT varMap = cont->GetPropertiesList();
        std::string buffer;

        bool is_new_outputs(false);
        bool is_new_inputs(false);

        for(std::map<RDK::NameT,RDK::UVariable>::iterator i = varMap.begin(); i != varMap.end(); ++i)
        {
            if (i->second.CheckMask(ptPubInput))
            {
             std::string::size_type k=i->first.find("DataInput");
             if(k != 0 || k == std::string::npos)
              is_new_inputs=true;
            }

            if (i->second.CheckMask(ptPubOutput))
            {
             std::string::size_type k=i->first.find("DataOutput");
             if(k != 0 || k == std::string::npos)
              is_new_outputs=true;
            }

            if(is_new_inputs && is_new_outputs)
             break;
        }

        for(std::map<RDK::NameT,RDK::UVariable>::iterator i = varMap.begin(); i != varMap.end();)
        {
            if (i->second.CheckMask(ptPubInput) && is_new_inputs)
            {
             std::string::size_type k=i->first.find("DataInput");
             if(k == 0)
             {
              std::map<RDK::NameT,RDK::UVariable>::iterator j=i; ++j;
              varMap.erase(i);
              i=j;
             }
             else
              ++i;
            }
            else
            if (i->second.CheckMask(ptPubOutput) && is_new_outputs)
            {
             std::string::size_type k=i->first.find("DataOutput");
             if(k == 0)
             {
              std::map<RDK::NameT,RDK::UVariable>::iterator j=i; ++j;
              varMap.erase(i);
              i=j;
             }
             else
              ++i;
            }
            else
            {
             ++i;
            }
        }


        for(std::map<RDK::NameT,RDK::UVariable>::iterator i = varMap.begin(); i != varMap.end(); ++i)
        {
            if (i->second.CheckMask(ptPubParameter))
            {
                QTreeWidgetItem* parametersItem = new QTreeWidgetItem(ui->treeWidgetParameters);
                QString parameterName = QString::fromLocal8Bit(i->first.c_str());
                parametersItem->setText(0, parameterName);
                cont->GetPropertyValue(i->first, buffer);
                parametersItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(buffer.c_str()));
                parametersItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
                if(parameterName == selectedParameterName)
                    ui->treeWidgetParameters->setCurrentItem(parametersItem);
                if(i->second.Property->GetLanguageType() == typeid(bool))
                {
                 parametersItem->setFlags(parametersItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);

                 const bool* val=reinterpret_cast<const bool*>(i->second.Property->GetMemoryArea());
                 if(*val)
                  parametersItem->setCheckState(1,Qt::Checked);
                 else
                  parametersItem->setCheckState(1,Qt::Unchecked);
                }
            }
            if (i->second.CheckMask(ptPubState))
            {
                QTreeWidgetItem* stateItem = new QTreeWidgetItem(ui->treeWidgetState);
                QString stateName = QString::fromLocal8Bit(i->first.c_str());
                stateItem->setText(0, stateName);
                cont->GetPropertyValue(i->first, buffer);
                stateItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(buffer.c_str()));
                stateItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
                if(stateName == selectedStateName)
                    ui->treeWidgetState->setCurrentItem(stateItem);
            }
            if (i->second.CheckMask(ptPubInput))
            {
                QTreeWidgetItem* inputItem = new QTreeWidgetItem(ui->treeWidgetInputs);
                QString inputName = QString::fromLocal8Bit(i->first.c_str());
                inputItem->setText(0, inputName);
                //inputItem->setText(1, QString::fromLocal8Bit(cont->GetPropertyValue(i->first, buffer).c_str()));
                inputItem->setText(2, QString(i->second.Property->GetLanguageType().name()));
                /*if(i->second.Property->GetIoType() & ipRange) inputItem->setText(3, QString("range"));
                else
                    if(i->second.Property->IsConnected()) inputItem->setText(3, QString("connected"));*/
                if(inputName == selectedInputName)
                    ui->treeWidgetInputs->setCurrentItem(inputItem);
            }
            if (i->second.CheckMask(ptPubOutput))
            {
                QTreeWidgetItem* outputItem = new QTreeWidgetItem(ui->treeWidgetOutputs);
                QString outputName = QString::fromLocal8Bit(i->first.c_str());
                outputItem->setText(0, outputName);
                cont->GetPropertyValue(i->first, buffer);
                outputItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(buffer.c_str()));
                outputItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
                outputItem->setText(2, QString(i->second.Property->GetLanguageType().name()));
                if(outputName == selectedOutputName)
                    ui->treeWidgetOutputs->setCurrentItem(outputItem);
            }
        }


        for(std::map<std::string, std::string>::iterator i = Favorites.begin(); i != Favorites.end(); ++i)
        {
            QTreeWidgetItem* favoriteItem = new QTreeWidgetItem(ui->treeWidgetFavorites);

            QString favoriteName = QString::fromLocal8Bit(i->first.c_str());
            QString favoritePath = QString::fromLocal8Bit(i->second.c_str());
            favoritePath.replace("{CompName}", currentDrawPropertyComponentName);

            favoriteItem->setText(0, favoriteName);
            favoriteItem->setText(1, favoritePath);

            favoriteItem->setToolTip(0, favoritePath);
            favoriteItem->setToolTip(1, favoritePath);

            // Parse path
            QStringList vals = favoritePath.split(":");

            QString component_long_name;
            QString prop_name;
            if(vals.size()==2)
            {
                component_long_name = vals[0];
                prop_name = vals[1];
            }

            RDK::UEPtr<RDK::UContainer> child_cont;
            child_cont = model->GetComponentL(component_long_name.toLocal8Bit().constData(), true);

            favoriteItem->setData(1, Qt::UserRole, component_long_name);
            favoriteItem->setData(0, Qt::UserRole, prop_name);

            if(child_cont)
            {
                child_cont->GetPropertyValue(prop_name.toStdString(), buffer);
                auto prop = child_cont->FindProperty(prop_name.toStdString());

                favoriteItem->setData(2, Qt::UserRole, QString::fromLocal8Bit(buffer.c_str()));
                favoriteItem->setText(2, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));

                if(favoriteName == selectedFavName)
                    ui->treeWidgetFavorites->setCurrentItem(favoriteItem);

                if(prop->GetLanguageType() == typeid(bool))
                {
                 favoriteItem->setFlags(favoriteItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);

                 const bool* val=reinterpret_cast<const bool*>(prop->GetMemoryArea());
                 if(*val)
                  favoriteItem->setCheckState(2,Qt::Checked);
                 else
                  favoriteItem->setCheckState(2,Qt::Unchecked);
                }
            }

        }

        // чтоб не бегали скролы на treeWidget'ах
        ui->treeWidgetParameters->verticalScrollBar()->setMaximum(paramScrollPosition);
        ui->treeWidgetParameters->verticalScrollBar()->setValue(paramScrollPosition);
        ui->treeWidgetState->verticalScrollBar()->setMaximum(stateScrollPosition);
        ui->treeWidgetState->verticalScrollBar()->setValue(stateScrollPosition);
        ui->treeWidgetInputs->verticalScrollBar()->setMaximum(inputsScrollPosition);
        ui->treeWidgetInputs->verticalScrollBar()->setValue(inputsScrollPosition);
        ui->treeWidgetOutputs->verticalScrollBar()->setMaximum(outputsScrollPosition);
        ui->treeWidgetOutputs->verticalScrollBar()->setValue(outputsScrollPosition);
        ui->treeWidgetFavorites->verticalScrollBar()->setMaximum(favoritesScrollPosition);
        ui->treeWidgetFavorites->verticalScrollBar()->setValue(favoritesScrollPosition);
        UpdateInterfaceFlag=false;
    }
    catch (RDK::UException &exception)
    {
     UpdateInterfaceFlag=false;
        Log_LogMessage(exception.GetType(), (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
     UpdateInterfaceFlag=false;
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
}

void UComponentsListWidget::parametersListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetParameters->currentItem();
    if(!item)
      return;

    QString changedParameterName = item->data(0, Qt::DisplayRole).toString();
    if(selectedParameterName == changedParameterName)
      return;

    selectedParameterName = changedParameterName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidget::parametersListItemChanged(QTreeWidgetItem *item, int column)
{
 try
 {
  if(UpdateInterfaceFlag)
   return;
  RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock(getWorkChannelIndex());

  RDK::UEPtr<RDK::UContainer> cont;
  if (currentDrawPropertyComponentName.isEmpty())
   cont = model.Get();
  else
   cont = model->GetComponentL(currentDrawPropertyComponentName.toLocal8Bit().constData(), true);

  if(!cont)
   return;

  std::string buffer;
  QString parameterName=item->text(0);
  RDK::UEPtr<RDK::UIProperty> property;

  property=cont->FindProperty(parameterName.toLocal8Bit().constData());

  if(!property)
   return;

//  parametersItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(buffer.c_str()));
//  parametersItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
//  if(parameterName == selectedParameterName)
//   ui->treeWidgetParameters->setCurrentItem(parametersItem);
  if(property->GetLanguageType() == typeid(bool))
  {
//   parametersItem->setFlags(parametersItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);

//   const bool* val=reinterpret_cast<const bool*>(i->second.Property->GetMemoryArea());
   bool value(false);
   if(item->checkState(1) == Qt::Checked)
    value=true;
   else
    value=false;
   property->ReadFromMemory(&value);
  }
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

void UComponentsListWidget::stateListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetState->currentItem();
    if(!item)
      return;

    QString changedStateName = item->data(0, Qt::DisplayRole).toString();
    if(selectedStateName == changedStateName)
      return;

    selectedStateName = changedStateName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidget::inputsListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetInputs->currentItem();
    if(!item)
      return;

    QString changedInputName = item->data(0, Qt::DisplayRole).toString();
    if(selectedInputName == changedInputName)
      return;

    selectedInputName = changedInputName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidget::outputsListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetOutputs->currentItem();
    if(!item)
      return;

    QString changedOutputName = item->data(0, Qt::DisplayRole).toString();
    if(selectedOutputName == changedOutputName)
      return;

    selectedOutputName = changedOutputName;

    emit selectedPropertyValue(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidget::favoritesListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetFavorites->currentItem();
    if(!item)
      return;

    QString changedFavName = item->data(0, Qt::DisplayRole).toString();
    if(selectedFavName == changedFavName)
      return;

    selectedFavName = changedFavName;

    emit selectedPropertyValue(item->data(2, Qt::UserRole).toString());
}

void UComponentsListWidget::favoritesListItemChanged(QTreeWidgetItem *item, int column)
{

try
{
    if(UpdateInterfaceFlag)
        return;

     if(!item)
        return;

     // Parse path
     QStringList vals = item->text(1).split(":");

     QString component_long_name;
     QString prop_name;
     if(vals.size()==2)
     {
         component_long_name = vals[0];
         prop_name = vals[1];
     }


     RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock(getWorkChannelIndex());

     RDK::UEPtr<RDK::UContainer> cont;

     cont = model->GetComponentL(component_long_name.toLocal8Bit().constData(), true);

     if(!cont)
      return;

     std::string buffer;

     RDK::UEPtr<RDK::UIProperty> property;

     property=cont->FindProperty(prop_name.toLocal8Bit().constData());

     if(!property)
      return;

   //  parametersItem->setData(1, Qt::UserRole, QString::fromLocal8Bit(buffer.c_str()));
   //  parametersItem->setText(1, QString::fromLocal8Bit((PreparePropertyValueToListView(buffer)).c_str()));
   //  if(parameterName == selectedParameterName)
   //   ui->treeWidgetParameters->setCurrentItem(parametersItem);
     if(property->GetLanguageType() == typeid(bool))
     {
   //   parametersItem->setFlags(parametersItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable);

   //   const bool* val=reinterpret_cast<const bool*>(i->second.Property->GetMemoryArea());
      bool value(false);
      if(item->checkState(1) == Qt::Checked)
       value=true;
      else
       value=false;
      property->ReadFromMemory(&value);
     }
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

void UComponentsListWidget::componentSelectedFromScheme(QString name)
{
    QTreeWidgetItemIterator iterator(componentsTree);
    while(*iterator)
    {
        if((*iterator)->data(0, Qt::UserRole) == name)
        {
            componentsTree->setCurrentItem(*iterator);
            return;
        }
        ++iterator;
    }
}

void UComponentsListWidget::componentDoubleClickFromScheme(QString name)
{
    QTreeWidgetItemIterator iterator(componentsTree);
    while(*iterator)
    {
        if((*iterator)->data(0, Qt::UserRole) == name)
        {
            componentsTree->setCurrentItem(*iterator);
            (*iterator)->setExpanded(true);
            currentDrawComponentName = (*iterator)->data(0, Qt::UserRole).toString();
            return;
        }
        ++iterator;
    }
}

void UComponentsListWidget::componentStapBackFromScheme()
{
    QStringList list = currentDrawComponentName.split(".");
    QString leaveComponent = list.last();
    list.pop_back();
    currentDrawComponentName = list.join(".");
    componentSelectedFromScheme(leaveComponent);
}

void UComponentsListWidget::channelsListSelectionChanged()
{
  if(ui->listWidgetChannelSelection->currentItem())
  {
    currentChannel = ui->listWidgetChannelSelection->currentItem()->data(Qt::UserRole).toInt();
  }
}

void UComponentsListWidget::drawSelectedComponent(QModelIndex index)
{
    currentDrawComponentName = index.data(Qt::UserRole).toString();
    emit componentDoubleClick(currentDrawComponentName);
    if(componentsTree->currentItem())
        componentsTree->currentItem()->setExpanded(true);
}


void UComponentsListWidget::componentMoveUp()
{
    if(componentsTree->currentItem())
    {
        MModel_ChangeComponentPosition(getWorkChannelIndex(), selectedComponentLongName.toLocal8Bit(),-1);
        UpdateInterface(true);
    }
}

void UComponentsListWidget::componentMoveDown()
{
    if(componentsTree->currentItem())
    {
        MModel_ChangeComponentPosition(getWorkChannelIndex(), selectedComponentLongName.toLocal8Bit(), 1);
        UpdateInterface(true);
    }
}

void UComponentsListWidget::componentRename()
{
    if(componentsTree->currentItem())
    {
        bool ok;
        QString restoreName = componentsTree->currentItem()->data(0, Qt::DisplayRole).toString();
        QString text = QInputDialog::getText(this, tr("Rename component"),
                                             tr("Enter new component name: "), QLineEdit::Normal,
                                             restoreName, &ok);
        if (ok && !text.isEmpty())
        {
            std::string new_name(text.toLocal8Bit());
            MModel_SetComponentPropertyData(getWorkChannelIndex(), selectedComponentLongName.toLocal8Bit(),"Name", &new_name);

            QStringList nameSeparator = selectedComponentLongName.split(".");
            nameSeparator.pop_back();
            nameSeparator.push_back(text);
            selectedComponentLongName = nameSeparator.join(".");
            emit updateScheme(true);
            UpdateInterface(true);
        }
    }
}

void UComponentsListWidget::componentDelete()
{
    if(componentsTree->currentItem())
    {
        if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
        {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Are you sure you want to delete component "+selectedComponentLongName+"?", QMessageBox::Yes|QMessageBox::Cancel);
            if (reply == QMessageBox::Cancel) return;
        }

        MModel_DelComponent(getWorkChannelIndex(), "", selectedComponentLongName.toLocal8Bit());
        UpdateInterface(true);
        emit updateScheme(true);
    }
}

void UComponentsListWidget::componentCopyNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    QStringList list = selectedComponentLongName.split(".");
    QString name = list.last();
    clipboard->setText(name);
}

void UComponentsListWidget::componentCopyLongNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(selectedComponentLongName);
}

void UComponentsListWidget::componentCopyClassNameToClipboard()
{
    const char *className=MModel_GetComponentClassName(getWorkChannelIndex(), selectedComponentLongName.toLocal8Bit());
    if(className)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString(className));
    }
    Engine_FreeBufString(className);
}

void UComponentsListWidget::componentReset()
{
    if(componentsTree->currentItem())
    {
        Env_Reset(selectedComponentLongName.toLocal8Bit());
        UpdateInterface(true);
    }
}

void UComponentsListWidget::componentCalculate()
{
    if(componentsTree->currentItem())
    {
        Env_Calculate(selectedComponentLongName.toLocal8Bit());
        RDK::UIVisualControllerStorage::UpdateInterface();
    }
}

void UComponentsListWidget::componentInit()
{
 if(componentsTree->currentItem())
 {
     Env_ModelInit(selectedComponentLongName.toLocal8Bit());
     RDK::UIVisualControllerStorage::UpdateInterface();
 }
}

void UComponentsListWidget::componentUnInit()
{
 if(componentsTree->currentItem())
 {
     Env_ModelUnInit(selectedComponentLongName.toLocal8Bit());
     RDK::UIVisualControllerStorage::UpdateInterface();
 }
}

void UComponentsListWidget::componentGUI()
{
  qDebug() << "component GUI";
}

void UComponentsListWidget::setUpdateInterval(long value)
{
  UpdateInterval = value;
}

void UComponentsListWidget::addComponentSons(QString componentName, QTreeWidgetItem *treeWidgetFather, QString oldRootItem, QString oldSelectedItem)
{
 RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock<RDK::UEngine>(getWorkChannelIndex());
    const char * stringBuff = MModel_GetComponentsNameList(getWorkChannelIndex(), componentName.toLocal8Bit());
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    QString str;
    if(!componentNames.empty()&&componentNames[0]!="")
    {
        QString father;
        if(treeWidgetFather) treeWidgetFather->setExpanded(false);
        if(!componentName.isEmpty()) father = componentName + ".";
        foreach(str, componentNames)
        {
            QTreeWidgetItem* childItem = new QTreeWidgetItem(treeWidgetFather);
            childItem->setText(0, str);
            childItem->setData(0, Qt::UserRole, father+str);
            if(oldRootItem == father+str)
            {
                componentsTree->setCurrentItem(childItem);
                childItem->setExpanded(false);
            }
            if(oldSelectedItem == father+str)
            {
                componentsTree->setCurrentItem(childItem);
                childItem->setExpanded(false);
            }

            addComponentSons(father+str, childItem, oldRootItem, oldSelectedItem);
        }
    }
}

void UComponentsListWidget::redrawChannelsList()
{
  ui->listWidgetChannelSelection->clear();
  int channelsCounter = Core_GetNumChannels();
  for(int i = 0; i < channelsCounter; i++)
  {
    QListWidgetItem *item = new QListWidgetItem(ui->listWidgetChannelSelection);
    item->setText(QString::number(i) + " ch.");
    item->setData(Qt::UserRole, i);
    if(i == currentChannel)
      ui->listWidgetChannelSelection->setCurrentItem(item);
  }
}

/// ”дал€ет из переданных данных лидирующие переводы строк
std::string& UComponentsListWidget::EraseLeadEndls(std::string &value)
{
 std::string::size_type data_i=value.find_first_of("\n");
 if(data_i != std::string::npos)
 {
  value.erase(value.begin(),value.begin()+data_i+1);
 }
 return value;
}

/// ”дал€ет из переданных данных лидирующие и завершающие переводы строк
std::string& UComponentsListWidget::EraseRangeEndls(std::string &value)
{
 std::string::size_type data_i=value.find_first_of("\n");
 if(data_i != std::string::npos && data_i<2)
 {
  value.erase(value.begin(),value.begin()+data_i+1);
 }

 data_i=value.find_last_of("\r\n");
 if(data_i != std::string::npos && data_i>=value.size()-1)
 {
  value.erase(value.begin()+data_i,value.end());
 }
 return value;
}

/// ≈сли в переданных данных есть хот€ бы один перевод строки, то замен€ет текст
/// на "[SEE BELOW]"
std::string& UComponentsListWidget::PreparePropertyValueToListView(std::string &value)
{
 EraseRangeEndls(value);
 if(value.empty())
 {
  value="[EMPTY]";
 }
 else
 {
  if(value.find_first_of("\n") != std::string::npos)
   value="[SEE BELOW]";
 }
 return value;
}


void UComponentsListWidget::propertyCopyNameToClipboard()
{
 QClipboard *clipboard = QApplication::clipboard();

 QString value;
 switch(ui->tabWidgetComponentInfo->currentIndex())
 {
   case 0:
     if(ui->treeWidgetParameters->currentItem())
       value=ui->treeWidgetParameters->currentItem()->data(0, Qt::DisplayRole).toString();
   break;

   case 1:
     if(ui->treeWidgetState->currentItem())
       value=ui->treeWidgetState->currentItem()->data(0, Qt::DisplayRole).toString();
   break;

   case 2:
     if(ui->treeWidgetInputs->currentItem())
       value=ui->treeWidgetInputs->currentItem()->data(0, Qt::DisplayRole).toString();
   break;

   case 3:
     if(ui->treeWidgetOutputs->currentItem())
       value=ui->treeWidgetOutputs->currentItem()->data(0, Qt::DisplayRole).toString();
   break;
 }

 clipboard->setText(value);
}

void UComponentsListWidget::propertyCopyValueToClipboard()
{
 QClipboard *clipboard = QApplication::clipboard();

 QString value;
 switch(ui->tabWidgetComponentInfo->currentIndex())
 {
   case 0:
     if(ui->treeWidgetParameters->currentItem())
       value=ui->treeWidgetParameters->currentItem()->data(1, Qt::DisplayRole).toString();
   break;

   case 1:
     if(ui->treeWidgetState->currentItem())
       value=ui->treeWidgetState->currentItem()->data(1, Qt::DisplayRole).toString();
   break;

   case 2:
     if(ui->treeWidgetInputs->currentItem())
       value=ui->treeWidgetInputs->currentItem()->data(1, Qt::DisplayRole).toString();
   break;

   case 3:
     if(ui->treeWidgetOutputs->currentItem())
       value=ui->treeWidgetOutputs->currentItem()->data(1, Qt::DisplayRole).toString();
   break;
 }

 clipboard->setText(value);
}

void UComponentsListWidget::propertyPasteValueFromClipboard()
{
 QClipboard *clipboard = QApplication::clipboard();

 QString value=clipboard->text();
 switch(ui->tabWidgetComponentInfo->currentIndex())
 {
   case 0:
     if(ui->treeWidgetParameters->currentItem())
     {
      ui->treeWidgetParameters->currentItem()->setData(1, Qt::UserRole, value);
      std::string temp=value.toLocal8Bit().constData();
      ui->treeWidgetParameters->currentItem()->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(temp).c_str()));
     }
   break;

   case 1:
     if(ui->treeWidgetState->currentItem())
     {
      ui->treeWidgetState->currentItem()->setData(1, Qt::UserRole, value);
      std::string temp=value.toLocal8Bit().constData();
      ui->treeWidgetState->currentItem()->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(temp).c_str()));
     }
   break;

   case 2:
     if(ui->treeWidgetInputs->currentItem())
     {
      ui->treeWidgetInputs->currentItem()->setData(1, Qt::UserRole, value);
      std::string temp=value.toLocal8Bit().constData();
      ui->treeWidgetInputs->currentItem()->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(temp).c_str()));
     }
   break;

   case 3:
     if(ui->treeWidgetOutputs->currentItem())
     {
      ui->treeWidgetOutputs->currentItem()->setData(1, Qt::UserRole, value);
      std::string temp=value.toLocal8Bit().constData();
      ui->treeWidgetOutputs->currentItem()->setText(1, QString::fromLocal8Bit(PreparePropertyValueToListView(temp).c_str()));
     }
   break;
 }
}


void UComponentsListWidget::on_actionReloadTree_triggered()
{
    UpdateInterface(true);
}

