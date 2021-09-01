#ifndef UCLASSFAVORITESEDITOR_H
#define UCLASSFAVORITESEDITOR_H

#include <QWidget>

#include <UVisualControllerWidget.h>
#include "USingleClassListWidget.h"

namespace Ui {
class UClassFavoritesEditor;
}
/// Класс для создания Favorite для описания определенного класса
class UClassFavoritesEditor : public UVisualControllerWidget
{
    Q_OBJECT

private:
    std::string ClassName;
    USingleClassListWidget *componentList;

public:
    explicit UClassFavoritesEditor(std::string class_name, QWidget *parent = nullptr, RDK::UApplication *app = NULL);
    ~UClassFavoritesEditor();

    void ChangeClass(std::string class_name);


signals:
    // Сигнал о создании нового Favorite
    void CreateNewFavorite(QString name, QString path);

private slots:
    void CreateFavorite();


private:
    Ui::UClassFavoritesEditor *ui;
};

#endif // UCLASSFAVORITESEDITOR_H
