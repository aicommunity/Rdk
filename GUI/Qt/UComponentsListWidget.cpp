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

UComponentsListWidget::UComponentsListWidget(QWidget *parent, QString settingsFile, QString settingsGroup) :
    UVisualControllerWidget(parent),
    ui(new Ui::UComponentsListWidget)
{
    propertyXML = NULL;
    ui->setupUi(this);

    componentsTree = new UComponentListTreeWidget(this);
    ui->verticalLayoutTreeWidget->setMargin(0);
    ui->verticalLayoutTreeWidget->addWidget(componentsTree);
    connect(componentsTree, SIGNAL(moveComponentUp()), this, SLOT(componentMoveUp()));
    connect(componentsTree, SIGNAL(moveComponentDown()), this, SLOT(componentMoveDown()));

    UpdateInterval = 500; //не обновлять виджет по тикам ядра
    setAccessibleName("UComponentsListWidget"); // имя класса для сериализации
    readSettings(settingsFile, settingsGroup);
    selectedComponentLongName = ""; //нулевая инициализация (чтобы было)
    currentDrawComponentName = "";
    UpdateInterface(true);

    ui->splitter_2->setStretchFactor(0, 10);
    ui->splitter_2->setStretchFactor(1, 1);

    ui->splitter_3->setStretchFactor(0, 10);
    ui->splitter_3->setStretchFactor(1, 1);

    ui->splitter_4->setStretchFactor(0, 10);
    ui->splitter_4->setStretchFactor(1, 1);

    ui->splitter_5->setStretchFactor(0, 10);
    ui->splitter_5->setStretchFactor(1, 1);

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


    //кнопки управления для фиксации параметров компонента
    //Parameters tab
    QMenu *setParametersMenu = new QMenu(ui->tabParameters);
    setParametersMenu->addAction(ui->actionSetGlobalParameters);
    setParametersMenu->addAction(ui->actionSetGlobalOwnerParameters);
    QToolButton *setParameters = new QToolButton(ui->toolBarParameters);
    setParameters->setText("Set");
    setParameters->setMenu(setParametersMenu);
    setParameters->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolBarParameters->addWidget(setParameters);
    ui->toolBarParameters->addAction(ui->actionReloadParameters);
    ui->toolBarParameters->addAction(ui->actionDefaultAllParameters);
    ui->toolBarParameters->addAction(ui->actionShowParametersXML);
    connect(setParameters, SIGNAL(clicked()), this, SLOT(setParametersClicked()));
    connect(ui->actionReloadParameters, SIGNAL(triggered()), this, SLOT(reloadPropertys()));
    connect(ui->actionSetGlobalParameters, SIGNAL(triggered()), this, SLOT(setGlobalParametersClicked()));
    connect(ui->actionSetGlobalOwnerParameters, SIGNAL(triggered()), this, SLOT(setGlobalOwnerParametersClicked()));
    connect(ui->actionDefaultAllParameters, SIGNAL(triggered()), this, SLOT(defaultAllParametersClicked()));
    connect(ui->actionShowParametersXML, SIGNAL(triggered()), this, SLOT(showParametersXMLClicked()));

    //State tab
    QMenu *setStateMenu = new QMenu(ui->tabState);
    setStateMenu->addAction(ui->actionSetGlobalState);
    setStateMenu->addAction(ui->actionSetGlobalOwnerState);
    QToolButton *setState = new QToolButton(ui->toolBarState);
    setState->setText("Set");
    setState->setMenu(setStateMenu);
    setState->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolBarState->addWidget(setState);
    ui->toolBarState->addAction(ui->actionReloadState);
    ui->toolBarState->addAction(ui->actionDefaultAllState);
    ui->toolBarState->addAction(ui->actionShowStateXML);
    connect(setState, SIGNAL(clicked()), this, SLOT(setStateClicked()));
    connect(ui->actionReloadState, SIGNAL(triggered()), this, SLOT(reloadPropertys()));
    connect(ui->actionSetGlobalState, SIGNAL(triggered()), this, SLOT(setGlobalStateClicked()));
    connect(ui->actionSetGlobalOwnerState, SIGNAL(triggered()), this, SLOT(setGlobalOwnerStateClicked()));
    connect(ui->actionDefaultAllState, SIGNAL(triggered()), this, SLOT(defaultAllParametersClicked()));
    connect(ui->actionShowStateXML, SIGNAL(triggered()), this, SLOT(showStateXMLClicked()));

    //Inputs tab
    QMenu *setInputsMenu = new QMenu(ui->tabInputs);
    setInputsMenu->addAction(ui->actionSetGlobalInputs);
    setInputsMenu->addAction(ui->actionSetGlobalOwnerInputs);
    QToolButton *setInputs = new QToolButton(ui->toolBarInputs);
    setInputs->setText("Set");
    setInputs->setMenu(setInputsMenu);
    setInputs->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolBarInputs->addWidget(setInputs);
    ui->toolBarInputs->addAction(ui->actionReloadInputs);
    ui->toolBarInputs->addAction(ui->actionDefaultAllInputs);
    ui->toolBarInputs->addAction(ui->actionShowInputsXML);
    connect(setInputs, SIGNAL(clicked()), this, SLOT(setInputsClicked()));
    connect(ui->actionReloadInputs, SIGNAL(triggered()), this, SLOT(reloadPropertys()));
    connect(ui->actionSetGlobalInputs, SIGNAL(triggered()), this, SLOT(setGlobalInputsClicked()));
    connect(ui->actionSetGlobalOwnerInputs, SIGNAL(triggered()), this, SLOT(setGlobalOwnerInputsClicked()));
    connect(ui->actionDefaultAllInputs, SIGNAL(triggered()), this, SLOT(defaultAllParametersClicked()));
    connect(ui->actionShowInputsXML, SIGNAL(triggered()), this, SLOT(showInputsXMLClicked()));

    //Outputs tab
    QMenu *setOutputsMenu = new QMenu(ui->tabOutputs);
    setOutputsMenu->addAction(ui->actionSetGlobalOutputs);
    setOutputsMenu->addAction(ui->actionSetGlobalOwnerOutputs);
    QToolButton *setOutputs = new QToolButton(ui->toolBarOutputs);
    setOutputs->setText("Set");
    setOutputs->setMenu(setOutputsMenu);
    setOutputs->setPopupMode(QToolButton::MenuButtonPopup);
    ui->toolBarOutputs->addWidget(setOutputs);
    ui->toolBarOutputs->addAction(ui->actionReloadOutputs);
    ui->toolBarOutputs->addAction(ui->actionDefaultAllOutputs);
    ui->toolBarOutputs->addAction(ui->actionShowOutputsXML);
    connect(setOutputs, SIGNAL(clicked()), this, SLOT(setOutputsClicked()));
    connect(ui->actionReloadOutputs, SIGNAL(triggered()), this, SLOT(reloadPropertys()));
    connect(ui->actionSetGlobalOutputs, SIGNAL(triggered()), this, SLOT(setGlobalOutputsClicked()));
    connect(ui->actionSetGlobalOwnerOutputs, SIGNAL(triggered()), this, SLOT(setGlobalOwnerOutputsClicked()));
    connect(ui->actionDefaultAllOutputs, SIGNAL(triggered()), this, SLOT(defaultAllParametersClicked()));
    connect(ui->actionShowOutputsXML, SIGNAL(triggered()), this, SLOT(showOutputsXMLClicked()));
}

