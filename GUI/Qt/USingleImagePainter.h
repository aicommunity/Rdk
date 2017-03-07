#ifndef USINGLEIMAGEPAINTER_H
#define USINGLEIMAGEPAINTER_H

#include <QWidget>
#include <QPainter>

class USingleImagePainter : public QWidget
{
    Q_OBJECT
public:
    explicit USingleImagePainter(QWidget *parent = 0):QWidget(parent)
    {
        dispImage = NULL;
        sizeMode = 0;
    }

    void setImage(QImage* image)
    {
        dispImage = image;
        repaint();
    }

    int getSizeMode() const {return sizeMode;}
    void setSizeMode(int value) {sizeMode = value;}

private:
    QImage* dispImage;
    int sizeMode;

protected:
    void paintEvent(QPaintEvent*)
    {
        if(!dispImage) return;
        QPainter painter(this);
        /*switch (sizeMode)
        {
        case 0:*/
            //resize(dispImage->size());
            painter.drawImage(dispImage->rect(), *dispImage, dispImage->rect());
            /*break;

        case 1:
            painter.drawImage(rect(), *dispImage, dispImage->rect());
            break;

        case 2:
            painter.drawImage(rect(), *dispImage, dispImage->rect());
            break;
        }*/
    }

signals:

public slots:
};

#endif // USINGLEIMAGEPAINTER_H
