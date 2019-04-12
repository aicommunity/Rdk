#include "UImagesWidget.h"
#include "ui_UImagesWidget.h"

#include <QDialog>
#include <QDialogButtonBox>
#include "UComponentPropertySelectionWidget.h"
#include <QFileDialog>

#define FORWARD_TO_USINGLEIMAGE(gridLayout, selectedImage, imageNum, methodCall) \
  if (imageNum == -1 && selectedImage) \
  { \
    selectedImage->methodCall; \
  } \
  else \
  { \
    if (!(imageNum < 0) && imageNum < gridLayout->count()) \
    { \
      USingleImageWidget *item = qobject_cast<USingleImageWidget*>(gridLayout->itemAt(imageNum)->widget()); \
      if (item) \
      { \
        item->methodCall; \
      } \
    } \
  }

UImagesWidget::UImagesWidget(QWidget *parent, RDK::UApplication* app) :
    UVisualControllerWidget(parent, app),
    ui(new Ui::UImagesWidget)
{
    setAccessibleName("UImagesWidget");
    /*rowsCounter = 1;
    columnsCounter = 1;
    showLegend = true;*/
    imagesSizeMod = 0;
    singleImageMode = false;
    enableChanges=true;
    columnsCounter=0;
    rowsCounter=0;

    UpdateInterval = 0;

    selectedImage = NULL;

    ui->setupUi(this);

    ALoadParameters();

    //главное контекстное меню
    QAction *actionSeparator1 = new QAction(this);
    actionSeparator1->setSeparator(true);
    QAction *actionSeparator2 = new QAction(this);
    actionSeparator2->setSeparator(true);
    QAction *actionSeparator3 = new QAction(this);
    actionSeparator3->setSeparator(true);
    ui->frameImages->addAction(ui->actionSaveToBMP);
    ui->frameImages->addAction(ui->actionSaveToJPEG);
    ui->frameImages->addAction(actionSeparator1);
    ui->frameImages->addAction(ui->actionSaveAllToBMP);
    ui->frameImages->addAction(ui->actionSaveAllToJPEG);
    ui->frameImages->addAction(actionSeparator2);
    ui->frameImages->addAction(ui->actionSelectSource);
    ui->frameImages->addAction(actionSeparator3);
    ui->frameImages->addAction(ui->actionAddColumn);
    ui->frameImages->addAction(ui->actionAddRow);
    ui->frameImages->addAction(ui->actionDeleteColumn);
    ui->frameImages->addAction(ui->actionDeleteRow);

    //connect(ui->action, SIGNAL(triggered(bool)), this, SLOT(action()));
    connect(ui->actionSaveToBMP, SIGNAL(triggered(bool)), this, SLOT(actionSaveToBMP()));
    connect(ui->actionSaveToJPEG, SIGNAL(triggered(bool)), this, SLOT(actionSaveToJPEG()));
    connect(ui->actionSaveAllToBMP, SIGNAL(triggered(bool)), this, SLOT(actionSaveAllToBMP()));
    connect(ui->actionSaveAllToJPEG, SIGNAL(triggered(bool)), this, SLOT(actionSaveAllToJPEG()));
    connect(ui->actionSelectSource, SIGNAL(triggered(bool)), this, SLOT(actionSelectSource()));
    connect(ui->actionAddColumn, SIGNAL(triggered(bool)), this, SLOT(actionAddColumn()));
    connect(ui->actionAddRow, SIGNAL(triggered(bool)), this, SLOT(actionAddRow()));
    connect(ui->actionDeleteColumn, SIGNAL(triggered(bool)), this, SLOT(actionDeleteColumn()));
    connect(ui->actionDeleteRow, SIGNAL(triggered(bool)), this, SLOT(actionDeleteRow()));

    //св€зывание виджетов нижней панельки
    connect(ui->radioButtonOriginalSize, SIGNAL(clicked(bool)), this, SLOT(setOriginalSize()));
    connect(ui->radioButtonTiledSize, SIGNAL(clicked(bool)), this, SLOT(setTiledSize()));
    connect(ui->radioButtonPropSize, SIGNAL(clicked(bool)), this, SLOT(setPropSize()));
    connect(ui->checkBoxShowLegend, SIGNAL(toggled(bool)), this, SLOT(setShowLegend(bool)));
    connect(ui->checkBoxIndChannels, SIGNAL(toggled(bool)), this, SLOT(setIndChannels(bool)));

    //создание первого окошка
    /*USingleImageWidget *item = addSingleItem(0, 0);
    item->setSelected(true);
    selectedImage = item;*/
}

