#include "UDrawEngineImageWidget.h"

#include <QDebug>
#include <QByteArray>
#include <QDataStream>
#include <QMimeData>
#include <QAction>
#include <QApplication>
#include <QInputDialog>
#include <QMessageBox>
#include <QClipboard>
#include <QFileDialog>

UDrawEngineImageWidget::UDrawEngineImageWidget(QWidget *parent) : QLabel(parent)
{
 Application=NULL;
    setMinimumSize(QSize(300,350));

    //<����������� ����>
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenu = new QMenu(this);

    //separators
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
    QAction *actionSeparator6 = new QAction(this);
    actionSeparator6->setSeparator(true);
    QAction *actionSeparator7 = new QAction(this);
    actionSeparator7->setSeparator(true);

    QAction *actionSeparator8 = new QAction(this);
    actionSeparator8->setSeparator(true);

    //������� ������������ ����
    actionViewOrBreakLink = new QAction(contextMenu);
    actionViewOrBreakLink->setText("View/Break link");
    actionCreateLink = new QAction(contextMenu);
    actionCreateLink->setText("Create link");   
    actionFinishLink = new QAction(contextMenu);
    actionFinishLink->setText("Finish link");
    actionFinishLink->setEnabled(false);
    actionCancelLink = new QAction(contextMenu);
    actionCancelLink->setText("Cancel link");
    actionCancelLink->setEnabled(false);
    actionStartMoving = new QAction(contextMenu);
    actionStartMoving->setText("Start moving");
    actionFinishMoving = new QAction(contextMenu);
    actionFinishMoving->setText("Finish moving");
    actionFinishMoving->setEnabled(false);
    actionCancelMoving = new QAction(contextMenu);
    actionCancelMoving->setText("Cancel moving");
    actionCancelMoving->setEnabled(false);

    actionSwitchLink = new QAction(contextMenu);
    actionSwitchLink->setText("Switch link");
    actionSwitchLink->setEnabled(true);
    actionFinishSwitching = new QAction(contextMenu);
    actionFinishSwitching->setText("Finish switching");
    actionFinishSwitching->setEnabled(false);
    actionCancelSwitching = new QAction(contextMenu);
    actionCancelSwitching->setText("Cancel switching");
    actionCancelSwitching->setEnabled(false);

    actionCloneComponent = new QAction(contextMenu);
    actionCloneComponent->setText("Clone");
    actionCloneComponent->setEnabled(true);

    QAction *actionRenameComponent = new QAction(contextMenu);
    actionRenameComponent->setText("Rename");
    QAction *actionClassDescription = new QAction(contextMenu);
    actionClassDescription->setText("Class Description");
    QAction *actionDeleteComponent = new QAction(contextMenu);
    actionDeleteComponent->setText("Delete");
    QAction *actionCopyNameToClipboard = new QAction(contextMenu);
    actionCopyNameToClipboard->setText("Copy name to Clipboard");
    QAction *actionCopyLongNameToClipboard = new QAction(contextMenu);
    actionCopyLongNameToClipboard->setText("Copy long name to Clipboard");
    QAction *actionCopyClassNameToClipboard = new QAction(contextMenu);
    actionCopyClassNameToClipboard->setText("Copy class to Clipboard");
    QAction *actionResetComponent = new QAction(contextMenu);
    actionResetComponent->setText("Reset");
    QAction *actionCalculateComponent = new QAction(contextMenu);
    actionCalculateComponent->setText("Calculate");
    QAction *actionDefaultComponent = new QAction(contextMenu);
    actionDefaultComponent->setText("Default");
    QAction *actionGUI = new QAction(contextMenu);
    actionGUI->setText("GUI (not implemented)");
    actionGUI->setEnabled(false);
    QAction *actionCopyComponentXMLDescription= new QAction(contextMenu);
    actionCopyComponentXMLDescription->setText("Copy component XML description");;

    //���������� � ����
    contextMenu->addAction(actionViewOrBreakLink);
    contextMenu->addAction(actionSeparator1);
    contextMenu->addAction(actionCreateLink);
    contextMenu->addAction(actionFinishLink);
    contextMenu->addAction(actionCancelLink);
    contextMenu->addAction(actionSeparator7);
    contextMenu->addAction(actionSwitchLink);
    contextMenu->addAction(actionFinishSwitching);
    contextMenu->addAction(actionCancelSwitching);
    contextMenu->addAction(actionSeparator2);
    contextMenu->addAction(actionStartMoving);
    contextMenu->addAction(actionFinishMoving);
    contextMenu->addAction(actionCancelMoving);
    contextMenu->addAction(actionSeparator3);
    contextMenu->addAction(actionRenameComponent);
    contextMenu->addAction(actionClassDescription);
    contextMenu->addAction(actionDeleteComponent);
    contextMenu->addAction(actionSeparator4);
    contextMenu->addAction(actionCopyNameToClipboard);
    contextMenu->addAction(actionCopyLongNameToClipboard);
    contextMenu->addAction(actionCopyClassNameToClipboard);
    contextMenu->addAction(actionSeparator5);
    contextMenu->addAction(actionResetComponent);
    contextMenu->addAction(actionCalculateComponent);
    contextMenu->addAction(actionDefaultComponent);
    contextMenu->addAction(actionSeparator6);
    contextMenu->addAction(actionGUI);
    contextMenu->addAction(actionCopyComponentXMLDescription);
    contextMenu->addAction(actionSeparator8);
    contextMenu->addAction(actionCloneComponent);

    //�����
    connect(actionViewOrBreakLink, SIGNAL(triggered(bool)), this, SLOT(componentViewOrBreakLink()));
    connect(actionCreateLink, SIGNAL(triggered(bool)), this, SLOT(componentCreateLink()));
    connect(actionFinishLink, SIGNAL(triggered(bool)), this, SLOT(componentFinishLink()));
    connect(actionCancelLink, SIGNAL(triggered(bool)), this, SLOT(componentCancelLink()));
    connect(actionStartMoving, SIGNAL(triggered(bool)), this, SLOT(componentStartMoving()));
    connect(actionFinishMoving, SIGNAL(triggered(bool)), this, SLOT(componentFinishMoving()));
    connect(actionCancelMoving, SIGNAL(triggered(bool)), this, SLOT(componentCancelMoving()));
    connect(actionSwitchLink, SIGNAL(triggered(bool)), this, SLOT(componentStartSwitching()));
    connect(actionFinishSwitching, SIGNAL(triggered(bool)), this, SLOT(componentFinishSwitching()));
    connect(actionCancelSwitching, SIGNAL(triggered(bool)), this, SLOT(componentCancelSwitching()));
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
    connect(actionCloneComponent, SIGNAL(triggered(bool)), this, SLOT(componentCloneComponent()));
    //</����������� ����>

    setScaledContents(true);

    //<��� �� �������>
    Graph.SetCanvas(&GraphCanvas);
    FontType = "Tahoma";
    FontSize = 15;
    RDK::UBitmapFont* font=dynamic_cast<RDK::UBitmapFont*>(RDK::GetCoreLock()->GetFonts().GetFont(FontType,FontSize));
    if(font)
     Font=*font;
    Graph.SetFont(&Font);
    DrawEngine.SetEngine(&Graph);
    DrawEngine.SetFonts(RDK::GetCoreLock()->GetFonts());
    //</��� �� �������>
    reDrawScheme(true);
    setAcceptDrops(true);
}

