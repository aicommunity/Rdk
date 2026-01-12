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
#include <QTreeWidgetItem>
#include <QString>
#include <QHash>

namespace Ui {
class UClassesListWidget;
}

// Методы группировки классов
enum class GroupingMethod {
    None = 0,              // Без группировки
    ByDescription = 1,     // По описанию
    ByInheritance = 2,     // По наследованию
    ByBaseComponent = 3    // По базовому компоненту
};

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

    // Обновляет поле библиотек и ставит библиотеку с именем lib_name выбраной
    void AUpdateLibsView(QString lib_name);

    void SetModelScheme(UDrawEngineImageWidget* model);

    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);

    void disable_cl_desc_popup_menu();

    void removeTab(int index);
public slots:
    void dragEvent(QModelIndex index);

signals:
    void classSelectionChanged();

private slots:

    // переключение фокуса библиотек во вкладке LibsCtrl
    void on_listWidgetRTlibs_itemSelectionChanged();

    // изменение некста поисковой строки
    void on_lineEditSearch_textChanged(const QString &arg1);

    // реакция разных вкладок в зависимости от их активности на изменение текста поисковой строки
    void tab0_textChanged(const QString &arg1);
    void tab1_textChanged(const QString &arg1);
    void tab2_textChanged(const QString &arg1);

    void on_treeWidgetStorageByLibs_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_listWidgetStorageByName_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidgetRTlibClasses_itemDoubleClicked(QListWidgetItem *item);

public slots:
    //События контекстного меню
    // создание/удаление библиотеки
    void CreateRTlibrary();
    void DeleteRTlibrary();

    //создание/удаление класса
    void AddNewClass(QString cur_lib = "");
    void DeleteClass();

    void on_tabWidget_currentChanged(int index);

    void on_action_cl_desc_triggered();

    // Слот для изменения метода группировки
    void on_comboBoxGroupingMethod_currentIndexChanged(int index);

private:
    UDrawEngineImageWidget* ModelScheme;
    Ui::UClassesListWidget *ui;

    // Методы группировки классов
    GroupingMethod GetCurrentGroupingMethod() const;
    QString GetClassGroup(const QString& className, GroupingMethod method) const;
    QString GroupByDescription(const QString& className) const;
    QString GroupByInheritance(const QString& className) const;
    QString GroupByBaseComponent(const QString& className) const;
    
    // Построение дерева с группировкой
    void BuildGroupedTree(const QString& searchText = "");
    
    // Кэш для результатов группировки (для оптимизации)
    mutable QHash<QString, QString> GroupingCache;
    
    // Получение tooltip для класса из class description
    QString GetClassTooltip(const QString& className) const;

};

// Диалоговое окно для создания новой библиотеки
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

// Диалоговое окно для создания нового класса
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
    bool GetReplace() const;
public slots:
    void ReplaceClicked();
    void ProcessInput();
};

// Диалоговое окно подтверждения удаления библиотеки/класса
class DeleteDialog: public QDialog
{
    Q_OBJECT
public:
     DeleteDialog(QString title, QString message, QWidget* pwgt = 0);

};
#endif // UCLASSESLISTWIDGET_H
