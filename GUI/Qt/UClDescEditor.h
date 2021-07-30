#ifndef UCLDESCEDITOR_H
#define UCLDESCEDITOR_H

#include <QWidget>
#include <UVisualControllerWidget.h>
#include "UClassDescriptionDisplay.h"
#include "UClassesListWidget.h"

namespace Ui {
class UClDescEditor;
}

class UClDescEditor : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UClDescEditor(QWidget *parent = nullptr, RDK::UApplication *app = NULL);
    ~UClDescEditor();

private:
    Ui::UClDescEditor *ui;

    UClassDescriptionDisplay* ClassDescriptionDisplay;
    UClassesListWidget* ClassesListWidget;

private slots:
    void clListItemChanged();
};

#endif // UCLDESCEDITOR_H
