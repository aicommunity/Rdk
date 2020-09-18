#ifndef UCLASSESLISTWIDGET_H
#define UCLASSESLISTWIDGET_H

#include "UVisualControllerWidget.h"
#include "UDrawEngineImageWidget.h"
#include <QWidget>
#include <QDragMoveEvent>
#include <QMouseEvent>
#include <QModelIndex>
#include <QListWidget>
#include <QDialog>

namespace Ui {
class UClassesListWidget;
}


/// UClassesListWidget class - виджет отображени€ списка доступных компонентов из UStorage
///
/// —одержит два списка отсортированных по имени:
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

    // переключение фокуса библиотек во вкладке LibsCtrl
    void on_listWidgetRTlibs_itemSelectionChanged();

    // изменение некста поисковой строки
    void on_lineEditSearch_textChanged(const QString &arg1);

    // реакци€ разных вкладок в зависимости от их активности на изменение текста поисковой строки
    void tab0_textChanged(const QString &arg1);
    void tab1_textChanged(const QString &arg1);
    void tab2_textChanged(const QString &arg1);


public slots:
    //—обыти€ контекстного меню
    // создание/удаление библиотеки
    void CreateRTlibrary();
    void DeleteRTlibrary();

    //создание/удаление класса
    void AddNewClass();
    void DeleteClass();

private:

    Ui::UClassesListWidget *ui;

};

// ƒиалоговое окно дл€ создани€ новой библиотеки
class CrLibDialog: public QDialog
{
    Q_OBJECT
private:
    QLineEdit* InputLibName;
    QLabel* Message;

public:
    CrLibDialog(QWidget* pwgt = 0);

    const std::string GetLibName() const;

public slots:
    void ProcessInput();
};

// ƒиалоговое окно дл€ создани€ нового класса
class CrClassDialog: public QDialog
{
    Q_OBJECT
private:
    QLineEdit* InputClassName;
    QLineEdit* ComponentName;
    QLabel* Info;
    QLabel* MessageClass;
    QLabel* MessageComp;
    // нужно ли делать replace класса
    bool Replace;

public:
    CrClassDialog(QString lib_name, QWidget* pwgt = 0);
    const bool GetReplace() const;
    const std::string GetClassName() const;
    const std::string GetCompName() const;

public slots:
    void ProcessInput();
};

#endif // UCLASSESLISTWIDGET_H
