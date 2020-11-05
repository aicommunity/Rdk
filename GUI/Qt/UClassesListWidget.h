#ifndef UCLASSESLISTWIDGET_H
#define UCLASSESLISTWIDGET_H

#include "UVisualControllerWidget.h"
#include "UDrawEngineImageWidget.h"
#include <QWidget>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMouseEvent>
#include <QModelIndex>
#include <QListWidget>
#include <QDialog>
#include <QComboBox>
#include <QMessageBox>

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

    // ќбновл€ет поле библиотек и ставит библиотеку с именем lib_name выбраной
    void AUpdateLibsView(QString lib_name);

    void SetModelScheme(UDrawEngineImageWidget* model);

    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
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
    void AddNewClass(QString cur_lib = "");
    void DeleteClass();

private:
    UDrawEngineImageWidget* ModelScheme;
    Ui::UClassesListWidget *ui;

};

// ƒиалоговое окно дл€ создани€ новой библиотеки
class CrLibDialog: public QDialog
{
    Q_OBJECT
private:
    QLineEdit* InputLibName;
    QLabel* Message;
    QPushButton* AddButton;

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

    QLabel* MessageLib;
    QComboBox* Libraries;
    QLabel* MessageComp;
    QLineEdit* InputCompName;
    QLabel* MessageClass;
    QLineEdit* InputClassName;
    QPushButton* AddButton;
    QPushButton* ReplaceButton;
    bool Replace;
public:
    CrClassDialog(QStringList libs, QString cur_lib, QString cur_comp_name, QWidget* pwgt = 0);
    const std::string GetClassName() const;
    const std::string GetCompName() const;
    const std::string GetLibName() const;
    const bool GetReplace() const;
public slots:
    void ReplaceClicked();
    void ProcessInput();
};

// ƒиалоговое окно подтверждени€ удалени€ библиотеки/класса
class DeleteDialog: public QDialog
{
    Q_OBJECT
public:
     DeleteDialog(QString title, QString message, QWidget* pwgt = 0);

};
#endif // UCLASSESLISTWIDGET_H
