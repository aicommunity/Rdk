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

UDrawEngineImageWidget::UDrawEngineImageWidget(QWidget *parent) : QLabel(parent)
{
    setMinimumSize(QSize(300,350));

    //<контекстное меню>
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

    //событи€ контекстного меню
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
    QAction *actionGUI = new QAction(contextMenu);
    actionGUI->setText("GUI (not implemented)");
    actionGUI->setEnabled(false);
    QAction *actionCopyComponentXMLDescription= new QAction(contextMenu);
    actionCopyComponentXMLDescription->setText("Copy component XML description");;

    //добавление в меню
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
    contextMenu->addAction(actionDeleteComponent);
    contextMenu->addAction(actionSeparator4);
    contextMenu->addAction(actionCopyNameToClipboard);
    contextMenu->addAction(actionCopyLongNameToClipboard);
    contextMenu->addAction(actionCopyClassNameToClipboard);
    contextMenu->addAction(actionSeparator5);
    contextMenu->addAction(actionResetComponent);
    contextMenu->addAction(actionCalculateComponent);
    contextMenu->addAction(actionSeparator6);
    contextMenu->addAction(actionGUI);
    contextMenu->addAction(actionCopyComponentXMLDescription);
    contextMenu->addAction(actionSeparator8);
    contextMenu->addAction(actionCloneComponent);

    //св€зи
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
    connect(actionDeleteComponent, SIGNAL(triggered(bool)), this, SLOT(componentDelete()));
    connect(actionCopyNameToClipboard, SIGNAL(triggered(bool)), this, SLOT(componentCopyNameToClipboard()));
    connect(actionCopyLongNameToClipboard, SIGNAL(triggered(bool)), this, SLOT(componentCopyLongNameToClipboard()));
    connect(actionCopyClassNameToClipboard, SIGNAL(triggered(bool)), this, SLOT(componentCopyClassNameToClipboard()));
    connect(actionResetComponent, SIGNAL(triggered(bool)), this, SLOT(componentReset()));
    connect(actionCalculateComponent, SIGNAL(triggered(bool)), this, SLOT(componentCalculate()));
    connect(actionGUI, SIGNAL(triggered(bool)), this, SLOT(componentGUI()));
    connect(actionCopyComponentXMLDescription, SIGNAL(triggered(bool)), this, SLOT(componentCopyXMLDescription()));
    connect(actionCloneComponent, SIGNAL(triggered(bool)), this, SLOT(componentCloneComponent()));
    //</контекстное меню>

    setScaledContents(true);

    //<код из билдера>
    Graph.SetCanvas(&GraphCanvas);
    FontType = "Tahoma";
    FontSize = 15;
    RDK::UBitmapFont* font=dynamic_cast<RDK::UBitmapFont*>(RDK::GetCoreLock()->GetFonts().GetFont(FontType,FontSize));
    if(font)
     Font=*font;
    Graph.SetFont(&Font);
    DrawEngine.SetEngine(&Graph);
    DrawEngine.SetFonts(RDK::GetCoreLock()->GetFonts());
    //</код из билдера>
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
            contextMenu->popup(QWidget::mapToGlobal(event->pos()));
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
        }
    }
    else
        if(event->button() == Qt::RightButton && !ComponentName.isEmpty())
        {
            QString rootWay = "root."+ComponentName;
            QStringList stapBackWay = rootWay.split(".");
            stapBackWay.pop_back();
            QString remains = stapBackWay.join(".");
            if(!remains.isEmpty() /*&& remains!="root"*/)
            {
                stapBackWay.pop_front();
                remains = stapBackWay.join(".");
                setComponentName(remains);
                emit componentStapBack();
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

    //если модель не существует, спросить не создать ли ее
    if(!Model_Check())
    {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Warning", "Model not exist. Create new model?", QMessageBox::Yes|QMessageBox::Cancel);
        if (reply == QMessageBox::Yes)
        {
            //создать новую модель
            Model_Create(classname.toLocal8Bit());
        }
    }

    if (event->mimeData()->hasFormat("Component"))
    {
        /*QByteArray itemData = event->mimeData()->data("Component");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);
        QString classname;
        dataStream >> classname;*/
        //qDebug() << event->pos() << "   " << classname;

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
    if (event->mimeData()->hasFormat("Component"))
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
    if (event->mimeData()->hasFormat("Component"))
    {
      event->setDropAction(Qt::MoveAction);
      event->accept();
    }
    else
    {
      event->ignore();
    }
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
    if(nameStd!=selectedComponent && !nameStd.empty())
    {
        selectedComponent = nameStd;
        DrawEngine.SelectSingleComponent(nameStd);
        reDrawScheme(false);
    }
}

/// ћен€ет размер канвы
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
        //выбрать компонент с новым именем
        selectComponent(ComponentName.isEmpty()? QString::fromStdString(new_name)
                                               : ComponentName + "." + QString::fromStdString(new_name));
        emit componentSelected(myLongName());
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
    reDrawScheme(true);
    emit updateComponentsList();
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

        if(!ComponentName.isEmpty())
        Model_SetComponentParameterValue((ComponentName.toLocal8Bit().constData()+std::string(".")+name).c_str(), "Coord", buffer.c_str());
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

