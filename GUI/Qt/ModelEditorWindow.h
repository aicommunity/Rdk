#ifndef MODELEDITORWINDOW_H
#define MODELEDITORWINDOW_H

#include <QScrollArea>
#include <QSplitter>
#include <QWidget>

#include "PictureArea.h"
#include "ComponentList.h"

class ModelEditorWindow : public QWidget
{
 Q_OBJECT

public:
 ModelEditorWindow(QWidget *parent = 0);
 ~ModelEditorWindow();

private:
 PictureArea *area;
 ComponentList *list;
 QSplitter *splitter;
 QScrollArea *scrollArea;

 void WriteSettings();
 QSize sizeHint() const;
};

#endif // MODELEDITORWINDOW_H
