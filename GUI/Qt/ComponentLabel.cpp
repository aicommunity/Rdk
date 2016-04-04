#include <QPainter>

#include "ComponentLabel.h"

ComponentLabel::ComponentLabel(QWidget *parent)
  : QLabel(parent)
{

}

ComponentLabel::ComponentLabel(QString text, QPoint position, QWidget *parent)
  : QLabel(parent)
{
  labelText = text;

  QFontMetrics metric(font());
  QSize size = metric.size(Qt::TextSingleLine, text);

  QImage image(size.width() + 12, size.height() + 12, QImage::Format_ARGB32_Premultiplied);
  image.fill(qRgba(0, 0, 0, 0));

  QFont font;
  font.setStyleStrategy(QFont::ForceOutline);

  QPainter painter;
  painter.begin(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setBrush(Qt::lightGray);
  painter.drawRoundedRect(QRectF(0.5, 0.5, image.width()-1, image.height()-1),
  4, 4, Qt::AbsoluteSize);

  painter.setFont(font);
  painter.setBrush(Qt::black);
  painter.drawText(QRect(QPoint(6, 6), size), Qt::AlignCenter, text);
  painter.end();

  setPixmap(QPixmap::fromImage(image));
  move(position);

  setAttribute(Qt::WA_DeleteOnClose);

  show();
}

ComponentLabel::~ComponentLabel()
{

}

void ComponentLabel::setLabelText(const QString& string)
{
  labelText = string;
}

QString ComponentLabel::LabelText()
{
  return labelText;
}