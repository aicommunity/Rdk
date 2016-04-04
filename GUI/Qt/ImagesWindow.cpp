#include "../../../Rtv/Deploy/Include/rtv_cpp_initdll.h"

#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QImageWriter>
#include <QMenu>
#include <QPainter>
#include <QSettings>

#include "ImagesWindow.h"
#include "MainWindow.h"
#include "OutputSourceDialog.h"
#include "Sources.h"

ImagesWindow::ImagesWindow(QWidget *parent)
    : QWidget(parent)
{
}

ImagesWindow::ImagesWindow(VideoCaptureClass *mainWindowCapture, QWidget *parent)
    : QWidget(parent)
{
 ui.setupUi(this);
 ui.verticalLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

 layout = new QGridLayout(ui.frame);
 layout->setContentsMargins(3, 3, 3, 3);
 layout->setSpacing(3);
 layout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

 sourceLineEdits.push_back(ui.deviceIdLine);
 sourceLineEdits.push_back(ui.ipCameraUrlLine);
 sourceLineEdits.push_back(ui.fileNameLine);
 sourceLineEdits.push_back(ui.pictureNameLine);
 sourceLineEdits.push_back(ui.sequencePathLine);

 imageSize = QSize(640, 480); // default value
 selectedImage = 0;
 rows = 0;
 columns = 0;
 ReadSettings();
 
 ui.widthSpinBox->setValue(imageSize.width());
 ui.heightSpinBox->setValue(imageSize.height());

 videoCapturesPointer = mainWindowCapture;
 videoCapturesPointer->GetNumCaptures();

 QList<QString> capturesNum;
 for (int i = 0; i < videoCapturesPointer->GetNumCaptures(); i++)
 {
  capturesNum << QString("%1").arg(i);
 }
 capturesNumModel = new QStringListModel(this);
 capturesNumModel->setStringList(capturesNum);
 ui.selectSource1->setModel(capturesNumModel);
 ui.selectSource1->setCurrentIndex(0);
 SelectSourceIndexChanged(0);
 connect(ui.selectSource1, SIGNAL(currentIndexChanged(int)), this, SLOT(SelectSourceIndexChanged(int)));
 connect(ui.selectSource1, SIGNAL(activated(int)), this, SLOT(SelectSourceIndexChanged(int)));

 connect(ui.setSizeButton, SIGNAL(clicked()), this, SLOT(SetImagesSize()));
 connect(ui.addRowButton, SIGNAL(clicked()), this, SLOT(AddRow()));
 connect(ui.addColumnButton, SIGNAL(clicked()), this, SLOT(AddColumn()));
 connect(ui.deleteRowButton, SIGNAL(clicked()), this, SLOT(RmRow()));
 connect(ui.deleteColumnButton, SIGNAL(clicked()), this, SLOT(RmColumn()));

 ui.sourcesControlFrame->hide();

 connect(ui.showSourcesControlsButton, SIGNAL(clicked()), this, SLOT(ShowControlsFrame()));
 connect(ui.hideSourcesControlsButton, SIGNAL(clicked()), this, SLOT(HideControlsFrame()));

 connect(ui.setOutputButton, SIGNAL(clicked()), this, SLOT(SetOutput()));
 connect(ui.outputSourceButton, SIGNAL(clicked()), this, SLOT(SelectOutputSourceClicked()));
 connect(ui.setInputButton, SIGNAL(clicked()), this, SLOT(SetShownInput()));
 connect(ui.setInputConfigButton, SIGNAL(clicked()), this, SLOT(SetInputConfiguration()));
 
 connect(ui.addSource1, SIGNAL(clicked()), this, SLOT(AddSource()));
 connect(ui.deleteSource1, SIGNAL(clicked()), this, SLOT(DeleteSource()));

 connect(ui.fileBrowseButton, SIGNAL(clicked()), this, SLOT(BrowseForFile()));
 connect(ui.pictureBrowseButton, SIGNAL(clicked()), this, SLOT(BrowseForPicture()));
 connect(ui.imgSeqBrowseButton, SIGNAL(clicked()), this, SLOT(BrowseForImgSeq()));

 ui.videoSlider->setMinimum(0);
 ui.videoSlider->setMaximum(0);
 connect(ui.videoSlider, SIGNAL(sliderReleased()), this, SLOT(SliderMoved()));

 //images[0][0] = QImage(640, 480, QImage::Format_RGB888);
 //images[0][1] = QImage(640, 480, QImage::Format_RGB888);
 //images[1][0] = QImage(640, 480, QImage::Format_RGB888);
 //images[1][1] = QImage(640, 480, QImage::Format_RGB888);
}