UImagesWidget::~UImagesWidget()
{
    delete ui;
    imagesList.clear();
}

void UImagesWidget::AUpdateInterface()
{

}

///«адает размещение окошек в окне @param layoutW на @param layoutH
void UImagesWidget::SetImageLayout(int layoutW, int layoutH)
{
    clearImagesWidget();
    for(int i=0; i<layoutW; i++)
    {
        for(int j=0; j<layoutH; j++)
        {
            addSingleItem(i,j);
        }
    }
}

/// «адать параметры захвата дл€ текущего окна - номер канала, им€ компонента, свойство
void UImagesWidget::selectCapture(const int channelNum, QString componentLongName, QString propertyName)
{
    USingleImageWidget *item = selectedImage;

    item->setCalcChannel(channelNum);
    item->setComponentName(componentLongName);
    item->setComponentPropertyName(propertyName);
}

/*QMenu* UImagesWidget::getContextMenu()
{
    return contextMenu;
}*/

void UImagesWidget::setCaptureForChannels(int numChannels, QString componentLongName, QString propertyName)
{
  clearImagesWidget();

  columnsCounter = static_cast<int>(sqrt(static_cast<double>(numChannels)));
  if(columnsCounter * columnsCounter == numChannels)
  {
    rowsCounter = columnsCounter;
  }
  else
  {
    if(columnsCounter * (columnsCounter + 1) > numChannels)
    {
      rowsCounter = columnsCounter + 1;
    }
    else
    {
      ++columnsCounter;
      rowsCounter = columnsCounter;
    }
  }

  for(int i = 0, counter = 0; i < rowsCounter; ++i)
    for(int j = 0; j < columnsCounter; ++j, ++counter)
    {
      USingleImageWidget *item = addSingleItem(i, j);
      if(counter < numChannels)
      {
        item->setCalcChannel(counter);
        item->setComponentName(componentLongName);
        item->setComponentPropertyName(propertyName);
      }
    }

  ui->checkBoxIndChannels->setChecked(true);
}

void UImagesWidget::selectImage(int id)
{
  selectImage(qobject_cast<USingleImageWidget*>(ui->gridLayoutImages->itemAt(id)->widget()));
}

void UImagesWidget::setEnableChanges(bool value)
{
  enableChanges = value;
  ui->actionAddColumn    ->setEnabled(value);
  ui->actionAddRow       ->setEnabled(value);
  ui->actionDeleteColumn ->setEnabled(value);
  ui->actionDeleteRow    ->setEnabled(value);
  ui->actionSelectSource ->setEnabled(value);
  ui->checkBoxIndChannels->setEnabled(value);
}

void UImagesWidget::setPolygons(const QList<UDrawablePolygon> &polygons, int imageNum)
{
  FORWARD_TO_USINGLEIMAGE(ui->gridLayoutImages, selectedImage, imageNum, setPolygons(polygons))
  /*if(imageNum == -1 && selectedImage)
  {
    selectedImage->setPolygons(polygons);
    return;
  }

  USingleImageWidget *item = qobject_cast<USingleImageWidget*>(ui->gridLayoutImages->itemAt(imageNum)->widget());
  if (item)
  {
   item->setPolygons(polygons);
  }*/
}

void UImagesWidget::PreventContextMenu()
{
    this->setContextMenuPolicy(Qt::PreventContextMenu);
    ui->frameImages->setContextMenuPolicy(Qt::PreventContextMenu);
    foreach(USingleImageWidget * w, imagesList)
    {
        w->setContextMenuPolicy(Qt::PreventContextMenu);
    }
}

void UImagesWidget::BlockContextMenu()
{
    this->setContextMenuPolicy(Qt::NoContextMenu);
    ui->frameImages->setContextMenuPolicy(Qt::NoContextMenu);
    foreach(USingleImageWidget * w, imagesList)
    {
        w->setContextMenuPolicy(Qt::NoContextMenu);
    }
}

