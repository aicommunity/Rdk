#ifndef USINGLEIMAGEPAINTER_H
#define USINGLEIMAGEPAINTER_H

#include <QWidget>
#include <QPainter>

#include "rdk_application.h"

class USingleImagePainter : public QWidget
{
    Q_OBJECT
public:
    explicit USingleImagePainter(QWidget *parent = 0):QWidget(parent)
    {
        dispImage = NULL;
        loaderMutex = NULL;
    }

private:
    QImage* dispImage;
    UGenericMutex *loaderMutex;

protected:
    void paintEvent(QPaintEvent*)
    {
        if(!dispImage || !loaderMutex) return;
        QPainter painter(this);

        UGenericMutexSharedLocker locker(loaderMutex);
        painter.drawImage(dispImage->rect(), *dispImage, dispImage->rect());
    }

signals:

public slots:

    void setImage(QImage* image)
    {
        dispImage = image;
        repaint();
    }

    void setLoaderMutex(UGenericMutex *mutex)
    {
        loaderMutex = mutex;
    }
};

#endif // USINGLEIMAGEPAINTER_H
