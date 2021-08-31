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

/// USingleClassListWidget class - виджет отображения класса и его сложенных компонентов
/// Урезанная версия UComponentsListWidget
/// Древовидный список компонентов с учетом вложенности, в порядке расчета
/// Содержит сигналы адресованные к схеме сети, но не содержит указателя на схему, сигналы связываются в UGEngineControllWidget.
class USingleClassListWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit USingleClassListWidget(std::string class_name, QWidget *parent = 0, RDK::UApplication* app = NULL);
    virtual ~USingleClassListWidget();

    /// Отрисовывает дерево компонентов
    void reloadClassTree();

    /// запись файла настроек
    virtual void ASaveParameters();
    /// считывание файла настроек
    virtual void ALoadParameters();


    void ChangeClass(std::string class_name);

    /// Возвращает полное имя выбранного компонента
    QString getSelectedComponentLongName();

    /// устанавливает доступность вкладок
    void setEnableTabN(int n, bool enable);

public slots:
    void componentListItemSelectionChanged();

    void parametersListSelectionChanged();
    void reloadPropertys();

    /// Внешняя установка UpdateInterval, для уменьшения нагрузки на ядро
    void setUpdateInterval(long value);

signals:
    void parameterChanged(QString path);

private:
    std::string ClassName;

    /// Выделенный компонент. Отличается от currentDrawPropertyComponentName, тем что переписывается
    /// при componentListItemSelectionChanged, затем сравнивается с currentDrawPropertyComponentName,
    /// таким образом избегается перерисовка при множественном выделении одного компонента.
    QString selectedClass;

    QString selectedComponentLong;

    /// имена выбранных строк Property
    QString selectedParameterName;

    /// Указатель на кастомный класс TreeWidget с перемещением компонентов при нажатом shift
    QTreeWidget *componentsTree;

    /// Скрытый рекурсивный метод заполнения списка компонентов
    void addComponentSons(RDK::UEPtr<RDK::UContainer> cont, QString componentName, QTreeWidgetItem *treeWidgetFather);

    QString CalSelectedParameterPath();

    Ui::USingleClassListWidget *ui;
};



#endif // USINGLECLASSLISTWIDGET_H