UDrawEngineImageWidget::~UDrawEngineImageWidget()
{
    delete contextMenu;
    /*delete actionViewOrBreakLink;
    delete actionCreateLink;
    delete actionFinishLink;
    delete actionCancelLink;
    delete actionStartMoving;
    delete actionFinishMoving;
    delete actionCancelMoving;*/
}

void UDrawEngineImageWidget::mousePressEvent(QMouseEvent *event)
{
    selectedComponent = DrawEngine.FindComponent(event->x(), event->y());
    if(!selectedComponent.empty())
    {
        DrawEngine.SelectSingleComponent(selectedComponent);
        emit componentSelected(myLongName());
        reDrawScheme(false);
        if(event->button() == Qt::RightButton)
        {
            contextMenu->popup(QWidget::mapToGlobal(event->pos()));
            return;
        }
        if(QApplication::keyboardModifiers() == Qt::ControlModifier)
        {
            dragEvent();
            return;
        }
    }
    else
        if((event->x() < 10 || event->y() < 10
            || (size().width() - event->x()) < 10 || (size().height() - event->y()) < 10)
                && (event->button() == Qt::RightButton))
        {
            emit componentSelected(myLongName());
            contextMenu->popup(QWidget::mapToGlobal(event->pos()));
        }
}

void UDrawEngineImageWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    //qDebug() << "component name: " << ComponentName;
    if(event->button() == Qt::LeftButton)
    {
        selectedComponent = DrawEngine.FindComponent(event->x(), event->y());
        if(!selectedComponent.empty())
        {
            emit componentDoubleClick(myLongName());
            setComponentName(myLongName());
            selectedComponent="";
        }
    }
    else
        if(event->button() == Qt::RightButton && !ComponentName.isEmpty())
        {
            QString rootWay = "root."+ComponentName;
            QStringList stapBackWay = rootWay.split(".");
            std::string sel_comp=stapBackWay.back().toLocal8Bit().constData();
            stapBackWay.pop_back();
            QString remains = stapBackWay.join(".");
            if(!remains.isEmpty() /*&& remains!="root"*/)
            {
                stapBackWay.pop_front();
                remains = stapBackWay.join(".");
                setComponentName(remains);
                emit componentStapBack();

                selectedComponent=sel_comp;
                DrawEngine.SelectSingleComponent(sel_comp);
                reDrawScheme(false);
            }
        }
}

void UDrawEngineImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
    saveComponentPosition(selectedComponent);
}

void UDrawEngineImageWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!selectedComponent.empty())
    {
        DrawEngine.MoveComponent(selectedComponent, event->x(), event->y());
        reDrawScheme(false);
    }
}

void UDrawEngineImageWidget::dropEvent(QDropEvent *event)
{
    QByteArray itemData = event->mimeData()->data("Component");
    QDataStream dataStream(&itemData, QIODevice::ReadOnly);
    QString classname;
    dataStream >> classname;

    // ���� ������������ �� �������, �� ������� �����
    if(!Application->GetProjectOpenFlag())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Config not created. Auto-create one-channel configuration and model from this component?", QMessageBox::Yes|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            std::string file_name;

            QString default_path=QString::fromLocal8Bit((Application->GetWorkDirectory()+"/../../Configs/").c_str());
            QDir path1(default_path);
            if(!path1.exists(default_path))
            {
                default_path=QString::fromLocal8Bit((Application->GetWorkDirectory()+"/../../../Configs/").c_str());
                QDir path2(default_path);
                if(!path2.exists(default_path))
                {
                    default_path=QString::fromLocal8Bit(Application->GetWorkDirectory().c_str());
                }
            }

            std::string path_dialog=default_path.toUtf8().data();

            // �������� ����� ������� �������������� ���� ����� ������������
            if(QMessageBox::question(this, "Info", "Autocreate configuration folder?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes)
            {
                time_t curr_time;
                time(&curr_time);

                // ���������� ����� � ���� �������� ������ ���� YYYY.MM.DD HH:MM:SS
                std::string folder=RDK::get_text_time(curr_time, '.', '_');
                path_dialog+=std::string("/Autocreate")+folder.c_str();

                if(RDK::CreateNewDirectory(std::string(path_dialog).c_str()) != 0)
                    return;
            }
            else
            {
                path_dialog = QFileDialog::getExistingDirectory(this, tr("Select project directory"), default_path, QFileDialog::ShowDirsOnly).toUtf8().data();
            }

            if(path_dialog.empty())
                return;

            file_name=path_dialog + "/Project.ini";


            Application->CreateProject(file_name, classname.toLocal8Bit().constData());

            selectedComponent = "";
            DrawEngine.SelectSingleComponent(selectedComponent);
            emit componentSelected(QString::fromStdString(selectedComponent));
            reDrawScheme(true);
        }
        return;
    }

    //���� ������ �� ����������, �������� �� ������� �� ��
    if(!Model_Check())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Model not exist. Create new model from this class?", QMessageBox::Yes|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            //������� ����� ������
            Model_Create(classname.toLocal8Bit());
            selectedComponent = "";
            DrawEngine.SelectSingleComponent(selectedComponent);
            emit componentSelected(QString::fromStdString(selectedComponent));
            reDrawScheme(true);
        }
     return;
    }


    if (event->mimeData()->hasFormat("Component"))
    {
        const char* pname = Model_AddComponent(ComponentName.toLocal8Bit(), classname.toLocal8Bit());
        if(pname)
        {
            std::string name=pname;
            Engine_FreeBufString(pname);
            reDrawScheme(true);
            DrawEngine.MoveComponent(name, event->pos().x(), event->pos().y());
            saveComponentPosition(name);
            emit updateComponentsList();

            selectedComponent = name;
            DrawEngine.SelectSingleComponent(selectedComponent);
            emit componentSelected(QString::fromStdString(selectedComponent));
            reDrawScheme(false);
        }
    }
}

void UDrawEngineImageWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat("Component"))
    {
      event->setDropAction(Qt::MoveAction);
      event->accept();
    }
    else
    {
      event->ignore();
    }
}

void UDrawEngineImageWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if(event->mimeData()->hasFormat("Component"))
    {
      event->setDropAction(Qt::MoveAction);
      event->accept();
    }
    else
    {
      event->ignore();
    }
}

void UDrawEngineImageWidget::dragEvent()
{
    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << QString::fromStdString(GetLongName());

    QMimeData *mimeData = new QMimeData();
    mimeData->setData("ComponentName", itemData);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    drag->exec(Qt::CopyAction, Qt::CopyAction);
}


void UDrawEngineImageWidget::resizeEvent(QResizeEvent *)
{
    reDrawScheme(false,true);
}

void UDrawEngineImageWidget::reDrawScheme(bool shouldReloadXml, bool no_resize_canvas)
{
    if(shouldReloadXml)//(!NetXml.GetNumNodes())
    {
        const char *xml=Model_SaveComponentDrawInfo(ComponentName.toLocal8Bit());
        if(xml)
            NetXml.Load(xml,"");
        else
            NetXml.Destroy();
        Engine_FreeBufString(xml);
        DrawEngine.SetNetXml(NetXml);
        shouldReloadXml = false;
    }

    if(!no_resize_canvas)
     ResizeCanvas();
    GraphCanvas.SetRes(width(), height(),RDK::ubmRGB24);
    Graph.SetCanvas(&GraphCanvas);
    DrawEngine.Draw();
    //GraphCanvas.ReflectionX(&ShowCanvas);
    QImage pict((const uchar *) GraphCanvas.GetData(), GraphCanvas.GetWidth(),
                  GraphCanvas.GetHeight(), GraphCanvas.GetLineByteLength(), QImage::Format_RGB888);
    setPixmap(QPixmap::fromImage(pict.rgbSwapped()));

}

void UDrawEngineImageWidget::setComponentName(QString name)
{
    if(ComponentName != name)
    {
        ComponentName = name;
        reDrawScheme(true);
    }
}

void UDrawEngineImageWidget::selectComponent(QString name)
{
    QStringList list(name.split("."));
    std::string nameStd = list.last().toLocal8Bit().constData();
    std::string full_component_name = name.toLocal8Bit().constData();
    std::string expected_component_name;
    std::string::size_type i = full_component_name.find_last_of(".");
    if(i != std::string::npos)
     expected_component_name = full_component_name.substr(0,i);
    if(nameStd!=selectedComponent && !nameStd.empty() && (ComponentName == expected_component_name.c_str()) )
    {
        selectedComponent = nameStd;
        DrawEngine.SelectSingleComponent(nameStd);
        reDrawScheme(false);
    }
    else
    if(ComponentName != expected_component_name.c_str())
    {
        ComponentName = expected_component_name.c_str();
        reDrawScheme(true);
        selectedComponent = nameStd;
        DrawEngine.SelectSingleComponent(nameStd);
        reDrawScheme(false);
    }
}

/// ������ ������ �����
void UDrawEngineImageWidget::ResizeCanvas(void)
{
 int rec_width(width()), rec_height(height());
 DrawEngine.CalcRecommendSize(rec_width,rec_height);
 setFixedSize(rec_width,rec_height);
}

void UDrawEngineImageWidget::componentViewOrBreakLink()
{
    emit viewLinks(myLongName());
}

void UDrawEngineImageWidget::componentCreateLink()
{
    firstComponentToConnection = myLongName();
    actionViewOrBreakLink->setEnabled(false);
    actionCreateLink->setEnabled(false);
    actionFinishLink->setEnabled(true);
    actionCancelLink->setEnabled(true);

    actionStartMoving->setEnabled(false);
    actionFinishMoving->setEnabled(false);
    actionCancelMoving->setEnabled(false);

    actionSwitchLink->setEnabled(false);
    actionFinishSwitching->setEnabled(false);
    actionCancelSwitching->setEnabled(false);
}

void UDrawEngineImageWidget::componentFinishLink()
{
    emit createLinks(firstComponentToConnection, myLongName());

    componentCancelLink();
}

void UDrawEngineImageWidget::componentCancelLink()
{
    actionViewOrBreakLink->setEnabled(true);
    actionCreateLink->setEnabled(true);
    actionFinishLink->setEnabled(false);
    actionCancelLink->setEnabled(false);

    actionStartMoving->setEnabled(true);
    actionFinishMoving->setEnabled(true);
    actionCancelMoving->setEnabled(true);

    actionSwitchLink->setEnabled(true);
    actionFinishSwitching->setEnabled(false);
    actionCancelSwitching->setEnabled(false);
}

