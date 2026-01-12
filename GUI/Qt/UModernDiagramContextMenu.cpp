#include "UModernDiagramContextMenu.h"
#include "UModernDiagramWidget.h"
#include "UModernDiagramNodeItem.h"
#include "UModernDiagramCacheManager.h"
#include "UClassDescriptionDisplay.h"
#include "UQuickLinkDialog.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QApplication>
#include <QMainWindow>
#include <QLineEdit>
#include <QDialog>
#include "../../Deploy/Include/rdk_init.h"
#include "../Core/Engine/UEngine.h"
#include "rdk_application.h"

UModernDiagramContextMenu::UModernDiagramContextMenu(UModernDiagramWidget* owner, QObject* parent)
    : QObject(parent)
    , m_owner(owner)
    , m_contextMenu(nullptr)
    , m_actionViewOrBreakLink(nullptr)
    , m_actionCreateLink(nullptr)
    , m_actionFinishLink(nullptr)
    , m_actionCancelLink(nullptr)
    , m_actionStartMoving(nullptr)
    , m_actionFinishMoving(nullptr)
    , m_actionCancelMoving(nullptr)
    , m_actionSwitchLink(nullptr)
    , m_actionFinishSwitching(nullptr)
    , m_actionCancelSwitching(nullptr)
    , m_actionCloneComponent(nullptr)
    , m_actionQuickLink(nullptr)
    , m_contextMenuNode(nullptr)
{
    createContextMenu();
}

UModernDiagramContextMenu::~UModernDiagramContextMenu()
{
    // QMenu и QAction будут удалены автоматически, так как они являются дочерними элементами
}

