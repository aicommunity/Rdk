#ifndef UCOMPONENTSLISTWIDGET_H
#define UCOMPONENTSLISTWIDGET_H

#include "UVisualControllerWidget.h"
#include "UDrawEngineImageWidget.h"

#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QModelIndex>
#include <QApplication>
#include <QHeaderView>

namespace Ui {
class UComponentsListWidget;
}

/// Виджет двигающий компоненты в QTreeWidget по шифту
class UComponentListTreeWidget :public QTreeWidget
{
    Q_OBJECT

public:
    explicit UComponentListTreeWidget(QWidget *parent = 0):QTreeWidget(parent)
    {
        setContextMenuPolicy(Qt::ActionsContextMenu);
        header()->setVisible(false);
    }
    virtual ~UComponentListTreeWidget(){}

signals:
    void moveComponentUp();
    void moveComponentDown();

protected:
    void keyPressEvent(QKeyEvent * event)
    {
        if(QApplication::keyboardModifiers() == Qt::ShiftModifier)
        {
            QTreeWidgetItem *item = currentItem();
            if(!item) return;

            if(event->key() == Qt::Key_Up)
            {
                emit moveComponentUp();
                return;
            }
            if(event->key() == Qt::Key_Down)
            {
                emit moveComponentDown();
                return;
            }
        }
        QTreeWidget::keyPressEvent(event);
    }
};

/// UComponentsListWidget class - виджет отображения списка компонентов модели (UModel)
///
/// Древовидный список компонентов с учетом вложенности, в порядке расчета
/// Содержит сигналы адресованные к схеме сети, но не содержит указателя на схему, сигналы связываются в UGEngineControllWidget.
class UComponentsListWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UComponentsListWidget(QWidget *parent = 0, RDK::UApplication* app = NULL, int channel_mode=1);
    virtual ~UComponentsListWidget();

    /// Перерисовывает дерево текущего канала (С интерфейс с RDK)
    void AUpdateInterface();

    /// запись файла настроек
    virtual void ASaveParameters();
    /// считывание файла настроек
    virtual void ALoadParameters();

    // Доступ к данным для других виджетов:

    /// Устанавливает виджет в вертикальное положение
    void setVerticalOrientation(bool vertical);

    /// Возвращает полное имя выбранного компонента
    QString getSelectedComponentLongName();

    /// открывает определенную вкладку tabWidgetComponentInfo
    ///
    /// 0 - parameters
    /// 1 - state
    /// 2 - inputs
    /// 3 - outputs
    void openTabN(int n);

    /// Возвращает индекс текущего окна
    int currentTabIndex();

    /// Возвращает имя выбранного Property
    QString getSelectedPropertyName();

    /// Возвращает номер выбранного канала
    int getSelectedChannelIndex();

    /// Режим выбора канала
    /// 0 - всегда работа с текущим каналом
    /// 1 - работа с изначально заданным каналом
    void setChannelMode(int mode);

    /// Возвращает номер рабочего канала
    /// используемый при отображении информации
    int getWorkChannelIndex();

    /// устанавливает доступность вкладок
    void setEnableTabN(int n, bool enable);

    /// Устанавливает режим видимости для виджета выбора канала расчёта
    void setChannelsListVisible(bool value);

signals:
    void componentSelected(QString name); //single click
    void componentDoubleClick(QString name);
    void updateScheme(bool forceUpdate);
    void selectedPropertyValue(QString value);

public slots:
    void updateComponentsListFromScheme();
    void componentSelectedFromScheme(QString name); //single click
    void componentDoubleClickFromScheme(QString name);
    void componentStapBackFromScheme();

    void channelsListSelectionChanged();
    void componentListItemSelectionChanged();
    void reloadPropertys(bool forceReload = true);

    void parametersListSelectionChanged();
    void stateListSelectionChanged();
    void inputsListSelectionChanged();
    void outputsListSelectionChanged();

    /// Отправляет событие отрисовки выбранного компонента
    void drawSelectedComponent(QModelIndex index);

    //События контекстного меню
    void componentMoveUp();
    void componentMoveDown();
    void componentRename();
    void componentDelete();
    void componentCopyNameToClipboard();
    void componentCopyLongNameToClipboard();
    void componentCopyClassNameToClipboard();
    void componentReset();
    void componentCalculate();
    void componentGUI();

    /// Внешняя установка UpdateInterval, для уменьшения нагрузки на ядро
    void setUpdateInterval(long value);

private:
    /// Удаляет из переданных данных лидирующие переводы строк
    std::string& EraseLeadEndls(std::string &value);

    /// Удаляет из переданных данных лидирующие и завершающие переводы строк
    std::string& EraseRangeEndls(std::string &value);

    /// Если в переданных данных есть хотя бы один перевод строки, то заменяет текст
    /// на "[SEE BELOW]"
    std::string& PreparePropertyValueToListView(std::string &value);
private:

    /// Имя компонента, чьи проперти отображены
    QString currentDrawPropertyComponentName;

    /// Выделенный компонент. Отличается от currentDrawPropertyComponentName, тем что переписывается
    /// при componentListItemSelectionChanged, затем сравнивается с currentDrawPropertyComponentName,
    /// таким образом избегается перерисовка при множественном выделении одного компонента.
    QString selectedComponentLongName;

    // имена выбранных строк Property
    QString selectedParameterName;
    QString selectedStateName;
    QString selectedInputName;
    QString selectedOutputName;

    /// Текущий канал для виджета
    int currentChannel;

    /// Режим выбора канала
    /// 0 - всегда работа с текущим каналом
    /// 1 - работа с изначально заданным каналом
    int channelMode;

    /// Флаг видимости компонента выбора канала
    bool channelsSelectionVisible;

    /// Указатель на кастомный класс TreeWidget с перемещением компонентов при нажатом shift
    UComponentListTreeWidget *componentsTree;

    /// Компонент владелец отрисованной схемы
    QString currentDrawComponentName;

    /// Скрытый рекурсивный метод заполнения списка компонентов
    void addComponentSons(QString componentName, QTreeWidgetItem *treeWidgetFather, QString oldRootItem, QString oldSelectedItem);

    /// Перерисовка виджета со списком каналов
    void redrawChannelsList();

    Ui::UComponentsListWidget *ui;
};



#endif // UCOMPONENTSLISTWIDGET_H
