#ifndef USINGLECLASSLISTWIDGET_H
#define USINGLECLASSLISTWIDGET_H

#include "UVisualControllerWidget.h"
#include <rdk_application.h>
#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QModelIndex>
#include <QApplication>
#include <QHeaderView>

namespace Ui {
class USingleClassListWidget;
}

/// USingleClassListWidget class - виджет отображения класса и его вложенных компонентов
/// Урезанная версия UComponentsListWidget
/// Древовидный список компонентов с учетом вложенности, в порядке расчета
class USingleClassListWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit USingleClassListWidget(std::string class_name, QWidget *parent = 0, RDK::UApplication* app = NULL);
    virtual ~USingleClassListWidget();

    /// запись файла настроек
    virtual void ASaveParameters();
    /// считывание файла настроек
    virtual void ALoadParameters();

    /// Изменение класса для отображения
    void ChangeClass(std::string class_name);

    /// Возвращает полное имя выбранного компонента
    QString getSelectedComponentLongName();

public slots:
    void componentListItemSelectionChanged();

    void parametersListSelectionChanged();

    /// Обновление отображаемого дерева компонентов
    void reloadClassTree();

    /// Обновления отображаемыех свойств
    void reloadPropertys();

    /// Внешняя установка UpdateInterval, для уменьшения нагрузки на ядро
    void setUpdateInterval(long value);

signals:
    void parameterChanged(QString path);

private:
    /// Название класса
    std::string ClassName;

    /// Имя выделенного КЛАССА
    QString selectedClass;

    /// Длинное имя выделеннного КОМПОНЕНТА относительно базового класса
    QString selectedComponentLong;

    /// имена выбранной строки Property
    QString selectedParameterName;

    /// Дерево компонентов
    QTreeWidget *componentsTree;

    /// Скрытый рекурсивный метод заполнения списка компонентов
    void addComponentSons(RDK::UEPtr<RDK::UContainer> cont, QString componentName, QTreeWidgetItem *treeWidgetFather);

    /// Расчет текущего полного пути для выбранного компонента
    QString CalSelectedParameterPath();

    Ui::USingleClassListWidget *ui;
};



#endif // USINGLECLASSLISTWIDGET_H
