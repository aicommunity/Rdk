#ifndef IMAGESWINDOW_H
#define IMAGESWINDOW_H

#include "../../../../Rdk/Deploy/Include/myrdk.h"

#include <QWidget>
#include <QGridLayout>
#include <QLineEdit>
#include <QStringListModel>
#include <vector>

#include "Image.h"
#include "ui_imageswindow.h"
#include "../../../../Rdk/Gui/Qt/VideoCaptureClass.h"


class ImagesWindow : public QWidget
{
 Q_OBJECT

public:
 ImagesWindow(QWidget *parent = 0);
 ImagesWindow(VideoCaptureClass *mainWindowCapture, QWidget *parent = 0);
 ~ImagesWindow();

private:
 Ui::ImagesWindow ui;
 QGridLayout *layout;
 int rows;
 int columns; 
 Image *selectedImage;
 QPair<QString, QString> changedOutput;
 QStringListModel* capturesNumModel;
 VideoCaptureClass* videoCapturesPointer;
 QSize imageSize;

 // types - источник изображения
 // 0 - вход, device
 // 1 - вход, IP camera
 // 2 - вход, file
 // 3 - вход, изображение
 // 4 - вход, последовательность изображений
 // 5 - выход
 std::vector<std::vector<int> > types;

 // names. Если выход, то first - имя компонента, second - индекс\имя выхода
 std::vector<std::vector<QPair<QString, QString> > > names;

 // изображения для вывода
 std::vector<std::vector<QImage> > images;

 // в конструкторе сюда складываются все QLineEdit, созданные в .ui, для простоты доступа
 std::vector<QLineEdit *> sourceLineEdits;

 void UBitmap2QImage(const RDK::UBitmap* bitmap, QImage &resImage);
 void WriteSettings();
 void ReadSettings();
 
private slots:
 void SelectOutputSourceClicked();
 void AddRow();
 void AddColumn();
 void RmRow();
 void RmColumn();
 void SetImagesSize();
 void ShowControlsFrame();
 void HideControlsFrame();
 void UpdateSources(Image *);
 void SetOutput();
 void SetShownInput();
 void AddSource();
 void DeleteSource();
 void SelectSourceIndexChanged(int i);
 void SetInputConfiguration();
 void BrowseForFile();
 void BrowseForPicture();
 void BrowseForImgSeq();
 void SliderMoved();

public slots:
 void UpdateImages();
 void SaveImageToFile(Image *img);
 void Start();
  
signals:
 void InputConfigurationChanged();
};

#endif // IMAGESWINDOW_H
