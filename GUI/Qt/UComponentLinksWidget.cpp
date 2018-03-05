#include "UComponentLinksWidget.h"
#include "ui_UComponentLinksWidget.h"

#include "rdk_application.h"
#include "map"
#include <QDebug>
#include <QMessageBox>

UComponentLinksWidget::UComponentLinksWidget(QWidget *parent, QString settingsFile, QString settingsGroup) :
    UVisualControllerWidget(parent),
    ui(new Ui::UComponentLinksWidget)
{
    ui->setupUi(this);
    UpdateInterval = 0; //�� ��������� ������ �� ����� ����
    setAccessibleName("UComponentLinksWidget"); // ��� ������ ��� ������������
    ui->treeWidgetInputs->header()->setVisible(true);
    ui->treeWidgetOutputs->header()->setVisible(true);
    readSettings(settingsFile, settingsGroup);

    //������ ��������
    connect(ui->pushButtonCancel, SIGNAL(pressed()), this, SIGNAL(closeWindow()));
    //�������� �����
    connect(ui->pushButtonCreateLink, SIGNAL(pressed()), this, SLOT(createLink()));
    //���������� �����
    connect(ui->pushButtonBreakLink, SIGNAL(pressed()), this, SLOT(breakLink()));

    mode = 0;
}

UComponentLinksWidget::~UComponentLinksWidget()
{
    delete ui;
}

void UComponentLinksWidget::AUpdateInterface()
{
    switch(mode)
    {
    case 0:
        //qDebug("Zero mode update");
        break;

    case 1:
        {
            ui->treeWidgetInputs->clear();
            ui->treeWidgetOutputs->clear();
            ui->treeWidgetLinks->clear();

            QTreeWidgetItem *firstRootItem = new QTreeWidgetItem(ui->treeWidgetOutputs);
            QStringList splitFirstComponentName = QString(firstComponentName).split(".");
            firstRootItem->setText(0, splitFirstComponentName.last());
            firstRootItem->setData(0, Qt::UserRole, firstComponentName);
            firstRootItem->setExpanded(true);

            QTreeWidgetItem *secondRootItem = new QTreeWidgetItem(ui->treeWidgetInputs);
            secondRootItem->setText(0, splitFirstComponentName.last());
            secondRootItem->setData(0, Qt::UserRole, firstComponentName);
            secondRootItem->setExpanded(true);

            addParameters(firstComponentName, firstRootItem, ptPubOutput, secondRootItem, ptPubInput);
            addComponentSons(firstComponentName, firstRootItem, ptPubOutput, secondRootItem, ptPubInput);
            addLinks(firstComponentName);
        }
        break;

    case 2:
        {
            ui->treeWidgetInputs->clear();
            ui->treeWidgetOutputs->clear();
            ui->treeWidgetLinks->clear();

            QTreeWidgetItem *firstRootItem = new QTreeWidgetItem(ui->treeWidgetOutputs);
            QStringList splitFirstComponentName = QString(firstComponentName).split(".");
            firstRootItem->setText(0, splitFirstComponentName.last());
            firstRootItem->setData(0, Qt::UserRole, firstComponentName);
            firstRootItem->setExpanded(true);

            QTreeWidgetItem *secondRootItem = new QTreeWidgetItem(ui->treeWidgetInputs);
            QStringList splitSecondComponentName = QString(secondComponentName).split(".");
            secondRootItem->setText(0, splitSecondComponentName.last());
            secondRootItem->setData(0, Qt::UserRole, secondComponentName);
            secondRootItem->setExpanded(true);

            addParameters(firstComponentName, firstRootItem, ptPubOutput);
            addComponentSons(firstComponentName, firstRootItem, ptPubOutput);

            addParameters(secondComponentName, secondRootItem, ptPubInput);
            addComponentSons(secondComponentName, secondRootItem, ptPubInput);
            addLinks(firstComponentName);
        }
        break;

    default:
        return;
    }
}

void UComponentLinksWidget::initWidget(QString singleComponentName)
{
    firstComponentName = singleComponentName;
    ui->labelOutputsComponentName->setText(singleComponentName);
    ui->labelInputsComponentName->setText(singleComponentName);
    mode = 1;
    UpdateInterface(true);
}

void UComponentLinksWidget::initWidget(QString firstComponentName, QString secondComponentName)
{
    this->firstComponentName = firstComponentName;
    this->secondComponentName = secondComponentName;
    ui->labelOutputsComponentName->setText(firstComponentName);
    ui->labelInputsComponentName->setText(secondComponentName);
    mode = 2;
    UpdateInterface(true);
}