void UModernDiagramContextMenu::createContextMenu()
{
    if(!m_owner)
        return;

    m_contextMenu = new QMenu(m_owner);

    // Separators
    QAction* actionSeparator1 = new QAction(m_owner);
    actionSeparator1->setSeparator(true);
    QAction* actionSeparator2 = new QAction(m_owner);
    actionSeparator2->setSeparator(true);
    QAction* actionSeparator3 = new QAction(m_owner);
    actionSeparator3->setSeparator(true);
    QAction* actionSeparator4 = new QAction(m_owner);
    actionSeparator4->setSeparator(true);
    QAction* actionSeparator5 = new QAction(m_owner);
    actionSeparator5->setSeparator(true);
    QAction* actionSeparator6 = new QAction(m_owner);
    actionSeparator6->setSeparator(true);
    QAction* actionSeparator7 = new QAction(m_owner);
    actionSeparator7->setSeparator(true);
    QAction* actionSeparator8 = new QAction(m_owner);
    actionSeparator8->setSeparator(true);

    // Actions
    m_actionViewOrBreakLink = new QAction(m_contextMenu);
    m_actionViewOrBreakLink->setText("View/Break link");

    m_actionCreateLink = new QAction(m_contextMenu);
    m_actionCreateLink->setText("Create link");

    m_actionFinishLink = new QAction(m_contextMenu);
    m_actionFinishLink->setText("Finish link");
    m_actionFinishLink->setEnabled(false);

    m_actionCancelLink = new QAction(m_contextMenu);
    m_actionCancelLink->setText("Cancel link");
    m_actionCancelLink->setEnabled(false);

    m_actionStartMoving = new QAction(m_contextMenu);
    m_actionStartMoving->setText("Start moving");

    m_actionFinishMoving = new QAction(m_contextMenu);
    m_actionFinishMoving->setText("Finish moving");
    m_actionFinishMoving->setEnabled(false);

    m_actionCancelMoving = new QAction(m_contextMenu);
    m_actionCancelMoving->setText("Cancel moving");
    m_actionCancelMoving->setEnabled(false);

    m_actionSwitchLink = new QAction(m_contextMenu);
    m_actionSwitchLink->setText("Switch link");

    m_actionFinishSwitching = new QAction(m_contextMenu);
    m_actionFinishSwitching->setText("Finish switching");
    m_actionFinishSwitching->setEnabled(false);

    m_actionCancelSwitching = new QAction(m_contextMenu);
    m_actionCancelSwitching->setText("Cancel switching");
    m_actionCancelSwitching->setEnabled(false);

    m_actionCloneComponent = new QAction(m_contextMenu);
    m_actionCloneComponent->setText("Clone");

    m_actionQuickLink = new QAction(m_contextMenu);
    m_actionQuickLink->setText("Quick Link...");

    QAction* actionRenameComponent = new QAction(m_contextMenu);
    actionRenameComponent->setText("Rename");

    QAction* actionClassDescription = new QAction(m_contextMenu);
    actionClassDescription->setText("Class Description");

    QAction* actionDeleteComponent = new QAction(m_contextMenu);
    actionDeleteComponent->setText("Delete");

    QAction* actionCopyNameToClipboard = new QAction(m_contextMenu);
    actionCopyNameToClipboard->setText("Copy name to Clipboard");

    QAction* actionCopyLongNameToClipboard = new QAction(m_contextMenu);
    actionCopyLongNameToClipboard->setText("Copy long name to Clipboard");

    QAction* actionCopyClassNameToClipboard = new QAction(m_contextMenu);
    actionCopyClassNameToClipboard->setText("Copy class to Clipboard");

    QAction* actionResetComponent = new QAction(m_contextMenu);
    actionResetComponent->setText("Reset");

    QAction* actionCalculateComponent = new QAction(m_contextMenu);
    actionCalculateComponent->setText("Calculate");

    QAction* actionDefaultComponent = new QAction(m_contextMenu);
    actionDefaultComponent->setText("Default");

    QAction* actionGUI = new QAction(m_contextMenu);
    actionGUI->setText("GUI (not implemented)");
    actionGUI->setEnabled(false);

    QAction* actionCopyComponentXMLDescription = new QAction(m_contextMenu);
    actionCopyComponentXMLDescription->setText("Copy component XML description");

    QAction* actionClearComponentCache = new QAction(m_contextMenu);
    actionClearComponentCache->setText("Clear component cache");

    // Add actions to menu
    m_contextMenu->addAction(m_actionViewOrBreakLink);
    m_contextMenu->addAction(actionSeparator1);
    m_contextMenu->addAction(m_actionCreateLink);
    m_contextMenu->addAction(m_actionFinishLink);
    m_contextMenu->addAction(m_actionCancelLink);
    m_contextMenu->addAction(actionSeparator7);
    m_contextMenu->addAction(m_actionSwitchLink);
    m_contextMenu->addAction(m_actionFinishSwitching);
    m_contextMenu->addAction(m_actionCancelSwitching);
    m_contextMenu->addAction(actionSeparator2);
    m_contextMenu->addAction(m_actionStartMoving);
    m_contextMenu->addAction(m_actionFinishMoving);
    m_contextMenu->addAction(m_actionCancelMoving);
    m_contextMenu->addAction(actionSeparator3);
    m_contextMenu->addAction(actionRenameComponent);
    m_contextMenu->addAction(actionClassDescription);
    m_contextMenu->addAction(actionDeleteComponent);
    m_contextMenu->addAction(actionSeparator4);
    m_contextMenu->addAction(actionCopyNameToClipboard);
    m_contextMenu->addAction(actionCopyLongNameToClipboard);
    m_contextMenu->addAction(actionCopyClassNameToClipboard);
    m_contextMenu->addAction(actionSeparator5);
    m_contextMenu->addAction(actionResetComponent);
    m_contextMenu->addAction(actionCalculateComponent);
    m_contextMenu->addAction(actionDefaultComponent);
    m_contextMenu->addAction(actionSeparator6);
    m_contextMenu->addAction(actionGUI);
    m_contextMenu->addAction(actionCopyComponentXMLDescription);
    m_contextMenu->addAction(actionSeparator8);
    m_contextMenu->addAction(m_actionCloneComponent);
    m_contextMenu->addAction(m_actionQuickLink);
    m_contextMenu->addAction(actionClearComponentCache);

    // Connect signals
    connect(m_actionViewOrBreakLink, SIGNAL(triggered(bool)), this, SLOT(componentViewOrBreakLink()));
    connect(m_actionCreateLink, SIGNAL(triggered(bool)), this, SLOT(componentCreateLink()));
    connect(m_actionFinishLink, SIGNAL(triggered(bool)), this, SLOT(componentFinishLink()));
    connect(m_actionCancelLink, SIGNAL(triggered(bool)), this, SLOT(componentCancelLink()));
    connect(m_actionStartMoving, SIGNAL(triggered(bool)), this, SLOT(componentStartMoving()));
    connect(m_actionFinishMoving, SIGNAL(triggered(bool)), this, SLOT(componentFinishMoving()));
    connect(m_actionCancelMoving, SIGNAL(triggered(bool)), this, SLOT(componentCancelMoving()));
    connect(m_actionSwitchLink, SIGNAL(triggered(bool)), this, SLOT(componentStartSwitching()));
    connect(m_actionFinishSwitching, SIGNAL(triggered(bool)), this, SLOT(componentFinishSwitching()));
    connect(m_actionCancelSwitching, SIGNAL(triggered(bool)), this, SLOT(componentCancelSwitching()));
    connect(actionRenameComponent, SIGNAL(triggered(bool)), this, SLOT(componentRename()));
    connect(actionClassDescription, SIGNAL(triggered(bool)), this, SLOT(actionClassDescriptionTriggered()));
    connect(actionDeleteComponent, SIGNAL(triggered(bool)), this, SLOT(componentDelete()));
    connect(actionCopyNameToClipboard, SIGNAL(triggered(bool)), this, SLOT(componentCopyNameToClipboard()));
    connect(actionCopyLongNameToClipboard, SIGNAL(triggered(bool)), this, SLOT(componentCopyLongNameToClipboard()));
    connect(actionCopyClassNameToClipboard, SIGNAL(triggered(bool)), this, SLOT(componentCopyClassNameToClipboard()));
    connect(actionResetComponent, SIGNAL(triggered(bool)), this, SLOT(componentReset()));
    connect(actionCalculateComponent, SIGNAL(triggered(bool)), this, SLOT(componentCalculate()));
    connect(actionDefaultComponent, SIGNAL(triggered(bool)), this, SLOT(componentDefault()));
    connect(actionGUI, SIGNAL(triggered(bool)), this, SLOT(componentGUI()));
    connect(actionCopyComponentXMLDescription, SIGNAL(triggered(bool)), this, SLOT(componentCopyXMLDescription()));
    connect(m_actionCloneComponent, SIGNAL(triggered(bool)), this, SLOT(componentCloneComponent()));
    connect(m_actionQuickLink, SIGNAL(triggered(bool)), this, SLOT(componentQuickLink()));
    connect(actionClearComponentCache, SIGNAL(triggered(bool)), this, SLOT(componentClearCache()));
}

