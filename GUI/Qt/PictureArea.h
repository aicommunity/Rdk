#ifndef PICTUREAREA_H
#define PICTUREAREA_H

#include <QFrame>

class PictureArea : public QFrame
{
 Q_OBJECT

public:
 PictureArea(QWidget *parent);
 ~PictureArea();

private:
 QSize sizeHint() const;
 
protected:
 void dragEnterEvent(QDragEnterEvent *event);
 void dragMoveEvent(QDragMoveEvent *event);
 void dropEvent(QDropEvent *event);
 void mousePressEvent(QMouseEvent *event);
};

#endif // PICTUREAREA_H