void UComponentLinksWidget::unInit()
{
    firstComponentName.clear();
    secondComponentName.clear();
    ui->treeWidgetInputs->clear();
    ui->treeWidgetOutputs->clear();
    ui->labelInputsComponentName->clear();
    ui->labelOutputsComponentName->clear();
    ui->treeWidgetLinks->clear();
}

void UComponentLinksWidget::createLink()
{
    QString outputComponent, outputName, inputComponent, inputName;
    QTreeWidgetItem *outputItem = ui->treeWidgetOutputs->currentItem();
    QTreeWidgetItem *inputItem = ui->treeWidgetInputs->currentItem();

    if(!outputItem)
    {
        QMessageBox::critical(this,"Output selection error", "Output doesn't selected, please select output and try again.", QMessageBox::Ok);
        return;
    }
    if(!inputItem)
    {
        QMessageBox::critical(this,"Input selection error", "Input doesn't selected, please select Input and try again.", QMessageBox::Ok);
        return;
    }

    outputName = outputItem->text(0);
    inputName = inputItem->text(0);

    if(outputItem->text(1).isEmpty())
    {
        QMessageBox::critical(this,"Output selection error", "Selected output haven't type_info, \nmay be you are trying to select component as output.", QMessageBox::Ok);
        return;
    }
    if(inputItem->text(1).isEmpty())
    {
        QMessageBox::critical(this,"Input selection error", "Selected input haven't type_info, \nmay be you are trying to select component as input.", QMessageBox::Ok);
        return;
    }

    outputComponent = outputItem->parent()->data(0, Qt::UserRole).toString();
    inputComponent = inputItem->parent()->data(0, Qt::UserRole).toString();

    if(inputItem->text(2) != "range" && QApplication::keyboardModifiers() != Qt::ShiftModifier)
    {
        if(outputItem->text(1) != inputItem->text(1))
        {
            QMessageBox::critical(this,"Mismatching types", outputItem->text(1) + " != " + inputItem->text(1), QMessageBox::Ok);
            return;
        }
        if(!inputItem->text(2).isEmpty())
        {
            QMessageBox::critical(this,"The input busy", "The input is already connected!", QMessageBox::Ok);
            return;
        }
    }

    Model_CreateLinkByName(outputComponent.toLocal8Bit(), outputName.toLocal8Bit(), inputComponent.toLocal8Bit(), inputName.toLocal8Bit());
    UpdateInterface(true);
    emit updateScheme(true);
    emit closeWindow();
}

void UComponentLinksWidget::breakLink()
{
    QTreeWidgetItem *linkItem = ui->treeWidgetLinks->currentItem();

    if(!linkItem)
    {
        QMessageBox::critical(this,"Link selection error", "Link doesn't selected, please select link and try again.", QMessageBox::Ok);
        return;
    }

    QString outputComponent = linkItem->text(0),
            outputName = linkItem->text(1),
            inputComponent = linkItem->text(2),
            inputName = linkItem->text(3);

    Model_BreakLinkByName(outputComponent.toLocal8Bit(), outputName.toLocal8Bit(), inputComponent.toLocal8Bit(), inputName.toLocal8Bit());
    UpdateInterface(true);
    emit updateScheme(true);
}

void UComponentLinksWidget::readSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    this->restoreGeometry(settings.value("geometry").toByteArray());
    ui->splitter->restoreState(settings.value("splitterState").toByteArray());
    ui->splitter_2->restoreState(settings.value("splitter_2State").toByteArray());
    ui->treeWidgetInputs->header()->restoreState(settings.value("treeWidgetInputsHeader").toByteArray());
    ui->treeWidgetOutputs->header()->restoreState(settings.value("treeWidgetOutputsHeader").toByteArray());
    ui->treeWidgetLinks->header()->restoreState(settings.value("treeWidgetLinksHeader").toByteArray());
    settings.endGroup();
}

void UComponentLinksWidget::writeSettings(QString file, QString group)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.beginGroup(group);
    settings.setValue("geometry", this->saveGeometry());
    settings.setValue("splitterState", ui->splitter->saveState());
    settings.setValue("splitter_2State", ui->splitter_2->saveState());
    settings.setValue("treeWidgetInputsHeader", ui->treeWidgetInputs->header()->saveState());
    settings.setValue("treeWidgetOutputsHeader", ui->treeWidgetOutputs->header()->saveState());
    settings.setValue("treeWidgetLinksHeader", ui->treeWidgetLinks->header()->saveState());
    settings.endGroup();
}