void UDrawEngineImageWidget::componentStartSwitching()
{
    startSwitchComponent = myLongName();
    actionFinishMoving->setEnabled(false);
    actionCancelMoving->setEnabled(false);
    actionStartMoving->setEnabled(false);

    actionCreateLink->setEnabled(false);
    actionFinishLink->setEnabled(false);
    actionCancelLink->setEnabled(false);

    actionSwitchLink->setEnabled(false);
    actionFinishSwitching->setEnabled(true);
    actionCancelSwitching->setEnabled(true);
}
void UDrawEngineImageWidget::componentFinishSwitching()
{
    emit switchLinks(startSwitchComponent, myLongName());

    componentCancelSwitching();
}
void UDrawEngineImageWidget::componentCancelSwitching()
{
    actionFinishMoving->setEnabled(false);
    actionCancelMoving->setEnabled(false);
    actionStartMoving->setEnabled(true);

    actionCreateLink->setEnabled(true);
    actionFinishLink->setEnabled(true);
    actionCancelLink->setEnabled(true);

    actionSwitchLink->setEnabled(true);
    actionFinishSwitching->setEnabled(false);
    actionCancelSwitching->setEnabled(false);
}

void UDrawEngineImageWidget::componentStartMoving()
{
    startMoveComponent = myLongName();
    actionFinishMoving->setEnabled(true);
    actionCancelMoving->setEnabled(true);
    actionStartMoving->setEnabled(false);

    actionCreateLink->setEnabled(false);
    actionFinishLink->setEnabled(false);
    actionCancelLink->setEnabled(false);

    actionSwitchLink->setEnabled(false);
    actionFinishSwitching->setEnabled(false);
    actionCancelSwitching->setEnabled(false);
}

void UDrawEngineImageWidget::componentFinishMoving()
{
    endMoveComponent = myLongName();

    if(Model_MoveComponent(startMoveComponent.toLocal8Bit(), endMoveComponent.toLocal8Bit()) != 0)
        QMessageBox::critical(this, "Error", "Component move error", QMessageBox::Ok);

    reDrawScheme(true);
    emit updateComponentsList();
    componentCancelMoving();
}

void UDrawEngineImageWidget::componentCancelMoving()
{
    actionFinishMoving->setEnabled(false);
    actionCancelMoving->setEnabled(false);
    actionStartMoving->setEnabled(true);

    actionCreateLink->setEnabled(true);
    actionFinishLink->setEnabled(true);
    actionCancelLink->setEnabled(true);

    actionSwitchLink->setEnabled(true);
    actionFinishSwitching->setEnabled(false);
    actionCancelSwitching->setEnabled(false);
}

void UDrawEngineImageWidget::componentRename()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Rename component"),
                                         tr("Enter new component name: "), QLineEdit::Normal,
                                         QString::fromStdString(selectedComponent), &ok);
    if (ok && !text.isEmpty())
    {
        std::string new_name(text.toLocal8Bit());
        Model_SetComponentPropertyData(myLongName().toLocal8Bit(),"Name", &new_name);

        emit updateComponentsList();
        reDrawScheme(true);
        //������� ��������� � ����� ������
        selectComponent(ComponentName.isEmpty()? QString::fromStdString(new_name)
                                               : ComponentName + "." + QString::fromStdString(new_name));
        emit componentSelected(myLongName());
    }
}

void UDrawEngineImageWidget::actionClassDescriptionTriggered()
{
    std::string class_name = Model_GetComponentClassName(myLongName().toLocal8Bit());
    classDescription(class_name);
}

void UDrawEngineImageWidget::classDescription(const std::string& class_name)
{
    if(!class_name.empty())
    {
        QMainWindow* classDescWindow = new QMainWindow(this);
        classDescWindow->setAttribute(Qt::WA_DeleteOnClose);
        UClassDescriptionDisplay* display = new UClassDescriptionDisplay(class_name);
        classDescWindow->setCentralWidget(display);
        classDescWindow->setWindowTitle("Class Description");

        classDescWindow->resize(display->size());
        display->show();
        classDescWindow->showNormal();
        classDescWindow->activateWindow();
    }
}