UComponentsListWidget::~UComponentsListWidget()
{
    writeSettings(settingsFileName, settingsGroupName);
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
    componentsTree->clear();

    //Заполнение списка
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(componentsTree);
    rootItem->setText(0, "Model");
    rootItem->setExpanded(true);
    addComponentSons("", rootItem, oldRootItem, oldSelectedItem);
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
    if(!componentsTree->currentItem()) return "";
    else return componentsTree->currentItem()->data(0, Qt::UserRole).toString();
}

QString UComponentsListWidget::getSelectedParameterName()
{
    if(!ui->treeWidgetParameters->currentItem()) return "";
    else return ui->treeWidgetParameters->currentItem()->data(0, Qt::DisplayRole).toString();
}

QString UComponentsListWidget::getSelectedStateName()
{
    if(!ui->treeWidgetState->currentItem()) return "";
    else return ui->treeWidgetState->currentItem()->data(0, Qt::DisplayRole).toString();
}

QString UComponentsListWidget::getSelectedInputName()
{
    if(!ui->treeWidgetInputs->currentItem()) return "";
    else return ui->treeWidgetInputs->currentItem()->data(0, Qt::DisplayRole).toString();
}

QString UComponentsListWidget::getSelectedOutputName()
{
    if(!ui->treeWidgetOutputs->currentItem()) return "";
    else return ui->treeWidgetOutputs->currentItem()->data(0, Qt::DisplayRole).toString();
}

