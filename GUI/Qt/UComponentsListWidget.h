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

/// ������ ��������� ���������� � QTreeWidget �� �����
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

/// UComponentsListWidget class - ������ ����������� ������ ����������� ������ (UModel)
///
/// ����������� ������ ����������� � ������ �����������, � ������� �������
/// �������� ������� ������������ � ����� ����, �� �� �������� ��������� �� �����, ������� ����������� � UGEngineControllWidget.
class UComponentsListWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UComponentsListWidget(QWidget *parent = 0, RDK::UApplication* app = NULL, int channel_mode=1);
    virtual ~UComponentsListWidget();

    /// �������������� ������ �������� ������ (� ��������� � RDK)
    void AUpdateInterface();

    /// ������ ����� ��������
    virtual void ASaveParameters();
    /// ���������� ����� ��������
    virtual void ALoadParameters();

    // ������ � ������ ��� ������ ��������:

    /// ������������� ������ � ������������ ���������
    void setVerticalOrientation(bool vertical);

    /// ���������� ������ ��� ���������� ����������
    QString getSelectedComponentLongName();

    /// ��������� ������������ ������� tabWidgetComponentInfo
    ///
    /// 0 - parameters
    /// 1 - state
    /// 2 - inputs
    /// 3 - outputs
    void openTabN(int n);

    /// ���������� ������ �������� ����
    int currentTabIndex();

    /// ���������� ��� ���������� Property
    QString getSelectedPropertyName();

    /// ���������� ����� ���������� ������
    int getSelectedChannelIndex();

    /// ����� ������ ������
    /// 0 - ������ ������ � ������� �������
    /// 1 - ������ � ���������� �������� �������
    void setChannelMode(int mode);

    /// ���������� ����� �������� ������
    /// ������������ ��� ����������� ����������
    int getWorkChannelIndex();

    /// ������������� ����������� �������
    void setEnableTabN(int n, bool enable);

    /// ������������� ����� ��������� ��� ������� ������ ������ �������
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

    /// ���������� ������� ��������� ���������� ����������
    void drawSelectedComponent(QModelIndex index);

    //������� ������������ ����
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

    /// ������� ��������� UpdateInterval, ��� ���������� �������� �� ����
    void setUpdateInterval(long value);

private:
    /// ������� �� ���������� ������ ���������� �������� �����
    std::string& EraseLeadEndls(std::string &value);

    /// ������� �� ���������� ������ ���������� � ����������� �������� �����
    std::string& EraseRangeEndls(std::string &value);

    /// ���� � ���������� ������ ���� ���� �� ���� ������� ������, �� �������� �����
    /// �� "[SEE BELOW]"
    std::string& PreparePropertyValueToListView(std::string &value);
private:

    /// ��� ����������, ��� �������� ����������
    QString currentDrawPropertyComponentName;

    /// ���������� ���������. ���������� �� currentDrawPropertyComponentName, ��� ��� ��������������
    /// ��� componentListItemSelectionChanged, ����� ������������ � currentDrawPropertyComponentName,
    /// ����� ������� ���������� ����������� ��� ������������� ��������� ������ ����������.
    QString selectedComponentLongName;

    // ����� ��������� ����� Property
    QString selectedParameterName;
    QString selectedStateName;
    QString selectedInputName;
    QString selectedOutputName;

    /// ������� ����� ��� �������
    int currentChannel;

    /// ����� ������ ������
    /// 0 - ������ ������ � ������� �������
    /// 1 - ������ � ���������� �������� �������
    int channelMode;

    /// ���� ��������� ���������� ������ ������
    bool channelsSelectionVisible;

    /// ��������� �� ��������� ����� TreeWidget � ������������ ����������� ��� ������� shift
    UComponentListTreeWidget *componentsTree;

    /// ��������� �������� ������������ �����
    QString currentDrawComponentName;

    /// ������� ����������� ����� ���������� ������ �����������
    void addComponentSons(QString componentName, QTreeWidgetItem *treeWidgetFather, QString oldRootItem, QString oldSelectedItem);

    /// ����������� ������� �� ������� �������
    void redrawChannelsList();

    Ui::UComponentsListWidget *ui;
};



#endif // UCOMPONENTSLISTWIDGET_H
