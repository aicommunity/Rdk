#ifndef UCLASSESLISTWIDGET_H
#define UCLASSESLISTWIDGET_H

#include "UVisualControllerWidget.h"
#include "UDrawEngineImageWidget.h"
#include <QWidget>
#include <QDragMoveEvent>
#include <QMouseEvent>
#include <QModelIndex>
#include <QListWidget>

namespace Ui {
class UClassesListWidget;
}


/// UClassesListWidget class - виджет отображения списка доступных компонентов из UStorage
///
/// Содержит два списка отсортированных по имени:
/// - By Name - список всех компонент хранилища
/// - By Libs - древовидных список компонент по библиотекам
/// компоненты из списка можно перемещать на виджеты, принимающие dropEvent

class UClassesListWidget : public UVisualControllerWidget
{
    Q_OBJECT

public:
    explicit UClassesListWidget(QWidget *parent = 0, RDK::UApplication* app = NULL);
    virtual ~UClassesListWidget();

    QString selctedClass() const;

    virtual void AUpdateInterface(void);

public slots:
    void dragEvent(QModelIndex index);

signals:
    void classSelectionChanged();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    // переключение фокуса библиотек
    void on_listWidgetRTlibs_itemSelectionChanged();


public slots:
    //События контекстного меню
    // создание/удаление библиотеки
    void CreateRTlibrary();
    void DeleteRTlibrary();

    //создание/удаление класса
    void AddNewClass();
    void DeleteClass();

private:
    Ui::UClassesListWidget *ui;

};

#endif // UCLASSESLISTWIDGET_H