ImagesWindow::~ImagesWindow()
{
 WriteSettings();
}

void ImagesWindow::AddRow()
{
 for(int j = 0; j < columns; j++)
 {
  Image* im = new Image(imageSize, this);
  connect(im, SIGNAL(ImageSelected(Image *)), this, SLOT(UpdateSources(Image*)));
  connect(im, SIGNAL(SaveImage(Image *)), this, SLOT(SaveImageToFile(Image *)));
  layout->addWidget(im, rows, j);
 }
 rows++;

 types.resize(rows);
 types.at(rows-1).resize(columns);
 names.resize(rows);
 names.at(rows-1).resize(columns);
 images.resize(rows);
 images.at(rows-1).resize(columns);
 for(int j = 0; j < columns; j++)
 {
  types[rows-1][j] = 0;
  names[rows-1][j] = QPair<QString, QString>("", "");
 }
}

void ImagesWindow::AddColumn()
{
 for (int i = 0; i < rows; i++)
 {
  Image* im = new Image(imageSize, this);
  connect(im, SIGNAL(ImageSelected(Image *)), this, SLOT(UpdateSources(Image*)));
  connect(im, SIGNAL(SaveImage(Image *)), this, SLOT(SaveImageToFile(Image *)));
  layout->addWidget(im, i, columns);
 }
 columns++;

 for(int i = 0; i < rows; i++)
 {
  types.at(i).resize(columns);
  names.at(i).resize(columns);
  images.at(i).resize(columns);
  types[i][columns-1] = 0;
  names[i][columns-1] = QPair<QString, QString>("", "");
 }
}

void ImagesWindow::RmRow()
{
 for(int j = 0; j < columns; j++)
 {
  QWidget* widget = layout->itemAtPosition(rows-1, j)->widget(); 
  layout->removeItem(layout->itemAtPosition(rows-1, j));
  delete layout->itemAtPosition(rows-1, j);
  delete widget;
 }
 rows--;

 types.back().pop_back();
 types.pop_back();
 names.back().pop_back();
 names.pop_back();
 images.back().pop_back();
 images.pop_back();
}

void ImagesWindow::RmColumn()
{
 for(int i = 0; i < rows; i++)
 {
  QWidget* widget = layout->itemAtPosition(i, columns-1)->widget(); 
  layout->removeItem(layout->itemAtPosition(i, columns-1));
  delete layout->itemAtPosition(i, columns-1);
  delete widget;
 }
 columns--;

 for(int i = 0; i < rows; i++)
 {
  types.at(i).pop_back();
  names.at(i).pop_back();
  images.at(i).pop_back();
 }
}

void ImagesWindow::SetImagesSize()
{
 imageSize = QSize(ui.widthSpinBox->value(), ui.heightSpinBox->value());

 QList<Image*> allImages = ui.frame->findChildren<Image*>("", Qt::FindDirectChildrenOnly);
 for (int i = 0; i < allImages.size(); i++)
 {
  allImages.at(i)->setMinimumSize(QSize(ui.widthSpinBox->value(), ui.heightSpinBox->value()));
  allImages.at(i)->setMaximumSize(QSize(ui.widthSpinBox->value(), ui.heightSpinBox->value()));
 }
}

