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

    ui->verticalLayout->addWidget(componentsTree);
    connect(componentsTree, SIGNAL(moveComponentUp()), this, SLOT(componentMoveUp()));
    connect(componentsTree, SIGNAL(moveComponentDown()), this, SLOT(componentMoveDown()));

    UpdateInterval = -1;
    setAccessibleName("USingleClassListWidget"); // пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ

    UpdateInterface(true);

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
    connect(componentsTree, SIGNAL(itemSelectionChanged()),
            this, SLOT(componentListItemSelectionChanged()));

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ propertys
    connect(ui->treeWidgetParameters, SIGNAL(itemSelectionChanged()),
            this, SLOT(parametersListSelectionChanged()));

    // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
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
    // пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ treeWidget'пїЅпїЅ
    int componentsListScrollMaximum = componentsTree->verticalScrollBar()->maximum();
    int componentsListScrollPosition = componentsTree->verticalScrollBar()->value();

    componentsTree->clear();

    //пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
    QTreeWidgetItem *rootItem = new QTreeWidgetItem(componentsTree);

    rootItem->setText(0, QString::fromStdString(ClassName));
    rootItem->setData(0, Qt::UserRole, "{CompName}");
    rootItem->setData(1, Qt::UserRole, QString::fromStdString(ClassName));
    rootItem->setExpanded(false);

    auto storage = RDK::GetStorageLock();

    if(!ClassName.empty() && storage->CheckClass(ClassName))
      try
      {
        // пїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ
        auto cont = RDK::dynamic_pointer_cast<RDK::UContainer>(storage->TakeObject(ClassName));
        // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
        addComponentSons(cont, "{CompName}", rootItem);
        // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
        storage->ReturnObject(cont);
      }
      catch(RDK::UException&)
      {
          return;
      }
    componentsTree->setCurrentItem(rootItem);

    // пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ treeWidget'пїЅпїЅ
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

    // пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ treeWidget'пїЅпїЅ
    int paramScrollPosition = ui->treeWidgetParameters->verticalScrollBar()->value();

    try
    {
        UpdateInterfaceFlag=true;

        // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
        auto storage = RDK::GetStorageLock();
        RDK::UEPtr<RDK::UContainer> cont = RDK::dynamic_pointer_cast<RDK::UContainer>(storage->TakeObject(selectedClass.toStdString()));

        if(!cont)
        {
            UpdateInterfaceFlag=false;
            return;
        }
        const RDK::UComponent::VariableMapT& varMap = cont->GetPropertiesList();
        const std::vector<RDK::NameT>& propOrder = cont->GetPropertiesOrder();
        std::string buffer;

        for(size_t oi = 0; oi < propOrder.size(); ++oi)
        {
            RDK::UComponent::VariableMapCIteratorT i = varMap.find(propOrder[oi]);
            if(i == varMap.end())
             continue;
            if(i->second.CheckMask(ptPubParameter))
            {
                QTreeWidgetItem* parametersItem = new QTreeWidgetItem(ui->treeWidgetParameters);
                QString parameterName = QString::fromLocal8Bit(i->first.c_str());
                parametersItem->setText(0, parameterName);
            }
        }

        // пїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ treeWidget'пїЅпїЅ
        ui->treeWidgetParameters->verticalScrollBar()->setMaximum(paramScrollPosition);
        ui->treeWidgetParameters->verticalScrollBar()->setValue(paramScrollPosition);
        UpdateInterfaceFlag=false;

        storage->ReturnObject(cont);
    }
    catch (RDK::UException &exception)
    {
        UpdateInterfaceFlag=false;
        RDK::Logging::SystemLog(exception.GetType(), (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        UpdateInterfaceFlag=false;
        RDK::Logging::SystemLog(RDK_EX_ERROR, (std::string("GUI-UComponentsList Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }

    // пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ
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
        // пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅ пїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
        foreach(str, componentNames)
        {
            QTreeWidgetItem* childItem = new QTreeWidgetItem(treeWidgetFather);
            childItem->setText(0, str);
            // пїЅ 0-пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ UserRole пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
            childItem->setData(0, Qt::UserRole, father+str);

            RDK::UEPtr<RDK::UContainer> child = cont->GetComponent(str.toStdString());
            // пїЅ 1-пїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅ UserRole пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅ пїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅпїЅ
            childItem->setData(1, Qt::UserRole, QString::fromStdString(RDK::GetStorageLock()->FindClassName(child->GetClass())));

            addComponentSons(child, father+str, childItem);
        }
    }
}



