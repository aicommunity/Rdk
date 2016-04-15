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

 // types - �������� �����������
 // 0 - ����, device
 // 1 - ����, IP camera
 // 2 - ����, file
 // 3 - ����, �����������
 // 4 - ����, ������������������ �����������
 // 5 - �����
 std::vector<std::vector<int> > types;

 // names. ���� �����, �� first - ��� ����������, second - ������\��� ������
 std::vector<std::vector<QPair<QString, QString> > > names;

 // ����������� ��� ������
 std::vector<std::vector<QImage> > images;

 // � ������������ ���� ������������ ��� QLineEdit, ��������� � .ui, ��� �������� �������
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
