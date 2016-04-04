#ifndef IMAGE_H
#define IMAGE_H

#include <QAction>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>

class Image : public QScrollArea
{
 Q_OBJECT

public:
 Image(QWidget *parent);
 Image(QSize size, QWidget *parent);
 ~Image();

 QLabel *videoLabel;
 QLabel* textLabel;

 void Deselect();
 void SetTextLabel(QString text);

private:
 QHBoxLayout *layout;
 QAction *actionSaveImage;
 
 void focusInEvent(QFocusEvent * event); 
 void contextMenuEvent(QContextMenuEvent *event);

 bool eventFilter(QObject *obj, QEvent *event);
  
private slots:
 void SaveImageTriggered();
  
signals:
 void ImageSelected(Image *);
 void SaveImage(Image *);
};

#endif // IMAGE_H