/*void UComponentsListWidget::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "key pressed";
    if(1)//QApplication::keyboardModifiers() == Qt::ShiftModifier)
    {
        switch(event->key())
        {
        case Qt::Key_Up:
            qDebug() << "up";//componentMoveUp();
            break;

        case Qt::Key_Down:
            qDebug() << "down";//componentMoveDown();
            break;

        default:
            QWidget::keyPressEvent(event);
            break;
        }
    }
    else
        QWidget::keyPressEvent(event);
}*/

void UComponentsListWidget::openTabN(int n)
{
    if(n>0 && n <4)
        ui->tabWidgetComponentInfo->setCurrentIndex(n);
}

void UComponentsListWidget::componentListItemSelectionChanged()
{
    QTreeWidgetItem * item = componentsTree->currentItem();
    if(!item) return;
    
    selectedComponentLongName = item->data(0,Qt::UserRole).toString();

    reloadPropertys();

    /*//Class
    const char *className=Model_GetComponentClassName(selectedComponentLongName.toLocal8Bit());
    if(className)
        ui->labelComponentClassName->setText(className);
    Engine_FreeBufString(className);

    ui->treeWidgetParameters->clear();
    ui->treeWidgetState->clear();
    ui->treeWidgetInputs->clear();
    ui->treeWidgetOutputs->clear();
    try
    {
        RDK::UELockPtr<RDK::UContainer> model = GetModelLock(Core_GetSelectedChannelIndex());

        RDK::UEPtr<RDK::UContainer> cont;
        if (selectedComponentLongName.isEmpty())
            cont = model;
        else
            cont = model->GetComponentL(selectedComponentLongName.toLocal8Bit().constData(), true);

        if(!cont) return;
        RDK::UComponent::VariableMapT varMap = cont->GetPropertiesList();
        std::string buffer;
        for(std::map<RDK::NameT,RDK::UVariable>::iterator i = varMap.begin(); i != varMap.end(); ++i)
        {
            if (i->second.CheckMask(ptPubParameter))
            {
                QTreeWidgetItem* parametersItem = new QTreeWidgetItem(ui->treeWidgetParameters);
                parametersItem->setText(0, QString::fromStdString(i->first));
                parametersItem->setText(1, QString::fromStdString(cont->GetPropertyValue(i->first, buffer)));
            }
            if (i->second.CheckMask(ptPubState))
            {
                QTreeWidgetItem* stateItem = new QTreeWidgetItem(ui->treeWidgetState);
                stateItem->setText(0, QString::fromStdString(i->first));
                stateItem->setText(1, QString::fromStdString(cont->GetPropertyValue(i->first, buffer)));
            }
            if (i->second.CheckMask(ptPubInput))
            {
                QTreeWidgetItem* inputItem = new QTreeWidgetItem(ui->treeWidgetInputs);
                inputItem->setText(0, QString::fromStdString(i->first));
                //inputItem->setText(1, QString::fromStdString(cont->GetPropertyValue(i->first, buffer)));
                inputItem->setText(2, QString(i->second.Property->GetLanguageType().name()));
                //if(i->second.Property->GetIoType() & ipRange) inputItem->setText(3, QString("range"));
                //else
                    //if(i->second.Property->IsConnected()) inputItem->setText(3, QString("connected"));
            }
            if (i->second.CheckMask(ptPubOutput))
            {
                QTreeWidgetItem* outputItem = new QTreeWidgetItem(ui->treeWidgetOutputs);
                outputItem->setText(0, QString::fromStdString(i->first));
                outputItem->setText(1, QString::fromStdString(cont->GetPropertyValue(i->first, buffer)));
                outputItem->setText(2, QString(i->second.Property->GetLanguageType().name()));
            }
        }
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }*/

    /*
    //Parameters
    const char * stringBuff = Model_GetComponentProperties(selectedComponentLongName.toLocal8Bit(), ptPubParameter);
    ui->plainTextEditParameters->clear();
    ui->plainTextEditParameters->insertPlainText(stringBuff);
    Engine_FreeBufString(stringBuff);

    //State
    stringBuff = Model_GetComponentProperties(selectedComponentLongName.toLocal8Bit(), ptPubState);
    ui->plainTextEditState->clear();
    ui->plainTextEditState->insertPlainText(stringBuff);
    Engine_FreeBufString(stringBuff);

    //Inputs
    stringBuff = Model_GetComponentProperties(selectedComponentLongName.toLocal8Bit(), ptPubInput);
    ui->plainTextEditInputs->clear();
    ui->plainTextEditInputs->insertPlainText(stringBuff);
    Engine_FreeBufString(stringBuff);

    //Outputs
    stringBuff = Model_GetComponentProperties(selectedComponentLongName.toLocal8Bit(), ptPubOutput);
    ui->plainTextEditOutputs->clear();
    ui->plainTextEditOutputs->insertPlainText(stringBuff);
    Engine_FreeBufString(stringBuff);
    */

    //говорим схеме выделить елемент
    emit componentSelected(selectedComponentLongName);
}

