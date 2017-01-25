#ifndef UCOMPONENTSLISTWIDGET_H
#define UCOMPONENTSLISTWIDGET_H

#include "UVisualControllerWidget.h"
#include "UDrawEngineImageWidget.h"
#include "UPropertyXMLWidget.h"

#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QModelIndex>
#include <QApplication>
#include <QHeaderView>

namespace Ui {
class UComponentsListWidget;
}

/// ������ ��������� ���������� �� �����
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
    explicit UComponentsListWidget(QWidget *parent = 0, QString settingsFile = "settings.qt", QString settingsGroup = "UComponentsListWidget");
    virtual ~UComponentsListWidget();
    ///�������������� ������ �������� ������ (� ��������� � RDK)
    void AUpdateInterface();

    void setVerticalOrientation(bool vertical);
    QString getSelectedComponentLongName();
    QString getSelectedParameterName();
    QString getSelectedStateName();
    QString getSelectedInputName();
    QString getSelectedOutputName();

    // Moveup && MoveDown from keys
    //void keyPressEvent(QKeyEvent * event);

    /// ��������� ������������ ������� tabWidgetComponentInfo
    ///
    /// 0 - parameters
    /// 1 - state
    /// 2 - inputs
    /// 3 - outputs
    void openTabN(int n);

signals:
    void componentSelected(QString name); //single click
    void componentDoubleClick(QString name);
    void updateScheme(bool forceUpdate);

public slots:
    void updateComponentsListFromScheme();
    void componentSelectedFromScheme(QString name); //single click
    void componentDoubleClickFromScheme(QString name);
    void componentStapBackFromScheme();

    void componentListItemSelectionChanged();
    void reloadPropertys(bool forceReload = true);
    void parametersListSelectionChanged();
    void stateListSelectionChanged();
    void inputsListSelectionChanged();
    void outputsListSelectionChanged();

    void drawSelectedComponent(QModelIndex index);

    ///���������� ������ ��������
    void readSettings(QString file, QString group = "UComponentsListWidget");
    ///������ ������ ��������
    void writeSettings(QString file, QString group);

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

    //������ �������� ��� ������� ��� ����� ����������� ����������:
    //Parameters:
    void setParametersClicked();
    void setGlobalParametersClicked();
    void setGlobalOwnerParametersClicked();
    void defaultAllParametersClicked();
    void showParametersXMLClicked();
    //State:
    void setStateClicked();
    void setGlobalStateClicked();
    void setGlobalOwnerStateClicked();
    void showStateXMLClicked();
    //Inputs
    void setInputsClicked();
    void setGlobalInputsClicked();
    void setGlobalOwnerInputsClicked();
    void showInputsXMLClicked();
    //Outputs
    void setOutputsClicked();
    void setGlobalOutputsClicked();
    void setGlobalOwnerOutputsClicked();
    void showOutputsXMLClicked();

private:
    /// ������� �� ���������� ������ ���������� �������� �����
    std::string& EraseLeadEndls(std::string &value);

    /// ������� �� ���������� ������ ���������� � ����������� �������� �����
    std::string& EraseRangeEndls(std::string &value);

    /// ���� � ���������� ������ ���� ���� �� ���� ������� ������, �� �������� �����
    /// �� "[SEE BELOW]"
    std::string& PreparePropertyValueToListView(std::string &value);
private:
    QString settingsFileName;
    QString settingsGroupName;

    ///��� ����������, ��� �������� ����������
    QString currentDrawPropertyComponentName;

    // ����� ��������� ����� Property
    QString selectedParameterName;
    QString selectedStateName;
    QString selectedInputName;
    QString selectedOutputName;

    UComponentListTreeWidget *componentsTree;

    UPropertyXMLWidget *propertyXML;

    ///���������� ���������
    QString selectedComponentLongName;

    ///��������� �������� ������������ �����
    QString currentDrawComponentName;

    ///������� ����������� ����� ���������� ������ �����������
    void addComponentSons(QString componentName, QTreeWidgetItem *treeWidgetFather, QString oldRootItem, QString oldSelectedItem);
    Ui::UComponentsListWidget *ui;
};



#endif // UCOMPONENTSLISTWIDGET_H
