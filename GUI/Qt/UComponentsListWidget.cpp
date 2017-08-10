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

UComponentsListWidget::UComponentsListWidget(QWidget *parent, QString settingsFile, QString settingsGroup) :
    UVisualControllerWidget(parent),
    ui(new Ui::UComponentsListWidget)
{
    ui->setupUi(this);

    componentsTree = new UComponentListTreeWidget(this);
    ui->verticalLayoutTreeWidget->setMargin(0);
    ui->verticalLayoutTreeWidget->addWidget(componentsTree);
    connect(componentsTree, SIGNAL(moveComponentUp()), this, SLOT(componentMoveUp()));
    connect(componentsTree, SIGNAL(moveComponentDown()), this, SLOT(componentMoveDown()));

    UpdateInterval = 500;
    setAccessibleName("UComponentsListWidget"); // имя класса для сериализации
    readSettings(settingsFile, settingsGroup);

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

    //контекстное меню для дерева компонентов
    QAction *actionSeparator1 = new QAction(this);
    actionSeparator1->setSeparator(true);
    QAction *actionSeparator2 = new QAction(this);
    actionSeparator2->setSeparator(true);
    QAction *actionSeparator3 = new QAction(this);
    actionSeparator3->setSeparator(true);
    QAction *actionSeparator4 = new QAction(this);
    actionSeparator4->setSeparator(true);
    componentsTree->addAction(ui->actionComponentMoveUp);
    componentsTree->addAction(ui->actionComponentMoveDown);
    componentsTree->addAction(actionSeparator1);
    componentsTree->addAction(ui->actionComponentRename);
    componentsTree->addAction(ui->actionComponentDelete);
    componentsTree->addAction(actionSeparator2);
    componentsTree->addAction(ui->actionComponentReset);
    componentsTree->addAction(ui->actionComponentCalculate);
    componentsTree->addAction(actionSeparator3);
    componentsTree->addAction(ui->actionCopyNameToClipboard);
    componentsTree->addAction(ui->actionCopyLongNameToClipboard);
    componentsTree->addAction(ui->actionCopyClassNameToClipboard);
    componentsTree->addAction(actionSeparator4);
    componentsTree->addAction(ui->actionComponentGUI);
    ui->actionComponentGUI->setEnabled(false); // нет реализации, по этому ставим в enable
    connect(ui->actionComponentMoveUp, SIGNAL(triggered()), this, SLOT(componentMoveUp()));
    connect(ui->actionComponentMoveDown, SIGNAL(triggered()), this, SLOT(componentMoveDown()));
    connect(ui->actionComponentRename, SIGNAL(triggered()), this, SLOT(componentRename()));
    connect(ui->actionComponentDelete, SIGNAL(triggered()), this, SLOT(componentDelete()));
    connect(ui->actionComponentReset, SIGNAL(triggered()), this, SLOT(componentReset()));
    connect(ui->actionComponentCalculate, SIGNAL(triggered()), this, SLOT(componentCalculate()));
    connect(ui->actionCopyNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyNameToClipboard()));
    connect(ui->actionCopyLongNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyLongNameToClipboard()));
    connect(ui->actionCopyClassNameToClipboard, SIGNAL(triggered()), this, SLOT(componentCopyClassNameToClipboard()));
    connect(ui->actionComponentGUI, SIGNAL(triggered()), this, SLOT(componentGUI()));

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

    //Заполнение списка
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(componentsTree);
    rootItem->setText(0, "Model");
    rootItem->setExpanded(true);
    addComponentSons("", rootItem, oldRootItem, oldSelectedItem);

    // чтоб не бегали скролы на treeWidget'ах
    componentsTree->verticalScrollBar()->setMaximum(componentsListScrollMaximum);
    componentsTree->verticalScrollBar()->setValue(componentsListScrollPosition);
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
      return componentsTree->currentItem()->data(0, Qt::UserRole).toString();
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

    default:
      return "";
  }
}