void UImagesWidget::setImagePen(const QPen &value, int imageNum)
{
  //if(selectedImage) selectedImage->setPainterPen(value);
  FORWARD_TO_USINGLEIMAGE(ui->gridLayoutImages, selectedImage, imageNum, setPainterPen(value))
}

void UImagesWidget::setDrawable(bool value, int imageNum)
{
  //selectedImage->setDrawable(value);
  FORWARD_TO_USINGLEIMAGE(ui->gridLayoutImages, selectedImage, imageNum, setDrawable(value))
}

void UImagesWidget::selectPolygon(int id, int imageNum)
{
  FORWARD_TO_USINGLEIMAGE(ui->gridLayoutImages, selectedImage, imageNum, selectPolygon(id))
  //selectedImage->selectPolygon(id);
}

void UImagesWidget::setRectangles(const QPair<QRectF, QRectF> &rects, int imageNum)
{
  FORWARD_TO_USINGLEIMAGE(ui->gridLayoutImages, selectedImage, imageNum, setRectangles(rects))
}

void UImagesWidget::setDrawRects(bool value, int imageNum)
{
  FORWARD_TO_USINGLEIMAGE(ui->gridLayoutImages, selectedImage, imageNum, setDrawRects(value))
}

///»звлекает насто€щую ширину изображени€
int UImagesWidget::GetImageWidth(int imageNum)
{
    if (imageNum == -1 && selectedImage)
    {
      return selectedImage->getImageWidth();
    }
    else
    {
      if (!(imageNum < 0) && imageNum < ui->gridLayoutImages->count())
      {
        USingleImageWidget *item = qobject_cast<USingleImageWidget*>(ui->gridLayoutImages->itemAt(imageNum)->widget());
        if (item)
        {
          return item->getImageWidth();
        }
      }
    }
 return 0;
}
///»звлекает насто€щую высоту изображени€
int UImagesWidget::GetImageHeight(int imageNum)
{
    if (imageNum == -1 && selectedImage)
    {
      return selectedImage->getImageHeight();
    }
    else
    {
      if (!(imageNum < 0) && imageNum < ui->gridLayoutImages->count())
      {
        USingleImageWidget *item = qobject_cast<USingleImageWidget*>(ui->gridLayoutImages->itemAt(imageNum)->widget());
        if (item)
        {
          return item->getImageHeight();
        }
      }
    }
 return 0;
}

void UImagesWidget::ASaveParameters()
{
  if(!application) return;

  QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt", QSettings::IniFormat);
  settings.beginGroup(accessibleName());
  settings.setValue("geometry", saveGeometry());
  settings.setValue("showLegend", ui->checkBoxShowLegend->isChecked());
  settings.setValue("IndChannels", ui->checkBoxIndChannels->isChecked());
  settings.setValue("imagesSizeMod", imagesSizeMod);
  settings.setValue("columnsCounter", columnsCounter);
  settings.setValue("rowsCounter", rowsCounter);

  for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
  {
      if((*i)->getConnected())
      settings.setValue(
                  QString::number((*i)->getRow())+"x"+QString::number((*i)->getColumn()),
                  QString::number((*i)->getCalcChannel())+"*"+(*i)->getComponentName()+"*"+(*i)->getComponentPropertyName());
  }

  settings.endGroup();
}

void UImagesWidget::ALoadParameters()
{
  if(!application) return;

  QSettings settings(QString::fromLocal8Bit(
                       application->GetProjectPath().c_str())+"settings.qt",
                     QSettings::IniFormat);
  settings.beginGroup(accessibleName());

  restoreGeometry(settings.value("geometry").toByteArray());

  ui->checkBoxShowLegend->setChecked(settings.value("showLegend").toBool());
  ui->checkBoxIndChannels->setChecked(settings.value("IndChannels").toBool());

  imagesSizeMod = settings.value("imagesSizeMod").toInt();
  switch(imagesSizeMod)
  {
  case 0:
      ui->radioButtonOriginalSize->setChecked(true);
      break;
  case 1:
      ui->radioButtonPropSize->setChecked(true);
      break;
  case 2:
      ui->radioButtonTiledSize->setChecked(true);
      break;
  }

  //окошки images
  //зачистка
  clearImagesWidget();

  columnsCounter = settings.value("columnsCounter").toInt();
  rowsCounter = settings.value("rowsCounter").toInt();
  if(rowsCounter == 0 || columnsCounter == 0)
  {
      rowsCounter = 1;
      columnsCounter = 1;
  }

  //заполнение
  for(int i = 0; i < rowsCounter; i++)
      for(int j = 0; j < columnsCounter; j++)
      {
          USingleImageWidget *item = addSingleItem(i, j);
          QString source = settings.value(QString::number(i)+"x"+QString::number(j)).toString();
          if(!source.isEmpty())
          {
              QStringList list = source.split("*");
              if (list.size() == 3)
              {
                  item->setCalcChannel(list.at(0).toInt());
                  item->setComponentName(list.at(1));
                  item->setComponentPropertyName(list.at(2));
              }
          }
      }

  settings.endGroup();
}