void ImagesWindow::UpdateImages()
{
 //qDebug() << "ImagesWindow is hidden: " << isHidden();

 for (int i = 0; i < rows; i++)
  for (int j = 0; j < columns; j++)
  {
   switch(types[i][j])
   {
    case output:  
     {
      UBitmap2QImage(static_cast<const RDK::UBitmap*>
       (Model_GetComponentBitmapOutputByIndex(names[i][j].first.toLocal8Bit(), names[i][j].second.toInt())), images[i][j]);
      (qobject_cast<Image *>(layout->itemAtPosition(i, j)->widget()))->videoLabel->setPixmap(QPixmap::fromImage(images[i][j]));
      break;
     }

    case device:
    case ipCamera:
    case videoFile:
    case pictureFile:
    case imgSequence:
    {
     const cv::Mat* matTmp = videoCapturesPointer->GetFrame(names[i][j].first.toInt()); // what to do with this pointer after it's used?
     QImage tmp(matTmp->data, matTmp->cols, matTmp->rows, matTmp->step, QImage::Format_RGB888); // нужно ли это создавать?
     images[i][j] = tmp;
     (qobject_cast<Image *>(layout->itemAtPosition(i, j)->widget()))->videoLabel->setPixmap(QPixmap::fromImage(images[i][j]));
 
     break;
    }
   }
  }

  // ѕока только дл€ источника 0!
  //if (videoCapturesPointer->GetCaptureSource(0).first == videoFile)
   if (!(ui.videoSlider->isSliderDown()))
   {
    ui.videoSlider->setValue(videoCapturesPointer->GetPosFrame(0));
    ui.frameLabel->setText(QString("%1").arg(videoCapturesPointer->GetPosFrame(0)));
   }
}


void ImagesWindow::UBitmap2QImage(const RDK::UBitmap* bitmap, QImage &resImage)
{
 int colorModel = bitmap->GetColorModel();

 if (colorModel == 3)
 {
  QImage image(bitmap->GetWidth(), bitmap->GetHeight(), QImage::Format_RGB888);
  if(bitmap->GetData())
   memcpy(image.bits(), bitmap->GetData(), bitmap->GetWidth()*bitmap->GetHeight()*3);
  resImage = image;
 }

 else if (colorModel == 400)
 {
  /* // так было
  QImage image(bitmap->GetWidth(), bitmap->GetHeight(), QImage::Format_Indexed8);
  if(bitmap->GetData())
   memcpy(image.bits(), bitmap->GetData(), bitmap->GetWidth()*bitmap->GetHeight());
  resImage = image;
  */

  QImage image(bitmap->GetWidth(), bitmap->GetHeight(), QImage::Format_RGB888);
  if(bitmap->GetData())
  {
   int max = bitmap->GetWidth()*bitmap->GetHeight();
   uchar* dst = image.bits();
   uchar* src = bitmap->GetData();
   for (int i = 0; i < max; i++)
   {
    for (int j = 0; j < 3; j++)
    {
     memcpy(dst, src, 1);
     dst++;
    }
    src++;
   }
  }
  resImage = image;
 }

 else
  return;
}

void ImagesWindow::ShowControlsFrame()
{
 ui.sourcesControlFrame->show();
 ui.showSourcesControlsButton->hide();
}

void ImagesWindow::HideControlsFrame()
{
 ui.sourcesControlFrame->hide();
 ui.showSourcesControlsButton->show();
}

void ImagesWindow::UpdateSources(Image* img)
{
 if (selectedImage != img)
 {
  if (selectedImage != 0)
   selectedImage->Deselect();
  selectedImage = img;
  int rw, cl, rwSpan, clSpan;
  layout->getItemPosition(layout->indexOf(selectedImage), &rw, &cl, &rwSpan, &clSpan);

  switch(types[rw][cl])
  {
  case output:
   ui.tabWidget->setCurrentIndex(0);
   ui.componentNameLine->setText(names[rw][cl].first);
   ui.outputNumberLine->setText(names[rw][cl].second);
   break;

  case device:
  case ipCamera:
  case videoFile:
  case pictureFile:
  case imgSequence:
   if (capturesNumModel->hasIndex(names[rw][cl].first.toInt(), 0))
   {
    ui.tabWidget->setCurrentIndex(1);
    ui.tabWidget_2->setCurrentIndex(types[rw][cl]);
    ui.selectSource1->setCurrentIndex(names[rw][cl].first.toInt());
    sourceLineEdits.at(types[rw][cl])->setText(names[rw][cl].second);
   }
   break;
  }
 }
}

