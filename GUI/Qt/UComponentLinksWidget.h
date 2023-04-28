#ifndef UCOMPONENTLINKSWIDGET_H
#define UCOMPONENTLINKSWIDGET_H

#include "UVisualControllerWidget.h"
#include <QTreeWidgetItem>

namespace Ui {
class UComponentLinksWidget;
}


/// UComponentLinksWidget class - виджет работы со связями компонента
///
/// Имеет три режима работы:
/// 1 - режим работы с одним компонентом - в этом режиме отображаются все входы и
/// выходы (включая вложенные входы/выходы) одного компонента, и есть возможность создавать связи между собственными входами и выходами.
/// 2 - режим работы с двумя компонентами - в этом режиме отображаются выходы и связи (включая вложенные выходы/связи) первого компонента,
/// и входы второго (включая вложенные входы)
/// 3 - режим переноса связей с одного компонента на другой - отображаются только доступные выходы первого и второго компонентов
/// включая вложенные выходы и связи
/// Важно! Выводит только публичные входы/выходы

class UComponentLinksWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UComponentLinksWidget(QWidget *parent = 0, RDK::UApplication *app = NULL);
    virtual ~UComponentLinksWidget();
    void AUpdateInterface();    

    /// запись файла настроек
    virtual void ASaveParameters();
    /// считывание файла настроек
    virtual void ALoadParameters();

public slots:
    ///Инициализация окна для просмотра входов/выходов/связей одного компонента
    /// \details Также позволяет создавать связи между компонентами, вложенными в текущий компонент
    void initWidget(QString singleComponentName);

    ///Инициализация окна для создания связей между двумя компонентами компонентами
    void initWidget(QString firstComponentName, QString secondComponentName);

    ///Инициализирует виджет для работы в особом режиме, в частности в режиме переноса связей
    void initWidget(QString firstComponentName, QString secondComponentName, int dlg_mode);

    ///Очищает интерфес, затирая компоненты
    void unInit();

    ///создание связи для выделенной пары выход/вход
    void createLink();

    ///разрушение выделенной связи
    void breakLink();

    ///переключение выделенной связи
    void switchLink();

    void output1ItemSelectionChanged();


signals:
    void updateScheme(bool forceUpdate);

private:
    Ui::UComponentLinksWidget *ui;

    ///Имя компонента источника
    QString firstComponentName;

    ///Имя компонента приемника
    QString secondComponentName;

    ///рекурсивная функция создания дерева вложенных компонентов со списком параметров по маске
    void addComponentSons(QString componentName, QTreeWidgetItem *firstTreeWidgetItemFather, unsigned int firstTypeMask,
                          QTreeWidgetItem *secondTreeWidgetItemFather = NULL, unsigned int secondTypeMask = 0);

    ///Функция добавления параметров по маске
    void addParameters(QString componentName, QTreeWidgetItem *firstTreeWidgetItemFather, unsigned int firstTypeMask,
                       QTreeWidgetItem *secondTreeWidgetItemFather = NULL, unsigned int secondTypeMask = 0);

    ///функция добавления связей в таблицу связей
    void addLinks(QString componentName);

    void resizeEvent(QResizeEvent* event);

/// Режим работы виджета
///
/// 0 - не задан ни один компонент;
/// 1 - отображение связей/входов/выходв одного компонента;
/// 2 - отображение выходов/связей первого конмопонента, и входов другого;
/// 3 - отображение выходов одного компонента и ...
    int mode;
};

#endif // UCOMPONENTLINKSWIDGET_H