void UModernDiagramContextMenu::showMenu(const QPoint& globalPos, UModernDiagramNodeItem* node)
{
    if(!m_contextMenu || !m_owner)
        return;

    m_contextMenuNode = node;
    m_contextMenu->popup(globalPos);
}

QString UModernDiagramContextMenu::getSelectedComponentLongName() const
{
    if(!m_contextMenuNode || !m_owner)
        return QString();
    return m_owner->m_componentName.isEmpty() ? m_contextMenuNode->nodeName
                                             : m_owner->m_componentName + "." + m_contextMenuNode->nodeName;
}

void UModernDiagramContextMenu::updateActionsState()
{
    // Метод для обновления состояния действий меню
    // Может быть вызван из виджета при изменении состояния
}

// Context menu action handlers

void UModernDiagramContextMenu::componentViewOrBreakLink()
{
    if(!m_owner)
        return;
    m_owner->emitViewLinks(getSelectedComponentLongName());
}

void UModernDiagramContextMenu::componentCreateLink()
{
    if(!m_owner)
        return;

    m_firstComponentToConnection = getSelectedComponentLongName();
    m_actionViewOrBreakLink->setEnabled(false);
    m_actionCreateLink->setEnabled(false);
    m_actionFinishLink->setEnabled(true);
    m_actionCancelLink->setEnabled(true);

    m_actionStartMoving->setEnabled(false);
    m_actionFinishMoving->setEnabled(false);
    m_actionCancelMoving->setEnabled(false);

    m_actionSwitchLink->setEnabled(false);
    m_actionFinishSwitching->setEnabled(false);
    m_actionCancelSwitching->setEnabled(false);
}

void UModernDiagramContextMenu::componentFinishLink()
{
    if(!m_owner)
        return;

    m_owner->emitCreateLinks(m_firstComponentToConnection, getSelectedComponentLongName());
    componentCancelLink();
}

void UModernDiagramContextMenu::componentCancelLink()
{
    m_actionViewOrBreakLink->setEnabled(true);
    m_actionCreateLink->setEnabled(true);
    m_actionFinishLink->setEnabled(false);
    m_actionCancelLink->setEnabled(false);

    m_actionStartMoving->setEnabled(true);
    m_actionFinishMoving->setEnabled(false);
    m_actionCancelMoving->setEnabled(false);

    m_actionSwitchLink->setEnabled(true);
    m_actionFinishSwitching->setEnabled(false);
    m_actionCancelSwitching->setEnabled(false);
}

void UModernDiagramContextMenu::componentStartSwitching()
{
    if(!m_owner)
        return;

    m_startSwitchComponent = getSelectedComponentLongName();
    m_actionFinishMoving->setEnabled(false);
    m_actionCancelMoving->setEnabled(false);
    m_actionStartMoving->setEnabled(false);

    m_actionCreateLink->setEnabled(false);
    m_actionFinishLink->setEnabled(false);
    m_actionCancelLink->setEnabled(false);

    m_actionSwitchLink->setEnabled(false);
    m_actionFinishSwitching->setEnabled(true);
    m_actionCancelSwitching->setEnabled(true);
}