void ImagesWindow::SetOutput()
{
 if (selectedImage != 0)
 {
  int rw, cl, rwSpan, clSpan;
  layout->getItemPosition(layout->indexOf(selectedImage), &rw, &cl, &rwSpan, &clSpan);
  int outputNumber = ui.outputNumberLine->text().toInt();
  QString name = ui.componentNameLine->text().simplified();

  if (Model_CheckComponent(name.toLocal8Bit())) 
   if ((outputNumber >= 0) && (outputNumber < Model_GetComponentNumOutputs(name.toLocal8Bit())))
   {
    types[rw][cl] = output;
    names[rw][cl].first = name;
    names[rw][cl].second = QString("%1").arg(outputNumber);

    // ѕоказать подпись, что это такое, на лейбле картинки
    selectedImage->SetTextLabel(QString("<font color='red'>" + names[rw][cl].first + ", " + names[rw][cl].second + "</font>"));
   }
 }
}

void ImagesWindow::SetShownInput() // choose what is shown
{
 if (selectedImage != 0)
 {
  int rw, cl, rwSpan, clSpan;
  layout->getItemPosition(layout->indexOf(selectedImage), &rw, &cl, &rwSpan, &clSpan);

  // what kind of input it is (device, file, ip camera etc)
  types[rw][cl] = ui.tabWidget_2->currentIndex();
  // which capture is used
  names[rw][cl].first = QString("%1").arg(ui.selectSource1->currentIndex());
  // capture arguments (the same as in VideoCaptureClass::sources[i].second)
  names[rw][cl].second = videoCapturesPointer->GetCaptureSource(ui.selectSource1->currentIndex()).second;

  selectedImage->SetTextLabel(QString("<font color='red'>" + 
   names[rw][cl].first + ", " + names[rw][cl].second + "</font>"));
 }
}

void ImagesWindow::WriteSettings()
{
 QSettings settings("Settings.ini", QSettings::IniFormat);
 settings.beginGroup("images_window");
 settings.setValue("image_size", imageSize);
 settings.setValue("rows", rows);
 settings.setValue("columns", columns);
 settings.remove("types");
 settings.remove("names");
 for(int i = 0; i < rows; i++)
  for(int j = 0; j < columns; j++)
  {
   settings.setValue(QString("types/_%1_%2_").arg(i).arg(j), types[i][j]);
   settings.setValue(QString("names/_%1_%2_.first").arg(i).arg(j), names[i][j].first);
   settings.setValue(QString("names/_%1_%2_.second").arg(i).arg(j), names[i][j].second);
  }
 settings.endGroup();
}

void ImagesWindow::ReadSettings()
{
 QSettings settings("Settings.ini", QSettings::IniFormat);
 settings.beginGroup("images_window");
 imageSize = settings.value("image_size", QSize(320, 240)).toSize();
 int rw = settings.value("rows", 0).toInt();
 int cl = settings.value("columns", 0).toInt();

 if (rw == 0)
  AddRow();
 if (cl == 0)
  AddColumn();
 
 while(rw--)
  AddRow();
 while(cl--)
  AddColumn();
 for(int i = 0; i < rows; i++)
  for(int j = 0; j < columns; j++)
  {
   types[i][j] = (settings.value(QString("types/_%1_%2_").arg(i).arg(j))).toInt();
   names[i][j].first = (settings.value(QString("names/_%1_%2_.first").arg(i).arg(j))).toString();
   names[i][j].second = (settings.value(QString("names/_%1_%2_.second").arg(i).arg(j))).toString();
  }

 QList<Image *> allImages = ui.frame->findChildren<Image *>();
 int rws, cls, rwSpan, clSpan;
 for (int i = 0; i < allImages.size(); i++)
 {
  layout->getItemPosition(layout->indexOf(allImages.at(i)), &rws, &cls, &rwSpan, &clSpan);
  allImages.at(i)->SetTextLabel(QString("<font color='red'>" + names[rws][cls].first + ", " + names[rws][cls].second + "</font>"));
 }

 settings.endGroup();
}

void ImagesWindow::SelectOutputSourceClicked()
{
 OutputSourceDialog dialog(changedOutput, this);
 int result = dialog.exec();
 if (result == 1)
 {
  ui.componentNameLine->setText(changedOutput.first);
  ui.outputNumberLine->setText(changedOutput.second);
 }
}

void ImagesWindow::AddSource()
{
 // Add source in VideoCaptureClass
 videoCapturesPointer->AddCapture();

 // Add item in view model
 int row = capturesNumModel->rowCount();
 capturesNumModel->insertRows(row, 1);
 capturesNumModel->setData(capturesNumModel->index(row), QString("%1").arg(row));
}

