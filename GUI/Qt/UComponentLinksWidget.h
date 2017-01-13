#ifndef UCOMPONENTLINKSWIDGET_H
#define UCOMPONENTLINKSWIDGET_H

#include "UVisualControllerWidget.h"
#include <QTreeWidgetItem>

namespace Ui {
class UComponentLinksWidget;
}


/// UComponentLinksWidget class - виджет работы со связями компонента
///
/// Имеет два режима работы:
/// - режим работы с одним компонентом - в этом режиме отображаются все входы и
/// выходы (включая вложенные входы/выходы) одного компонента, и есть возможность создавать связи между собственными входами и выходами.
/// - режим работы с двумя компонентами - в этом режиме отображаются выходы и связи (включая вложенные выходы/связи) первого компонента,
/// и входы второго (включая вложенные входы)
/// Важно! Выводит только публичные входы/выходы

class UComponentLinksWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UComponentLinksWidget(QWidget *parent = 0, QString settingsFile = "settings.qt", QString settingsGroup = "UComponentLinksWidget");
    virtual ~UComponentLinksWidget();
    void AUpdateInterface();    

public slots:
    ///Инициализация окна для просмотра входов/выходов/связей одного компонента
    /// \details Также позволяет создавать связи между компонентами, вложенными в текущий компонент
    void initWidget(QString singleComponentName);

    ///Инициализация окна для создания связей между двумя компонентами компонентами
    void initWidget(QString firstComponentName, QString secondComponentName);

    ///Очищает интерфес, затирая компоненты
    void unInit();

    ///создание связи для выделенной пары выход/вход
    void createLink();

    ///разрушение выделенной связи
    void breakLink();

    ///считывание файлов настроек
    void readSettings(QString file, QString group = "UComponentLinksWidget");
    ///запись файлов настроек
    void writeSettings(QString file, QString group);

signals:
    void updateScheme(bool forceUpdate);

private:
    Ui::UComponentLinksWidget *ui;

    QString settingsFileName;
    QString settingsGroupName;

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

/// Режим работы виджета
///
/// 0 - не задан ни один компонент;
/// 1 - отображение связей/входов/выходв одного компонента;
/// 2 - отображение выходов/связей первого конмопонента, и входов другого;
    int mode;
};

#endif // UCOMPONENTLINKSWIDGET_H
