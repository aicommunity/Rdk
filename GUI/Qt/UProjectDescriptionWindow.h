#ifndef UPROJECTDESCRIPTIONWINDOW_H
#define UPROJECTDESCRIPTIONWINDOW_H

#include "UVisualControllerMainWidget.h"
#include "UMarkdownViewerWidget.h"

/// Окно для отображения описания проекта (README.md или Description.rtf)
class UProjectDescriptionWindow : public UVisualControllerMainWidget
{
    Q_OBJECT

public:
    explicit UProjectDescriptionWindow(QWidget *parent = nullptr, RDK::UApplication* app = NULL);
    ~UProjectDescriptionWindow();

    // Метод, вызываемый после загрузки проекта
    virtual void AAfterLoadProject(void);

    // Метод, вызываемый перед закрытием проекта
    virtual void ABeforeCloseProject(void);

    // Обновление интерфейса
    virtual void AUpdateInterface(void);

    // Возврат интерфейса в исходное состояние
    virtual void AClearInterface(void);

private:
    /// Загрузить описание проекта из файла
    void loadProjectDescription();

    /// Виджет для отображения Markdown
    UMarkdownViewerWidget* m_markdownViewer;
};

#endif // UPROJECTDESCRIPTIONWINDOW_H