void UDrawEngineImageWidget::componentDelete()
{
    QString selectedComponentLongName = myLongName();
    if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Are you sure you want to delete component "+selectedComponentLongName+"?", QMessageBox::Yes|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) return;
    }

    Model_DelComponent("", selectedComponentLongName.toLocal8Bit());
    RDK::UIVisualControllerStorage::UpdateInterface(true);
    //reDrawScheme(true);
//    emit updateComponentsList();
}

void UDrawEngineImageWidget::componentCopyNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(QString::fromStdString(selectedComponent));
}

void UDrawEngineImageWidget::componentCopyLongNameToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(myLongName());
}

void UDrawEngineImageWidget::componentCopyClassNameToClipboard()
{
    const char *className=Model_GetComponentClassName(myLongName().toLocal8Bit());
    if(className)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString(className));
    }
    Engine_FreeBufString(className);
}

void UDrawEngineImageWidget::componentReset()
{
    Env_Reset(myLongName().toLocal8Bit());
    emit updateComponentsList();
}

void UDrawEngineImageWidget::componentCalculate()
{
    Env_Calculate(myLongName().toLocal8Bit());
    RDK::UIVisualControllerStorage::UpdateInterface();
}

void UDrawEngineImageWidget::componentDefault()
{
    QString selectedComponentLongName = myLongName();
    if(QApplication::keyboardModifiers() != Qt::ShiftModifier)
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Are you sure you want to reset all parameters for component "+selectedComponentLongName+" to default values?", QMessageBox::Yes|QMessageBox::Cancel);
        if (reply == QMessageBox::Cancel) return;
    }

    RDK::UELockPtr<RDK::UStorage> storage = RDK::GetStorageLock();
    std::string stringid = selectedComponentLongName.toLocal8Bit().constData();
    RDK::UEPtr<RDK::UNet> object;
    if(stringid.empty())
     object=RDK::dynamic_pointer_cast<RDK::UNet>(RDK::GetModel());
    else
     object=RDK::dynamic_pointer_cast<RDK::UNet>(RDK::GetEngine()->FindComponent(stringid.c_str()));

    RDK::UEPtr<RDK::UNet> owner = RDK::dynamic_pointer_cast<RDK::UNet>(object->GetOwner());
    RDK::UStringLinksList links_list;

    if(owner)
     object->GetLinks(links_list, owner, true, object);
    storage->DefaultObject(object);
    if(owner)
     object->CreateLinks(links_list, owner);

    RDK::UIVisualControllerStorage::UpdateInterface(true);
}

void UDrawEngineImageWidget::componentGUI()
{
 qDebug() << "component GUI";
}

void UDrawEngineImageWidget::componentCopyXMLDescription()
{
    const char *xmlDescription = Model_SaveComponent(myLongName().toLocal8Bit());
    if(xmlDescription)
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(QString(xmlDescription));
    }
    Engine_FreeBufString(xmlDescription);
}

void UDrawEngineImageWidget::componentCloneComponent()
{
 RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();

 if(!engine)
  return;

 int res=engine->Model_CloneComponent(myLongName().toLocal8Bit().constData(),"");
 if(res == RDK_SUCCESS)
 {
  reDrawScheme(true);
  emit updateComponentsList();
 }
}

void UDrawEngineImageWidget::saveComponentPosition(std::string name)
{
    if(!name.empty())
    {
        const RDK::UGEDescription &descr=DrawEngine.GetDescription(name);
        RDK::USerStorageXML xml;
        xml.Create("Coord");
        std::string buffer;
        RDK::MVector<double,3> pos((descr.Position-DrawEngine.GetOrigin())/DrawEngine.GetZoomCoeff());
        RDK::operator << (xml,pos);
        xml.Save(buffer);

        std::string comp_name=ComponentName.toLocal8Bit().constData();
        if(!comp_name.empty() && comp_name != name)
        Model_SetComponentParameterValue((comp_name+std::string(".")+name).c_str(), "Coord", buffer.c_str());
        else
        Model_SetComponentParameterValue(name.c_str(), "Coord", buffer.c_str());
    }
}

QString UDrawEngineImageWidget::myLongName()
{
    if(ComponentName.isEmpty()) return QString::fromStdString(selectedComponent);
    if(selectedComponent.empty()) return ComponentName;
    return ComponentName + "." + QString::fromStdString(selectedComponent);
}


/// ������������� ��������� �� ����
void UDrawEngineImageWidget::SetApplication(RDK::UApplication *app)
{
 Application=app;
}

/// ������������ ��� ������� ����������
const std::string UDrawEngineImageWidget::GetLongName()
{
    return myLongName().toUtf8().data();
}
