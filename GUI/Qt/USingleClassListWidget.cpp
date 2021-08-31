#include "USingleClassListWidget.h"
#include "ui_USingleClassListWidget.h"

#include <QDebug>
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include <QList>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QScrollBar>

USingleClassListWidget::USingleClassListWidget(std::string class_name, QWidget *parent, RDK::UApplication *app) :
    ClassName(class_name),
    UVisualControllerWidget(parent, app),
    ui(new Ui::USingleClassListWidget)
{
    CheckModelFlag=false;
    ui->setupUi(this);

    componentsTree = new QTreeWidget(this);
    componentsTree->setHeaderHidden(true);

    //ui->verticalLayoutTreeWidget->setMargin(0);
    ui->verticalLayout->addWidget(componentsTree);
    connect(componentsTree, SIGNAL(moveComponentUp()), this, SLOT(componentMoveUp()));
    connect(componentsTree, SIGNAL(moveComponentDown()), this, SLOT(componentMoveDown()));

    UpdateInterval = -1;
    setAccessibleName("USingleClassListWidget"); // имя класса для сериализации
    //readSettings(app, settingsGroup);

    UpdateInterface(true);

    //выделение элемента списка
    connect(componentsTree, SIGNAL(itemSelectionChanged()),
            this, SLOT(componentListItemSelectionChanged()));

    //выделение элменетов propertys
    connect(ui->treeWidgetParameters, SIGNAL(itemSelectionChanged()),
            this, SLOT(parametersListSelectionChanged()));

    reloadClassTree();
}

USingleClassListWidget::~USingleClassListWidget()
{
    delete ui;
}

void USingleClassListWidget::ChangeClass(std::string class_name)
{
     auto storage = RDK::GetStorageLock();
     storage->FreeObjectsStorage();
     ClassName = class_name;

     ui->treeWidgetParameters->clear();
     componentsTree->clear();

     if(!ClassName.empty())
        reloadClassTree();
}

void USingleClassListWidget::reloadClassTree()
{
    // чтоб не бегали скролы на treeWidget'ах
    int componentsListScrollMaximum = componentsTree->verticalScrollBar()->maximum();
    int componentsListScrollPosition = componentsTree->verticalScrollBar()->value();

    componentsTree->clear();

    //Заполнение списка
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(componentsTree);

    rootItem->setText(0, QString::fromStdString(ClassName));
    rootItem->setData(0, Qt::UserRole, "{CompName}");
    rootItem->setData(1, Qt::UserRole, QString::fromStdString(ClassName));
    rootItem->setExpanded(false);

    auto storage = RDK::GetStorageLock();

    try
    {
        auto cont = RDK::dynamic_pointer_cast<RDK::UContainer>(storage->TakeObject(ClassName));

        addComponentSons(cont, "{CompName}", rootItem);

        storage->ReturnObject(cont);
    }
    catch(RDK::UException& e)
    {
        return;
    }
    componentsTree->setCurrentItem(rootItem);
    // чтоб не бегали скролы на treeWidget'ах
    componentsTree->verticalScrollBar()->setMaximum(componentsListScrollMaximum);
    componentsTree->verticalScrollBar()->setValue(componentsListScrollPosition);
}

void USingleClassListWidget::ASaveParameters()
{
    if(!application) return;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    settings.setValue("splitterState", ui->splitter->saveState());
    settings.setValue("treeWidgetParameters", ui->treeWidgetParameters->header()->saveState());

    settings.endGroup();
}

void USingleClassListWidget::ALoadParameters()
{
    if(!application) return;

    QSettings settings(QString::fromLocal8Bit(
                         application->GetProjectPath().c_str())+"settings.qt",
                       QSettings::IniFormat);
    settings.beginGroup(accessibleName());
    ui->splitter->restoreState(settings.value("splitterState").toByteArray());
    ui->treeWidgetParameters->header()->restoreState(settings.value("treeWidgetParameters").toByteArray());
    settings.endGroup();
}


QString USingleClassListWidget::getSelectedComponentLongName()
{
    if(!componentsTree->currentItem())
      return "";
    else
      return componentsTree->currentItem()->data(0, Qt::UserRole).toString();
}