void UImagesWidget::actionSaveToBMP()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save BMP file"), QApplication::applicationDirPath()+"/../../../Configs", tr("image (*.bmp)"));
    if (!fileName.isEmpty() && selectedImage)
        selectedImage->getImage().save(fileName, "BMP");
}

void UImagesWidget::actionSaveToJPEG()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save JPEG file"), QApplication::applicationDirPath()+"/../../../Configs", tr("image (*.jpeg *.jpg)"));
    if (!fileName.isEmpty() && selectedImage)
        selectedImage->getImage().save(fileName, "JPEG");
}

void UImagesWidget::actionSaveAllToBMP()
{
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Save images directory"), QApplication::applicationDirPath()+"/../../../Configs");
    if (fileName.isEmpty()) return;

    for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
        (*i)->getImage().save(
                fileName + "/" + (*i)->getComponentName() + "." + (*i)->getComponentPropertyName() + ".bmp", "BMP");
}

void UImagesWidget::actionSaveAllToJPEG()
{
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Save images directory"), QApplication::applicationDirPath()+"/../../../Configs");
    if (fileName.isEmpty()) return;

    for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
        (*i)->getImage().save(
                fileName + "/" + (*i)->getComponentName() + "." + (*i)->getComponentPropertyName() + ".jpeg", "JPEG");
}

void UImagesWidget::actionSelectSource()
{
    UComponentPropertySelectionWidget dialog(this, 3, application);
    dialog.componentsList->setChannelsListVisible(ui->checkBoxIndChannels->isChecked());

    if (dialog.exec() && selectedImage)
    {
        selectedImage->setComponentName(dialog.componentsList->getSelectedComponentLongName());
        selectedImage->setComponentPropertyName(dialog.componentsList->getSelectedPropertyName());
        selectedImage->setCalcChannel(dialog.componentsList->getSelectedChannelIndex());
    }
    dialog.writeSettings(QString::fromLocal8Bit(
                           application->GetProjectPath().c_str())+"settings.qt");
}

void UImagesWidget::actionAddColumn()
{
    if(singleImageMode) return;
    for(int i = 0; i<rowsCounter; i++)
    {
        addSingleItem(i, columnsCounter);
    }
    ++columnsCounter;
}

void UImagesWidget::actionAddRow()
{
    if(singleImageMode) return;
    for(int i = 0; i<columnsCounter; i++)
    {
        addSingleItem(rowsCounter, i);
    }
    ++rowsCounter;
}

void UImagesWidget::actionDeleteColumn()
{
    if(singleImageMode) return;
    --columnsCounter;
    QList<USingleImageWidget*>::iterator i = imagesList.begin();
    while(i!=imagesList.end())
    {
        if((*i)->getColumn() == columnsCounter)
        {
            USingleImageWidget *forDelete = *i;
            i = imagesList.erase(i);
            delete forDelete;
        }
        else ++i;
    }
}

void UImagesWidget::actionDeleteRow()
{
    if(singleImageMode) return;
    --rowsCounter;
    QList<USingleImageWidget*>::iterator i = imagesList.begin();
    while(i!=imagesList.end())
    {
        if((*i)->getRow() == rowsCounter)
        {
            USingleImageWidget *forDelete = *i;
            i = imagesList.erase(i);
            delete forDelete;
        }
        else ++i;
    }
}

