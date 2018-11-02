#ifndef UDRAWENGINEIMAGEWIDGET_H
#define UDRAWENGINEIMAGEWIDGET_H

#include "rdk_application.h"
#include "UComponentsListWidget.h"

#include <QLabel>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMenu>

/// UDrawEngineImageWidget class - виджет отрисовки сети, основан на RDK::UDrawEngine
///
/// Содержит изображение сети, строящееся с помощью RDK::UDrawEngine,
/// не является наследником UVisualControllerWidget, как следствие,
/// не записывается в ядро, вся связь с ядром осуществляется через отца (UDrawEngineWidget).
/// Содержит сигналы адресованные к списку компонентов модели, но не содержит
/// указателя на список, сигналы связываются в UGEngineControllWidget.

class UDrawEngineImageWidget : public QLabel
{
    Q_OBJECT

public:
    explicit UDrawEngineImageWidget(QWidget *parent = 0);
    virtual ~UDrawEngineImageWidget();

    //События мыши
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);

    //расширени схемы при ресайзе
    void resizeEvent(QResizeEvent*);

    ///перерисовка схему с флагом принудительной перезагрузки схемы текущего отображения
    void reDrawScheme(bool shouldReloadXml);
    ///Задание имени отрисовываемого компонента и перерисовка схема
    void setComponentName(QString name);
    ///Выбор компонента на схеме
    void selectComponent(QString name);

public slots:
    //Контекстное меню
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
    void componentDelete();
    void componentCopyNameToClipboard();
    void componentCopyLongNameToClipboard();
    void componentCopyClassNameToClipboard();
    void componentReset();
    void componentCalculate();
    void componentGUI();
    void componentCopyXMLDescription();

signals:
    void componentSelected(QString name); //single click
    void componentDoubleClick(QString name);
    void componentStapBack();
    void updateComponentsList();

    void viewLinks(QString componentName);
    void createLinks(QString firstComponentName, QString secondComponentName);
    void switchLinks(QString firstComponentName, QString secondComponentName);

private:
    //некоторые события контекстного меню
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


    ///имя компонента для создания связй/перемещения
    QString firstComponentToConnection;

    ///сохраняет положение компонента в модели
    void saveComponentPosition(std::string name);

    ///Выбранный компонент
    std::string selectedComponent;

    ///Перемещение
    QString startMoveComponent;
    QString endMoveComponent;

    ///Переключение
    QString startSwitchComponent;
    QString endSwitchComponent;

    ///Popup Menu
    QMenu *contextMenu;

    ///Метод возвращающий полное имя выбранного компонента
    QString myLongName();

    //<код из билдера>
    /// Имя компонента, содержимое которого будет отображено
    QString ComponentName;

    /// Класс рисования структуры сети
    RDK::UDrawEngine DrawEngine;

    /// Графический движок
    RDK::UGraphics Graph;

    /// Тип шрифта
    std::string FontType;

    /// Размер шрифта
    int FontSize;

    /// Шрифт
    RDK::UBitmapFont Font;

    /// Канва рисования
    RDK::UBitmap GraphCanvas;

    /// Xml описание сети
    RDK::USerStorageXML NetXml;
    //</код из билдера>

};

#endif // UDRAWENGINEIMAGEWIDGET_H