void UComponentsListWidget::reloadPropertys(bool forceReload)
{
    if(currentDrawPropertyComponentName == selectedComponentLongName && !forceReload) return;


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
    try
    {
        RDK::UELockPtr<RDK::UContainer> model = GetModelLock(Core_GetSelectedChannelIndex());

        RDK::UEPtr<RDK::UContainer> cont;
        if (currentDrawPropertyComponentName.isEmpty())
            cont = model;
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
    if(!item) return;

    QString changedParameterName = item->data(0, Qt::DisplayRole).toString();
    if(selectedParameterName == changedParameterName) return;

    selectedParameterName = changedParameterName;
    ui->plainTextEditParameters->clear();
    ui->plainTextEditParameters->setPlainText(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidget::stateListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetState->currentItem();
    if(!item) return;

    QString changedStateName = item->data(0, Qt::DisplayRole).toString();
    if(selectedStateName == changedStateName) return;

    selectedStateName = changedStateName;
    ui->plainTextEditState->clear();
    ui->plainTextEditState->setPlainText(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidget::inputsListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetInputs->currentItem();
    if(!item) return;

    QString changedInputName = item->data(0, Qt::DisplayRole).toString();
    if(selectedInputName == changedInputName) return;

    selectedInputName = changedInputName;
    ui->plainTextEditInputs->clear();
    ui->plainTextEditInputs->setPlainText(item->data(1, Qt::UserRole).toString());
}

void UComponentsListWidget::outputsListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetOutputs->currentItem();
    if(!item) return;

    QString changedOutputName = item->data(0, Qt::DisplayRole).toString();
    if(selectedOutputName == changedOutputName) return;

    selectedOutputName = changedOutputName;
    ui->plainTextEditOutputs->clear();
    ui->plainTextEditOutputs->setPlainText(item->data(1, Qt::UserRole).toString());
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

void UComponentsListWidget::setParametersClicked()
{
    /*Model_SetComponentParameters(selectedComponentLongName.toLocal8Bit(),
                                 ui->plainTextEditParameters->toPlainText().toLocal8Bit());*/
    QTreeWidgetItem * item = ui->treeWidgetParameters->currentItem();
    if(item)
    {
        Model_SetComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditParameters->toPlainText().toLocal8Bit());
        reloadPropertys();
        ui->plainTextEditParameters->clear();
    }
}

void UComponentsListWidget::setGlobalParametersClicked()
{
    QTreeWidgetItem * item = ui->treeWidgetParameters->currentItem();
    if(item)
    {
        Model_SetGlobalComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    ui->labelComponentClassName->text().toLocal8Bit(),
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditParameters->toPlainText().toLocal8Bit());
        reloadPropertys();
        ui->plainTextEditParameters->clear();
    }
}

void UComponentsListWidget::setGlobalOwnerParametersClicked()
{
    QTreeWidgetItem * item = ui->treeWidgetParameters->currentItem();
    if(item)
    {
        Model_SetGlobalOwnerComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    ui->labelComponentClassName->text().toLocal8Bit(), //тут вопрос о классах которым ставится значение
                    Model_GetComponentClassName(currentDrawComponentName.toLocal8Bit()), //тут вопрос о владельце
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditParameters->toPlainText().toLocal8Bit());
        reloadPropertys();
        ui->plainTextEditParameters->clear();
    }
}