void UComponentsListWidget::setEnableTabN(int n, bool enable)
{
    if(n > 0 && n < 4)
      ui->tabWidgetComponentInfo->setTabEnabled(n, enable);
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

    //Class
    const char *className=Model_GetComponentClassName(currentDrawPropertyComponentName.toLocal8Bit());
    if(className)
        ui->labelComponentClassName->setText(className);
    Engine_FreeBufString(className);

    ui->treeWidgetParameters->clear();
    ui->treeWidgetState->clear();
    ui->treeWidgetInputs->clear();
    ui->treeWidgetOutputs->clear();

    // чтоб не бегали скролы на treeWidget'ах
    int paramScrollPosition = ui->treeWidgetParameters->verticalScrollBar()->value(),
        stateScrollPosition = ui->treeWidgetState->verticalScrollBar()->value(),
        inputsScrollPosition = ui->treeWidgetInputs->verticalScrollBar()->value(),
        outputsScrollPosition = ui->treeWidgetOutputs->verticalScrollBar()->value();

    try
    {
        RDK::UELockPtr<RDK::UContainer> model = GetModelLock(Core_GetSelectedChannelIndex());

        RDK::UEPtr<RDK::UContainer> cont;
        if (currentDrawPropertyComponentName.isEmpty())
            cont = model.Get();
        else
            cont = model->GetComponentL(currentDrawPropertyComponentName.toLocal8Bit().constData(), true);

        if(!cont) return;
        RDK::UComponent::VariableMapT varMap = cont->GetPropertiesList();
        std::string buffer;
        for(std::map<RDK::NameT,RDK::UVariable>::iterator i = varMap.begin(); i != varMap.end(); ++i)
        {
            if (i->second.CheckMask(ptPubParameter))
            {
                QTreeWidgetItem* parametersItem = new QTreeWidgetItem(ui->treeWidgetParameters);
                QString parameterName = QString::fromStdString(i->first);
                parametersItem->setText(0, parameterName);
                cont->GetPropertyValue(i->first, buffer);
                parametersItem->setData(1, Qt::UserRole, QString::fromStdString(buffer));
                parametersItem->setText(1, QString::fromStdString(PreparePropertyValueToListView(buffer)));
                if(parameterName == selectedParameterName)
                    ui->treeWidgetParameters->setCurrentItem(parametersItem);
            }
            if (i->second.CheckMask(ptPubState))
            {
                QTreeWidgetItem* stateItem = new QTreeWidgetItem(ui->treeWidgetState);
                QString stateName = QString::fromStdString(i->first);
                stateItem->setText(0, stateName);
                cont->GetPropertyValue(i->first, buffer);
                stateItem->setData(1, Qt::UserRole, QString::fromStdString(buffer));
                stateItem->setText(1, QString::fromStdString(PreparePropertyValueToListView(buffer)));
                if(stateName == selectedStateName)
                    ui->treeWidgetState->setCurrentItem(stateItem);
            }
            if (i->second.CheckMask(ptPubInput))
            {
                QTreeWidgetItem* inputItem = new QTreeWidgetItem(ui->treeWidgetInputs);
                QString inputName = QString::fromStdString(i->first);
                inputItem->setText(0, inputName);
                //inputItem->setText(1, QString::fromStdString(cont->GetPropertyValue(i->first, buffer)));
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
                QString outputName = QString::fromStdString(i->first);
                outputItem->setText(0, outputName);
                cont->GetPropertyValue(i->first, buffer);
                outputItem->setData(1, Qt::UserRole, QString::fromStdString(buffer));
                outputItem->setText(1, QString::fromStdString(PreparePropertyValueToListView(buffer)));
                outputItem->setText(2, QString(i->second.Property->GetLanguageType().name()));
                if(outputName == selectedOutputName)
                    ui->treeWidgetOutputs->setCurrentItem(outputItem);
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

void UComponentsListWidget::drawSelectedComponent(QModelIndex index)
{
    currentDrawComponentName = index.data(Qt::UserRole).toString();
    emit componentDoubleClick(currentDrawComponentName);
    if(componentsTree->currentItem())
        componentsTree->currentItem()->setExpanded(true);
}

void UComponentsListWidget::readSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    ui->splitter->restoreState(settings.value("splitterState").toByteArray());
    ui->treeWidgetInputs->header()->restoreState(settings.value("treeWidgetInputsHeader").toByteArray());
    ui->treeWidgetOutputs->header()->restoreState(settings.value("treeWidgetOutputsHeader").toByteArray());
    ui->treeWidgetParameters->header()->restoreState(settings.value("treeWidgetParameters").toByteArray());
    ui->treeWidgetState->header()->restoreState(settings.value("treeWidgetState").toByteArray());
    settings.endGroup();
}

void UComponentsListWidget::writeSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue("splitterState", ui->splitter->saveState());
    settings.setValue("treeWidgetInputsHeader", ui->treeWidgetInputs->header()->saveState());
    settings.setValue("treeWidgetOutputsHeader", ui->treeWidgetOutputs->header()->saveState());
    settings.setValue("treeWidgetParameters", ui->treeWidgetParameters->header()->saveState());
    settings.setValue("treeWidgetState", ui->treeWidgetState->header()->saveState());
    settings.endGroup();
}

void UComponentsListWidget::componentMoveUp()
{
    if(componentsTree->currentItem())
    {
        Model_ChangeComponentPosition(selectedComponentLongName.toLocal8Bit(),-1);
        UpdateInterface(true);
    }
}

void UComponentsListWidget::componentMoveDown()
{
    if(componentsTree->currentItem())
    {
        Model_ChangeComponentPosition(selectedComponentLongName.toLocal8Bit(), 1);
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
            Model_SetComponentPropertyData(selectedComponentLongName.toLocal8Bit(),"Name", &new_name);

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

        Model_DelComponent("", selectedComponentLongName.toLocal8Bit());
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
    const char *className=Model_GetComponentClassName(selectedComponentLongName.toLocal8Bit());
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

void UComponentsListWidget::componentGUI()
{
    qDebug() << "component GUI";
}

void UComponentsListWidget::addComponentSons(QString componentName, QTreeWidgetItem *treeWidgetFather, QString oldRootItem, QString oldSelectedItem)
{
    const char * stringBuff = Model_GetComponentsNameList(componentName.toLocal8Bit());
    QStringList componentNames = QString(stringBuff).split(",");
    Engine_FreeBufString(stringBuff);
    QString str;
    if(!componentNames.empty()&&componentNames[0]!="")
    {
        QString father;
        if(!componentName.isEmpty()) father = componentName + ".";
        foreach(str, componentNames)
        {
            QTreeWidgetItem* childItem = new QTreeWidgetItem(treeWidgetFather);
            childItem->setText(0, str);
            childItem->setData(0, Qt::UserRole, father+str);
            if(oldRootItem == father+str)
            {
                componentsTree->setCurrentItem(childItem);
                childItem->setExpanded(true);
            }
            if(oldSelectedItem == father+str)
            {
                componentsTree->setCurrentItem(childItem);
                childItem->setExpanded(true);
            }

            addComponentSons(father+str, childItem, oldRootItem, oldSelectedItem);
        }
    }
}

/// Удаляет из переданных данных лидирующие переводы строк
std::string& UComponentsListWidget::EraseLeadEndls(std::string &value)
{
 std::string::size_type data_i=value.find_first_of("\n");
 if(data_i != std::string::npos)
 {
  value.erase(value.begin(),value.begin()+data_i+1);
 }
 return value;
}

/// Удаляет из переданных данных лидирующие и завершающие переводы строк
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

/// Если в переданных данных есть хотя бы один перевод строки, то заменяет текст
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


