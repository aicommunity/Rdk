#ifndef COMPONENTLABEL_H
#define COMPONENTLABEL_H

#include <QLabel>

class ComponentLabel : public QLabel
{
  Q_OBJECT

public:
  ComponentLabel(QWidget *parent);
  ComponentLabel(QString text, QPoint position, QWidget *parent);
  ~ComponentLabel();
  void setLabelText(const QString& string);
  QString LabelText();

private:
  QString labelText;

};

#endif // COMPONENTLABEL_H
