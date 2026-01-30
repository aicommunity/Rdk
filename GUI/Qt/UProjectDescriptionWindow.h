#ifndef UPROJECTDESCRIPTIONWINDOW_H
#define UPROJECTDESCRIPTIONWINDOW_H

#include "UVisualControllerMainWidget.h"
#include "UMarkdownViewerWidget.h"
#include <QPlainTextEdit>
#include <QPushButton>
#include <QToolBar>
#include <QCloseEvent>

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

protected:
    /// Перехватываем событие закрытия окна для проверки несохраненных изменений
    virtual void closeEvent(QCloseEvent *event) override;

private slots:
    /// Переключение между режимами просмотра и редактирования
    void toggleEditMode();

    /// Сохранение README.md в файл и обновление project.ini
    void saveReadme();

    /// Отслеживание изменений в редакторе
    void onTextChanged();

private:
    /// Загрузить описание проекта из файла
    void loadProjectDescription();

    /// Проверка наличия несохраненных изменений
    bool hasUnsavedChanges() const;

    /// Виджет для отображения Markdown
    UMarkdownViewerWidget* m_markdownViewer;

    /// Виджет для редактирования Markdown (показывается в режиме редактирования)
    QPlainTextEdit* m_markdownEditor;

    /// Кнопка переключения режимов просмотр/редактирование
    QPushButton* m_toggleEditButton;

    /// Кнопка сохранения
    QPushButton* m_saveButton;

    /// Toolbar с кнопками
    QToolBar* m_toolBar;

    /// Текущий режим: true = редактирование, false = просмотр
    bool m_isEditMode;

    /// Путь к файлу README.md проекта
    QString m_readmeFilePath;

    /// Исходное содержимое файла (для отслеживания изменений)
    QString m_originalContent;

    /// Флаг наличия несохраненных изменений
    bool m_hasUnsavedChanges;
};

#endif // UPROJECTDESCRIPTIONWINDOW_H
