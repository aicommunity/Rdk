#ifndef UQUICKLINKDIALOG_H
#define UQUICKLINKDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSplitter>

#include "rdk_application.h"

/// UQuickLinkDialog - диалог для быстрого создания соединений между компонентами
/// Позволяет выбрать исходный выход и целевой вход с фильтрацией по совместимости типов
/// и приоритетом алиасов верхнего уровня

class UQuickLinkDialog : public QDialog
{
    Q_OBJECT

public:
    /// Конструктор
    /// @param parent - родительский виджет
    /// @param componentName - имя компонента, для которого создается связь (может быть пустым для модели)
    /// @param app - указатель на приложение RDK
    explicit UQuickLinkDialog(QWidget *parent, const QString& componentName, RDK::UApplication *app);
    virtual ~UQuickLinkDialog();

    /// Возвращает выбранный выход (источник)
    QString getSelectedOutput() const;

    /// Возвращает имя компонента-источника
    QString getSourceComponent() const;

    /// Возвращает имя свойства-источника
    QString getSourceProperty() const;

    /// Возвращает выбранный вход (приемник)
    QString getSelectedInput() const;

    /// Возвращает имя компонента-приемника
    QString getTargetComponent() const;

    /// Возвращает имя свойства-приемника
    QString getTargetProperty() const;

private slots:
    /// Обновляет списки при изменении фильтра
    void onFilterChanged(const QString& text);

    /// Обрабатывает выбор выхода
    void onOutputSelected(QTreeWidgetItem* item, int column);

    /// Обрабатывает выбор входа
    void onInputSelected(QTreeWidgetItem* item, int column);

    /// Проверяет возможность создания связи
    void validateSelection();

private:
    /// Заполняет дерево выходов
    void populateOutputs();

    /// Заполняет дерево входов
    void populateInputs();

    /// Фильтрует элементы дерева по тексту
    void filterTree(QTreeWidget* tree, const QString& filter);

    /// Создает элемент дерева для свойства
    QTreeWidgetItem* createPropertyItem(const QString& componentName,
                                        const QString& propertyName,
                                        const QString& aliasName,
                                        unsigned int propertyType,
                                        bool isConnected);

    /// Проверяет совместимость типов свойств
    bool areTypesCompatible(unsigned int outputType, unsigned int inputType);

private:
    /// Имя компонента контекста
    QString m_componentName;

    /// Указатель на приложение
    RDK::UApplication* m_application;

    /// Дерево выходов (источников)
    QTreeWidget* m_outputTree;

    /// Дерево входов (приемников)
    QTreeWidget* m_inputTree;

    /// Поле фильтра выходов
    QLineEdit* m_outputFilter;

    /// Поле фильтра входов
    QLineEdit* m_inputFilter;

    /// Кнопка создания связи
    QPushButton* m_createButton;

    /// Кнопка отмены
    QPushButton* m_cancelButton;

    /// Выбранный компонент-источник
    QString m_selectedSourceComponent;

    /// Выбранное свойство-источник
    QString m_selectedSourceProperty;

    /// Выбранный компонент-приемник
    QString m_selectedTargetComponent;

    /// Выбранное свойство-приемник
    QString m_selectedTargetProperty;
};

#endif // UQUICKLINKDIALOG_H