void UComponentsListWidget::defaultAllParametersClicked()
{
    Env_Default(selectedComponentLongName.toLocal8Bit(), false);
    //Env_Reset(selectedComponentLongName.toLocal8Bit()); //тут вопрос о сбросе вложенных компонент (как в билдере)
    reloadPropertys();
}

void UComponentsListWidget::showParametersXMLClicked()
{
    if(!propertyXML) propertyXML = new UPropertyXMLWidget(this);
    propertyXML->initWidget(selectedComponentLongName, ptPubParameter);
    propertyXML->show();
}

void UComponentsListWidget::setStateClicked()
{
    /*Model_SetComponentState(selectedComponentLongName.toLocal8Bit(),
                            ui->plainTextEditState->toPlainText().toLocal8Bit());*/
    QTreeWidgetItem * item = ui->treeWidgetState->currentItem();
    if(item)
    {
        Model_SetComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditState->toPlainText().toLocal8Bit());
        reloadPropertys();
    }
}

void UComponentsListWidget::setGlobalStateClicked()
{
    QTreeWidgetItem * item = ui->treeWidgetState->currentItem();
    if(item)
    {
        Model_SetGlobalComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    ui->labelComponentClassName->text().toLocal8Bit(),
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditState->toPlainText().toLocal8Bit());
        reloadPropertys();
    }
}

void UComponentsListWidget::setGlobalOwnerStateClicked()
{
    QTreeWidgetItem * item = ui->treeWidgetState->currentItem();
    if(item)
    {
        Model_SetGlobalOwnerComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    ui->labelComponentClassName->text().toLocal8Bit(), //тут вопрос о классах которым ставится значение
                    Model_GetComponentClassName(currentDrawComponentName.toLocal8Bit()), //тут вопрос о владельце
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditState->toPlainText().toLocal8Bit());
        reloadPropertys();
    }
}

void UComponentsListWidget::showStateXMLClicked()
{
    if(!propertyXML) propertyXML = new UPropertyXMLWidget(this);
    propertyXML->initWidget(selectedComponentLongName, ptPubState);
    propertyXML->show();
}

void UComponentsListWidget::setInputsClicked()
{
    QTreeWidgetItem * item = ui->treeWidgetInputs->currentItem();
    if(item)
    {
        Model_SetComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditInputs->toPlainText().toLocal8Bit());
        reloadPropertys();
    }
}