void UModernDiagramContextMenu::componentFinishSwitching()
{
    if(!m_owner)
        return;

    m_owner->emitSwitchLinks(m_startSwitchComponent, getSelectedComponentLongName());
    componentCancelSwitching();
}

void UModernDiagramContextMenu::componentCancelSwitching()
{
    m_actionFinishMoving->setEnabled(false);
    m_actionCancelMoving->setEnabled(false);
    m_actionStartMoving->setEnabled(true);

    m_actionCreateLink->setEnabled(true);
    m_actionFinishLink->setEnabled(false);
    m_actionCancelLink->setEnabled(false);

    m_actionSwitchLink->setEnabled(true);
    m_actionFinishSwitching->setEnabled(false);
    m_actionCancelSwitching->setEnabled(false);
}

void UModernDiagramContextMenu::componentStartMoving()
{
    if(!m_owner)
        return;

    m_startMoveComponent = getSelectedComponentLongName();
    m_actionFinishMoving->setEnabled(true);
    m_actionCancelMoving->setEnabled(true);
    m_actionStartMoving->setEnabled(false);

    m_actionCreateLink->setEnabled(false);
    m_actionFinishLink->setEnabled(false);
    m_actionCancelLink->setEnabled(false);

    m_actionSwitchLink->setEnabled(false);
    m_actionFinishSwitching->setEnabled(false);
    m_actionCancelSwitching->setEnabled(false);
}

void UModernDiagramContextMenu::componentFinishMoving()
{
    if(!m_owner)
        return;

    QString endMoveComponent = getSelectedComponentLongName();

    if(Model_MoveComponent(m_startMoveComponent.toStdString().c_str(), endMoveComponent.toStdString().c_str()) != RDK_SUCCESS)
        QMessageBox::critical(m_owner, "Error", "Component move error", QMessageBox::Ok);

    m_owner->Reload();
    emit m_owner->updateComponentsList();
    componentCancelMoving();
}

void UModernDiagramContextMenu::componentCancelMoving()
{
    m_actionFinishMoving->setEnabled(false);
    m_actionCancelMoving->setEnabled(false);
    m_actionStartMoving->setEnabled(true);

    m_actionCreateLink->setEnabled(true);
    m_actionFinishLink->setEnabled(false);
    m_actionCancelLink->setEnabled(false);

    m_actionSwitchLink->setEnabled(true);
    m_actionFinishSwitching->setEnabled(false);
    m_actionCancelSwitching->setEnabled(false);
}

void UModernDiagramContextMenu::componentRename()
{
    if(!m_contextMenuNode || !m_owner)
        return;

    bool ok;
    QString text = QInputDialog::getText(m_owner, tr("Rename component"),
                                         tr("Enter new component name: "), QLineEdit::Normal,
                                         m_contextMenuNode->nodeName, &ok);
    if (ok && !text.isEmpty())
    {
        std::string new_name(text.toLocal8Bit().constData());
        Model_SetComponentPropertyData(getSelectedComponentLongName().toLocal8Bit().constData(), "Name", &new_name);

        emit m_owner->updateComponentsList();
        m_owner->Reload();
        // Выбрать компонент с новым именем
        QString newFullName = m_owner->m_componentName.isEmpty() ? text : m_owner->m_componentName + "." + text;
        m_owner->selectComponent(newFullName);
        emit m_owner->componentSelected(newFullName);
    }
}

void UModernDiagramContextMenu::actionClassDescriptionTriggered()
{
    if(!m_contextMenuNode || !m_owner)
        return;

    const char* class_name = Model_GetComponentClassName(getSelectedComponentLongName().toLocal8Bit().constData());
    if(class_name && strlen(class_name) > 0)
    {
        QMainWindow* classDescWindow = new QMainWindow(m_owner);
        classDescWindow->setAttribute(Qt::WA_DeleteOnClose);
        UClassDescriptionDisplay* display = new UClassDescriptionDisplay(std::string(class_name));
        classDescWindow->setCentralWidget(display);
        classDescWindow->setWindowTitle("Class Description");

        classDescWindow->resize(display->size());
        display->show();
        classDescWindow->showNormal();
        classDescWindow->activateWindow();
    }
    Engine_FreeBufString(class_name);
}

void UModernDiagramContextMenu::componentDelete()
{
    if(!m_contextMenuNode || !m_owner)
        return;

    QList<UModernDiagramNodeItem*> nodesToDelete;
    nodesToDelete.append(m_contextMenuNode);
    m_owner->deleteComponents(nodesToDelete);
}