void UComponentLinksWidget::addComponentSons(QString componentName, QTreeWidgetItem *firstTreeWidgetItemFather, unsigned int firstTypeMask,
                                             QTreeWidgetItem *secondTreeWidgetItemFather, unsigned int secondTypeMask)
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
            QTreeWidgetItem* childItem = new QTreeWidgetItem(firstTreeWidgetItemFather);
            childItem->setExpanded(true);
            childItem->setText(0, str);
            childItem->setData(0, Qt::UserRole, father+str);

            QTreeWidgetItem* secondChildItem = NULL;
            if(secondTreeWidgetItemFather)
            {
                secondChildItem = new QTreeWidgetItem(secondTreeWidgetItemFather);
                secondChildItem->setExpanded(true);
                secondChildItem->setText(0, str);
                secondChildItem->setData(0, Qt::UserRole, father+str);
            }

            if(secondTreeWidgetItemFather)
                addParameters(father+str, childItem, firstTypeMask, secondChildItem, secondTypeMask);
            else
                addParameters(father+str, childItem, firstTypeMask);
            addComponentSons(father+str, childItem, firstTypeMask, secondChildItem, secondTypeMask);
        }
    }
}

void UComponentLinksWidget::addParameters(QString componentName, QTreeWidgetItem *firstTreeWidgetItemFather, unsigned int firstTypeMask,
                                          QTreeWidgetItem *secondTreeWidgetItemFather, unsigned int secondTypeMask)
{
    try
    {
        RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock(Core_GetSelectedChannelIndex());
        RDK::UEPtr<RDK::UContainer> cont = model->GetComponentL(componentName.toLocal8Bit().constData(), true);
        if(!cont) return;
        RDK::UComponent::VariableMapT varMap = cont->GetPropertiesList();

        for(std::map<RDK::NameT,RDK::UVariable>::iterator i = varMap.begin(); i != varMap.end(); ++i)
        {
            if (i->second.CheckMask(firstTypeMask))
            {
                QTreeWidgetItem* firstChildPropertyItem = new QTreeWidgetItem(firstTreeWidgetItemFather);
                firstChildPropertyItem->setText(0, QString::fromLocal8Bit(i->first.c_str()));
                firstChildPropertyItem->setText(1, QString(i->second.Property->GetLanguageType().name()));
                if(i->second.Property->IsConnected()) firstChildPropertyItem->setText(2, QString("connected"));
            }
            if(secondTreeWidgetItemFather && i->second.CheckMask(secondTypeMask))
            {
                QTreeWidgetItem* secondChildPropertyItem = new QTreeWidgetItem(secondTreeWidgetItemFather);
                secondChildPropertyItem->setText(0, QString::fromLocal8Bit(i->first.c_str()));
                secondChildPropertyItem->setText(1, QString(i->second.Property->GetLanguageType().name()));
                if(i->second.Property->GetIoType() & ipRange) secondChildPropertyItem->setText(2, QString("range"));
                else
                    if(i->second.Property->IsConnected()) secondChildPropertyItem->setText(2, QString("connected"));
            }
        }
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("GUI-UComponentsLinks Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-UComponentsLinks Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
}

void UComponentLinksWidget::addLinks(QString componentName)
{
    try
    {
        RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock(Core_GetSelectedChannelIndex());
        RDK::UEPtr<RDK::UNet> cont = model->GetComponentL<RDK::UNet>(componentName.toLocal8Bit().constData(), true);
        if(!cont) return;
        //Model_GetComponentPersonalLinks()
        RDK::UStringLinksList linksList;
        cont->GetLinks(linksList, model.Get());
        RDK::ULinkT<std::string>* linksListIterator = linksList.GetData();
        for(int i = 0; i < linksList.size(); i++)
        {
            for(std::vector<RDK::ULinkSideT<std::string> >::iterator connectorIterator = linksListIterator->Connector.begin();
                connectorIterator != linksListIterator->Connector.end(); connectorIterator++)
            {
                QTreeWidgetItem* item = new QTreeWidgetItem(ui->treeWidgetLinks);
                item->setText(0, QString::fromLocal8Bit(linksListIterator->Item.Id.c_str()));
                item->setText(1, QString::fromLocal8Bit(linksListIterator->Item.Name.c_str()));
                item->setText(2, QString::fromLocal8Bit((*connectorIterator).Id.c_str()));
                item->setText(3, QString::fromLocal8Bit((*connectorIterator).Name.c_str()));
            }
            ++linksListIterator;
        }
    }
    catch (RDK::UException &exception)
    {
        Log_LogMessage(exception.GetType(), (std::string("GUI-UComponentsLinks Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
    catch (std::exception &exception)
    {
        Log_LogMessage(RDK_EX_ERROR, (std::string("GUI-UComponentsLinks Exception: (Name=")+std::string(accessibleName().toLocal8Bit().constData())+std::string(") ")+exception.what()).c_str());
    }
}