void UComponentsListWidget::setGlobalInputsClicked()
{
    QTreeWidgetItem * item = ui->treeWidgetInputs->currentItem();
    if(item)
    {
        Model_SetGlobalComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    ui->labelComponentClassName->text().toLocal8Bit(),
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditInputs->toPlainText().toLocal8Bit());
        reloadPropertys();
    }
}

void UComponentsListWidget::setGlobalOwnerInputsClicked()
{
    QTreeWidgetItem * item = ui->treeWidgetInputs->currentItem();
    if(item)
    {
        Model_SetGlobalOwnerComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    ui->labelComponentClassName->text().toLocal8Bit(), //тут вопрос о классах которым ставится значение
                    Model_GetComponentClassName(currentDrawComponentName.toLocal8Bit()), //тут вопрос о владельце
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditInputs->toPlainText().toLocal8Bit());
        reloadPropertys();
    }
}

void UComponentsListWidget::showInputsXMLClicked()
{
    if(!propertyXML) propertyXML = new UPropertyXMLWidget(this);
    propertyXML->initWidget(selectedComponentLongName, ptPubInput);
    propertyXML->show();
}

void UComponentsListWidget::setOutputsClicked()
{
    QTreeWidgetItem * item = ui->treeWidgetOutputs->currentItem();
    if(item)
    {
        Model_SetComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditOutputs->toPlainText().toLocal8Bit());
        reloadPropertys();
    }
}

void UComponentsListWidget::setGlobalOutputsClicked()
{
    QTreeWidgetItem * item = ui->treeWidgetOutputs->currentItem();
    if(item)
    {
        Model_SetGlobalComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    ui->labelComponentClassName->text().toLocal8Bit(),
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditOutputs->toPlainText().toLocal8Bit());
        reloadPropertys();
    }
}

void UComponentsListWidget::setGlobalOwnerOutputsClicked()
{
    QTreeWidgetItem * item = ui->treeWidgetOutputs->currentItem();
    if(item)
    {
        Model_SetGlobalOwnerComponentPropertyValue(
                    selectedComponentLongName.toLocal8Bit(),
                    ui->labelComponentClassName->text().toLocal8Bit(), //тут вопрос о классах которым ставится значение
                    Model_GetComponentClassName(currentDrawComponentName.toLocal8Bit()), //тут вопрос о владельце
                    item->data(0, Qt::DisplayRole).toString().toLocal8Bit(),
                    ui->plainTextEditOutputs->toPlainText().toLocal8Bit());
        reloadPropertys();
    }
}

void UComponentsListWidget::showOutputsXMLClicked()
{
    if(!propertyXML) propertyXML = new UPropertyXMLWidget(this);
    propertyXML->initWidget(selectedComponentLongName, ptPubOutput);
    propertyXML->show();
}

void UComponentsListWidget::readSettings(QString file, QString group)
{
    settingsFileName = file;
    settingsGroupName = group;
    QSettings settings(settingsFileName, QSettings::IniFormat);
    settings.beginGroup(settingsGroupName);
    ui->splitter->restoreState(settings.value("splitterState").toByteArray());
    ui->splitter_2->restoreState(settings.value("splitterState_2").toByteArray());
    ui->splitter_3->restoreState(settings.value("splitterState_3").toByteArray());
    ui->splitter_4->restoreState(settings.value("splitterState_4").toByteArray());
    ui->splitter_5->restoreState(settings.value("splitterState_5").toByteArray());
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
    settings.setValue("splitterState_2", ui->splitter_2->saveState());
    settings.setValue("splitterState_3", ui->splitter_3->saveState());
    settings.setValue("splitterState_4", ui->splitter_4->saveState());
    settings.setValue("splitterState_5", ui->splitter_5->saveState());
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
                componentsTree->setCurrentItem(childItem);

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