void UModernDiagramContextMenu::componentCopyNameToClipboard()
{
    if(!m_contextMenuNode)
        return;
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(m_contextMenuNode->nodeName);
}

void UModernDiagramContextMenu::componentCopyLongNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(getSelectedComponentLongName());
}

void UModernDiagramContextMenu::componentCopyClassNameToClipboard()
{
    if(!m_contextMenuNode || !m_owner)
        return;
    const char *className = Model_GetComponentClassName(getSelectedComponentLongName().toLocal8Bit().constData());
    if(className)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString(className));
    }
    Engine_FreeBufString(className);
}

void UModernDiagramContextMenu::componentReset()
{
    if(!m_owner)
        return;
    Env_Reset(getSelectedComponentLongName().toLocal8Bit().constData());
    emit m_owner->updateComponentsList();
}

void UModernDiagramContextMenu::componentCalculate()
{
    if(!m_owner)
        return;
    Env_Calculate(getSelectedComponentLongName().toLocal8Bit().constData());
    // TODO: обновить интерфейс если нужно
}

void UModernDiagramContextMenu::componentDefault()
{
    if(!m_owner)
        return;

    QString selectedComponentLongName = getSelectedComponentLongName();
    if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(m_owner, "Warning",
            "Are you sure you want to reset all parameters for component " + selectedComponentLongName + " to default values?",
            QMessageBox::Yes|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel)
            return;
    }

    RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
    std::string stringid = selectedComponentLongName.toLocal8Bit().constData();
    RDK::UEPtr<RDK::UNet> object;
    if(stringid.empty())
        object = RDK::dynamic_pointer_cast<RDK::UNet>(RDK::GetModel());
    else
        object = RDK::dynamic_pointer_cast<RDK::UNet>(RDK::GetEngine()->FindComponent(stringid.c_str()));

    RDK::UEPtr<RDK::UNet> owner = RDK::dynamic_pointer_cast<RDK::UNet>(object->GetOwner());
    RDK::UStringLinksList links_list;

    if(owner)
        object->GetLinks(links_list, owner, true, object);
    storage->DefaultObject(object);
    if(owner)
        object->CreateLinks(links_list, owner);

    m_owner->Reload();
    emit m_owner->updateComponentsList();
}

void UModernDiagramContextMenu::componentGUI()
{
    // Не реализовано
}

void UModernDiagramContextMenu::componentCopyXMLDescription()
{
    if(!m_owner)
        return;

    const char *xmlDescription = Model_SaveComponent(getSelectedComponentLongName().toLocal8Bit().constData());
    if(xmlDescription)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString(xmlDescription));
    }
    Engine_FreeBufString(xmlDescription);
}

void UModernDiagramContextMenu::componentCloneComponent()
{
    if(!m_owner)
        return;

    int res = Model_CloneComponent(getSelectedComponentLongName().toLocal8Bit().constData(), "");
    if(res == RDK_SUCCESS)
    {
        m_owner->Reload();
        emit m_owner->updateComponentsList();
    }
}

void UModernDiagramContextMenu::componentQuickLink()
{
    if(!m_contextMenuNode || !m_owner)
        return;

    UQuickLinkDialog dialog(m_owner, m_owner->m_componentName, m_owner->m_application);

    if(dialog.exec() == QDialog::Accepted)
    {
        QString srcComp = dialog.getSourceComponent();
        QString srcProp = dialog.getSourceProperty();
        QString dstComp = dialog.getTargetComponent();
        QString dstProp = dialog.getTargetProperty();

        if(!srcProp.isEmpty() && !dstProp.isEmpty())
        {
            int result = Model_CreateLinkByName(srcComp.toStdString().c_str(),
                                               srcProp.toStdString().c_str(),
                                               dstComp.toStdString().c_str(),
                                               dstProp.toStdString().c_str());

            if(result == RDK_SUCCESS)
            {
                m_owner->Reload();
                emit m_owner->updateComponentsList();
            }
        }
    }
}

void UModernDiagramContextMenu::componentClearCache()
{
    if(!m_owner)
        return;

    int ret = QMessageBox::question(m_owner, tr("Clear Component Cache"),
                                    tr("Are you sure you want to clear the component cache? This will remove all cached component information."),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if(ret == QMessageBox::Yes)
    {
        if(m_owner->m_cacheManager) m_owner->m_cacheManager->clearComponentCache();
        QMessageBox::information(m_owner, tr("Cache Cleared"), tr("Component cache has been cleared successfully."));
    }
}