void ImagesWindow::DeleteSource()
{
 // Delete source in VideoCaptureClass
 videoCapturesPointer->DeleteCapture();

 // Delete item in view model
 capturesNumModel->removeRows(capturesNumModel->rowCount()-1, 1);
}

void ImagesWindow::SelectSourceIndexChanged(int i)
{
 // при выборе другого источника было бы здорово показать кадр (но где?)
 if (videoCapturesPointer->GetCaptureSource(i).second != "")
 {
  ui.tabWidget_2->setCurrentIndex(videoCapturesPointer->GetCaptureSource(i).first);
  sourceLineEdits.at(videoCapturesPointer->GetCaptureSource(i).first)->setText(videoCapturesPointer->GetCaptureSource(i).second);
 }
}

void ImagesWindow::SetInputConfiguration()
{
 int i = ui.tabWidget_2->currentIndex();

 videoCapturesPointer->SetCaptureSource(QPair<int, QString>(i, 
  sourceLineEdits.at(i)->text()), ui.selectSource1->currentIndex());

 for (int j = 0; j < rows; j++)
  for (int k = 0; k < columns; k++)
  {
   if ((types[j][k] != output) && (names[j][k].first.toInt() == ui.selectSource1->currentIndex()))
   {
    types[j][k] = i;
    names[j][k].first = QString("%1").arg(ui.selectSource1->currentIndex());
    names[j][k].second = videoCapturesPointer->GetCaptureSource(ui.selectSource1->currentIndex()).second;
    qobject_cast<Image *>(layout->itemAtPosition(j, k)->widget())->SetTextLabel
     (QString("<font color='red'>" + names[j][k].first + ", " + names[j][k].second + "</font>"));
   }
  }

  // ≈сли уже открыт capture, также нужно его открыть заново с новым источником
  // но только если он не совпадает с текущим
  // и он среди открытых источников
  emit InputConfigurationChanged();
}

void ImagesWindow::BrowseForFile()
{
 QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), QString("C:/"), tr("Video Files (*.avi);;All Files (*.*)"));
 if (!fileName.isEmpty())
  ui.fileNameLine->setText(fileName);
}

void ImagesWindow::BrowseForPicture()
{
 QString fileName = QFileDialog::getOpenFileName(this, tr("Open"), QString("C:/"), 
  tr("Picture Files (*.png *.jpg *.bmp));;All Files (*.*)"));
 if (!fileName.isEmpty())
  ui.pictureNameLine->setText(fileName);
}

void ImagesWindow::BrowseForImgSeq()
{
 QFileDialog dialog;
 dialog.setFileMode(QFileDialog::Directory);
 //dialog.setOption(QFileDialog::ShowDirsOnly);
 if (dialog.exec())
  // take the only one or the first one chosen folder as only one folder is needed
  ui.sequencePathLine->setText(dialog.selectedFiles().at(0));
}

// –ешение временное: пока только дл€ источника видео номер 0.
// ƒл€ количества источников более 1 нужно куда-нибудь переместить slider из боковой панели и переделать всЄ.
void ImagesWindow::SliderMoved() 
{
 videoCapturesPointer->SetPosFrame(ui.videoSlider->value(), 0); 
 ui.frameLabel->setText(QString("%1").arg(ui.videoSlider->value()));
}

void ImagesWindow::Start()
{
 // ѕока только дл€ источника 0.
 //if (videoCapturesPointer->GetCaptureSource(0).first == videoFile)
 //{
  ui.videoSlider->setMaximum(videoCapturesPointer->GetNumFrames(0)); 
 //}
}

void ImagesWindow::SaveImageToFile(Image *img)
{
 int rw, cl, rwSpan, clSpan;
 layout->getItemPosition(layout->indexOf(selectedImage), &rw, &cl, &rwSpan, &clSpan);

 QDateTime date = QDateTime::currentDateTime();
 QString name = "C:/Work/" + date.toString("yyyy.MM.dd_hh-mm-ss-zzz") + ".png";

 //images[rw][cl].save(name,"BMP");

 QImageWriter writer;
 writer.setFormat("png");
 writer.setFileName(name);
 writer.setQuality(100);
 if (writer.write(images[rw][cl]))
  qDebug() << "The image was written successfully";
 else 
  qDebug() << writer.errorString();
}
