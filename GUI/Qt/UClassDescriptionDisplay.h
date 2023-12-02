#ifndef UCLASSDESCRIPTIONDISPLAY_H
#define UCLASSDESCRIPTIONDISPLAY_H

#include <QWidget>
#include <QMessageBox>
#include <QAction>
#include <QMainWindow>
#include <rdk_application.h>
#include "../../Core/Engine/UContainerDescription.h"
#include <UVisualControllerWidget.h>
//#include "UClassFavoritesEditor.h"

namespace Ui {
class UClassDescriptionDisplay;
}

class UClassFavoritesEditor;
/// Класс отображения описания конкретного класса
class UClassDescriptionDisplay : public UVisualControllerWidget
{
    Q_OBJECT
private:
    std::string ClassName;
    RDK::UEPtr<RDK::UContainerDescription> ClassDescription;
    std::pair<std::string, RDK::UPropertyDescription> CurrentProp;

    // Окно для создания Favorite
    UClassFavoritesEditor *clFavEditor;

public:
    explicit UClassDescriptionDisplay(std::string class_name, QWidget *parent = nullptr, RDK::UApplication *app = NULL);
    ~UClassDescriptionDisplay();

    const Ui::UClassDescriptionDisplay* GetUi() const;
public slots:
    // Сохранение описания класса
    void SaveDescription();

    // Закрытие
    void CloseForm();

    // Изменение класса для отобраежния описания
    void ChangeClassDescription(const std::string& class_name);

    // Заполнение списков свойств и Favorites класса
    void FillProperties();
    void FillFavorites();

    void UpdateDataSelectionType(int type);

    // Сброс GUI
    void DefaultGUIState();

private slots:

    // Открытие окна для создания Favorite
    void createNewFavoriteEditorSlot();


    void createNewFavorite(QString name, QString path);
    void deleteFavoriteSlot();

    void on_spinBoxDataSelecType_valueChanged(int arg1);
    void on_textEditHeaderProp_textChanged();
    void on_textEditDescProp_textChanged();
    void on_lineEditValList_textChanged(const QString &arg1);
    void on_lineEditStep_textChanged(const QString &arg1);
    void on_listWidgetProperties_currentTextChanged(const QString &currentText);

protected:
    virtual void hideEvent(QHideEvent *event) override;


private:
    Ui::UClassDescriptionDisplay *ui;
};

#endif // UCLASSDESCRIPTIONDISPLAY_H
