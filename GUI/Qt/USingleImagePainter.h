#ifndef USINGLEIMAGEPAINTER_H
#define USINGLEIMAGEPAINTER_H

#include <QWidget>
#include <QPainter>
#include <QMutex>
#include <QMutexLocker>

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
    QImage *dispImage;
    QMutex *loaderMutex;

protected:
    void paintEvent(QPaintEvent*)
    {
        if(!dispImage || !loaderMutex) return;
        QPainter painter(this);

        QMutexLocker locker(loaderMutex);
        painter.drawImage(dispImage->rect(), *dispImage, dispImage->rect());
    }

signals:

public slots:

    void setImage(QImage* image)
    {
        dispImage = image;
        repaint();
    }

    void setLoaderMutex(QMutex *mutex)
    {
        loaderMutex = mutex;
    }
};

#endif // USINGLEIMAGEPAINTER_H
