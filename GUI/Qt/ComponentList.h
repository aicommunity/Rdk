#ifndef COMPONENTLIST_H
#define COMPONENTLIST_H

#include <QStringListModel>
#include <QTableView>

class ComponentList : public QTableView
{
  Q_OBJECT

public:
  ComponentList(QWidget *parent);
  ComponentList(QStringList& components, QWidget *parent);
  ~ComponentList();

private:
  QStringListModel *model;

protected:
  void mousePressEvent(QMouseEvent *event);
};

#endif // COMPONENTLIST_H