void UImagesWidget::setShowLegend(bool b)
{
    for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
        (*i)->setShowLegend(b);
}

void UImagesWidget::setIndChannels(bool b)
{
    for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
        (*i)->setShowChannels(b);
}

void UImagesWidget::setOriginalSize()
{
    imagesSizeMod = 0;
    for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
    {
        (*i)->setSize(imagesSizeMod);
        (*i)->reDrawWidget();
    }
}

void UImagesWidget::setTiledSize()
{
    imagesSizeMod = 2;
    for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
    {
        (*i)->setSize(imagesSizeMod);
        (*i)->reDrawWidget();
    }
}

void UImagesWidget::setPropSize()
{
    imagesSizeMod = 1;
    for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
    {
        (*i)->setSize(imagesSizeMod);
        (*i)->reDrawWidget();
    }
}

void UImagesWidget::selectImage(USingleImageWidget *item)
{
  if(item && item != selectedImage)
  {
    selectedImage = item;
    emit selectedImageChannel(selectedImage->getCalcChannel());

    for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
      (*i)->setSelected(false);
    item->setSelected(true);
  }
}

void UImagesWidget::showFullScreenImage(USingleImageWidget *item)
{
    if(singleImageMode)
    {
        for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
            (*i)->show();
        singleImageMode = false;
        if(enableChanges)
        {
            ui->actionAddColumn   ->setEnabled(true);
            ui->actionAddRow      ->setEnabled(true);
            ui->actionDeleteColumn->setEnabled(true);
            ui->actionDeleteRow   ->setEnabled(true);
        }
    }
    else
    {
     if(columnsCounter == 0 || rowsCounter == 0)
      return;
        for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
            (*i)->hide();
        item->show();
        singleImageMode = true;
        if(enableChanges)
        {
            ui->actionAddColumn   ->setEnabled(false);
            ui->actionAddRow      ->setEnabled(false);
            ui->actionDeleteColumn->setEnabled(false);
            ui->actionDeleteRow   ->setEnabled(false);
        }
    }
}

void UImagesWidget::updateImages()
{
    for(QList<USingleImageWidget*>::iterator i = imagesList.begin(); i!=imagesList.end(); i++)
      (*i)->reDrawWidget();
}

/*void UImagesWidget::drawFinished(QPolygonF poly, QSize imgSize)
{
  //ui->frameImages->setContextMenuPolicy(Qt::ActionsContextMenu);

  emit zoneFinished(poly, imgSize);
}*/

USingleImageWidget* UImagesWidget::addSingleItem(int row, int column)
{
    USingleImageWidget *item = new USingleImageWidget(this, row, column, Core_GetSelectedChannelIndex(), ui->checkBoxShowLegend->isChecked(), ui->checkBoxIndChannels->isChecked() , imagesSizeMod);
    ui->gridLayoutImages->addWidget(item, row, column);
    imagesList.push_back(item);
    connect(item, SIGNAL(selectionSignal(USingleImageWidget*)), this, SLOT(selectImage(USingleImageWidget*)));
    connect(item, SIGNAL(fullScreenSignal(USingleImageWidget*)), this, SLOT(showFullScreenImage(USingleImageWidget*)));
    connect(item, SIGNAL(polygonFinished(QPolygonF, QSize)), this, SIGNAL(polygonFinished(QPolygonF, QSize)));
    connect(item, SIGNAL(polygonModified(UDrawablePolygon, QSize)), this, SIGNAL(polygonModified(UDrawablePolygon, QSize)));
    connect(item, SIGNAL(polygonSelected(int)), this, SIGNAL(polygonSelected(int)));

    connect(item, SIGNAL(rectanglesChanged(QPair<QRectF, QRectF>)), this, SIGNAL(rectanglesChanged(QPair<QRectF, QRectF>)));
    selectImage(item);
    return item;
}

void UImagesWidget::clearImagesWidget()
{
  QList<USingleImageWidget*>::iterator deleteIterator = imagesList.begin();
  while(deleteIterator!=imagesList.end())
  {
      USingleImageWidget *forDelete = *deleteIterator;
      deleteIterator = imagesList.erase(deleteIterator);
      delete forDelete;
  }

  columnsCounter = 0;
  rowsCounter = 0;
}