void USingleClassListWidget::setEnableTabN(int n, bool enable)
{
    if(n >= 0 && n < 4)
      ui->tabWidgetComponentInfo->setTabEnabled(n, enable);
}

void USingleClassListWidget::componentListItemSelectionChanged()
{
    QTreeWidgetItem * item = componentsTree->currentItem();
    if(!item)
        return;
    
    selectedClass = item->data(1, Qt::UserRole).toString();
    selectedComponentLong = item->data(0, Qt::UserRole).toString();

    reloadPropertys();
}

void USingleClassListWidget::reloadPropertys()
{
    ui->treeWidgetParameters->clear();

    // чтоб не бегали скролы на treeWidget'ах
    int paramScrollPosition = ui->treeWidgetParameters->verticalScrollBar()->value();

    try
    {
        UpdateInterfaceFlag=true;

        auto storage = RDK::GetStorageLock();
        RDK::UEPtr<RDK::UContainer> cont = RDK::dynamic_pointer_cast<RDK::UContainer>(storage->TakeObject(selectedClass.toStdString()));

        if(!cont)
        {
            UpdateInterfaceFlag=false;
            return;
        }
        RDK::UComponent::VariableMapT varMap = cont->GetPropertiesList();
        std::string buffer;

        for(std::map<RDK::NameT,RDK::UVariable>::iterator i = varMap.begin(); i != varMap.end(); ++i)
        {
            if(i->second.CheckMask(ptPubParameter))
            {
                QTreeWidgetItem* parametersItem = new QTreeWidgetItem(ui->treeWidgetParameters);
                QString parameterName = QString::fromLocal8Bit(i->first.c_str());
                parametersItem->setText(0, parameterName);
            }
        }

        // чтоб не бегали скролы на treeWidget'ах
        ui->treeWidgetParameters->verticalScrollBar()->setMaximum(paramScrollPosition);
        ui->treeWidgetParameters->verticalScrollBar()->setValue(paramScrollPosition);
        UpdateInterfaceFlag=false;

        storage->ReturnObject(cont);
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

    if(ui->treeWidgetParameters->topLevelItemCount())
    {
        ui->treeWidgetParameters->setCurrentItem(
                ui->treeWidgetParameters->topLevelItem(0), 0, QItemSelectionModel::Select );
    }
}

void USingleClassListWidget::parametersListSelectionChanged()
{
    QTreeWidgetItem * item = ui->treeWidgetParameters->currentItem();

    if(!item)
        return;

    QString changedParameterName = item->data(0, Qt::DisplayRole).toString();

    selectedParameterName = changedParameterName;

    QString full_parameter_path = CalSelectedParameterPath();

    emit parameterChanged(full_parameter_path);
}

QString USingleClassListWidget::CalSelectedParameterPath()
{
    return selectedComponentLong + ":" + selectedParameterName;;
}

void USingleClassListWidget::setUpdateInterval(long value)
{
  UpdateInterval = value;
}

void USingleClassListWidget::addComponentSons(RDK::UEPtr<RDK::UContainer> cont, QString componentName, QTreeWidgetItem *treeWidgetFather)
{
    std::vector<std::string> tempbuffer;
    cont->GetComponentsList(tempbuffer);

    QStringList componentNames;
    for(const auto &i: tempbuffer)
        componentNames.push_back(QString::fromStdString(i));

    QString str;
    if(!componentNames.empty() && componentNames[0]!="")
    {
        QString father;
        if(treeWidgetFather)
            treeWidgetFather->setExpanded(false);

        if(!componentName.isEmpty())
            father = componentName + ".";

        foreach(str, componentNames)
        {
            QTreeWidgetItem* childItem = new QTreeWidgetItem(treeWidgetFather);
            childItem->setText(0, str);
            childItem->setData(0, Qt::UserRole, father+str);

            RDK::UEPtr<RDK::UContainer> child = cont->GetComponent(str.toStdString());
            childItem->setData(1, Qt::UserRole, QString::fromStdString(RDK::GetStorageLock()->FindClassName(child->GetClass())));

            addComponentSons(child, father+str, childItem);
        }
    }
}



