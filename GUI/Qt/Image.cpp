#include <QDebug>
#include <QInputDialog>
#include <QMenu>

#include "Image.h"


Image::Image(QWidget *parent)
 : QScrollArea(parent)
{
}

Image::Image(QSize size, QWidget *parent)
 : QScrollArea(parent)
{
 setMinimumSize(size);
 setMaximumSize(size);

 videoLabel = new QLabel(this);
 videoLabel->setText("0");

 textLabel = new QLabel(videoLabel);
  
 layout = new QHBoxLayout(this);
 setLayout(layout);
 layout->setContentsMargins(0, 0, 0, 0);
 layout->addWidget(videoLabel);
 videoLabel->setScaledContents(true);
 
 setFrameShape(QFrame::Panel);
 setFrameShadow(QFrame::Raised);

 setFocusPolicy(Qt::ClickFocus);

 actionSaveImage = new QAction("Save image", this);
 connect(actionSaveImage, SIGNAL(triggered()), this, SLOT(SaveImageTriggered()));

 videoLabel->installEventFilter(this);
}

Image::~Image()
{

}

void Image::focusInEvent(QFocusEvent * event)
{
 //qDebug() << "Selection reason: " << event->reason();

 // If focus by tabbing is enabled, the focus gets messed up
 // (like: focus is moved to the first image with the reason "TabFocusReason").
 // So there's only focus on click by now (set in the constructor).

 if (event->reason() != Qt::OtherFocusReason)
 {
  setStyleSheet("Image {border: 1px solid rgb(255, 0, 0);}");
  emit ImageSelected(this);
 }
}

void Image::Deselect()
{
 setStyleSheet("Image {}");
}

void Image::SetTextLabel(QString text)
{
 textLabel->setText(text);
 QFontMetrics metrics(textLabel->font());
 textLabel->setMinimumWidth(metrics.width(text));
}

void Image::contextMenuEvent(QContextMenuEvent *event)
{
 QMenu menu(this);
 menu.addAction(actionSaveImage);
 menu.exec(event->globalPos());
}

bool Image::eventFilter(QObject *obj, QEvent *event)
{
 if (event->type() == QEvent::ContextMenu)
 {
  contextMenuEvent(static_cast<QContextMenuEvent*>(event));
 }
 return false;
}

void Image::SaveImageTriggered()
{
 emit SaveImage(this);
}