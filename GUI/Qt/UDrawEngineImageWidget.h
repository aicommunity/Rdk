#ifndef UDRAWENGINEIMAGEWIDGET_H
#define UDRAWENGINEIMAGEWIDGET_H

#include "rdk_application.h"
#include "UComponentsListWidget.h"
#include "UClassDescriptionDisplay.h"

#include <QLabel>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QDropEvent>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMenu>

/// UDrawEngineImageWidget class - ������ ��������� ����, ������� �� RDK::UDrawEngine
///
/// �������� ����������� ����, ���������� � ������� RDK::UDrawEngine,
/// �� �������� ����������� UVisualControllerWidget, ��� ���������,
/// �� ������������ � ����, ��� ����� � ����� �������������� ����� ���� (UDrawEngineWidget).
/// �������� ������� ������������ � ������ ����������� ������, �� �� ��������
/// ��������� �� ������, ������� ����������� � UGEngineControllWidget.

class UDrawEngineImageWidget : public QLabel
{
    Q_OBJECT

public:
    explicit UDrawEngineImageWidget(QWidget *parent = 0);
    virtual ~UDrawEngineImageWidget();

    //������� ����
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);

    void dragEvent();

    //���������� ����� ��� �������
    void resizeEvent(QResizeEvent*);

    /// ����������� ����� � ������ �������������� ������������ ����� �������� �����������
    void reDrawScheme(bool shouldReloadXml, bool no_resize_canvas=false);

    /// ������� ����� ��������������� ���������� � ����������� �����
    void setComponentName(QString name);

    /// ����� ���������� �� �����
    void selectComponent(QString name);

    /// ������ ������ �����
    void ResizeCanvas(void);

    /// ������������� ��������� �� ����
    void SetApplication(RDK::UApplication *app);

    /// ������������ ��� ������� ����������
    const std::string GetLongName();

    void classDescription(const std::string& class_name);

public slots:
    //����������� ����
    void componentViewOrBreakLink();
    void componentCreateLink();
    void componentFinishLink();
    void componentCancelLink();
    void componentStartMoving();
    void componentFinishMoving();
    void componentCancelMoving();
    void componentStartSwitching();
    void componentFinishSwitching();
    void componentCancelSwitching();
    void componentRename();
    void actionClassDescriptionTriggered();
    void componentDelete();
    void componentCopyNameToClipboard();
    void componentCopyLongNameToClipboard();
    void componentCopyClassNameToClipboard();
    void componentReset();
    void componentCalculate();
    void componentDefault();
    void componentGUI();
    void componentCopyXMLDescription();
    void componentCloneComponent();

signals:
    void componentSelected(QString name); //single click
    void componentDoubleClick(QString name);
    void componentStapBack();
    void updateComponentsList();

    void viewLinks(QString componentName);
    void createLinks(QString firstComponentName, QString secondComponentName);
    void switchLinks(QString firstComponentName, QString secondComponentName);

private:
    //��������� ������� ������������ ����
    QAction *actionViewOrBreakLink;
    QAction *actionCreateLink;
    QAction *actionFinishLink;
    QAction *actionCancelLink;
    QAction *actionStartMoving;
    QAction *actionFinishMoving;
    QAction *actionCancelMoving;
    QAction *actionSwitchLink;
    QAction *actionFinishSwitching;
    QAction *actionCancelSwitching;
    QAction *actionCloneComponent;

    ///��� ���������� ��� �������� �����/�����������
    QString firstComponentToConnection;

    ///��������� ��������� ���������� � ������
    void saveComponentPosition(std::string name);

    ///��������� ���������
    std::string selectedComponent;

    ///�����������
    QString startMoveComponent;
    QString endMoveComponent;

    ///������������
    QString startSwitchComponent;
    QString endSwitchComponent;

    ///Popup Menu
    QMenu *contextMenu;

    ///����� ������������ ������ ��� ���������� ����������
    QString myLongName();

    //<��� �� �������>
    /// ��� ����������, ���������� �������� ����� ����������
    QString ComponentName;

    /// ����� ��������� ��������� ����
    RDK::UDrawEngine DrawEngine;

    /// ����������� ������
    RDK::UGraphics Graph;

    /// ��� ������
    std::string FontType;

    /// ������ ������
    int FontSize;

    /// �����
    RDK::UBitmapFont Font;

    /// ����� ���������
    RDK::UBitmap GraphCanvas;

    /// Xml �������� ����
    RDK::USerStorageXML NetXml;
    //</��� �� �������>

    /// �������� �� ����
    RDK::UApplication* Application;

};

#endif // UDRAWENGINEIMAGEWIDGET_H
