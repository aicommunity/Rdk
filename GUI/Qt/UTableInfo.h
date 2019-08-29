#ifndef U_TABLE_INFO_WIDGET_H
#define U_TABLE_INFO_WIDGET_H

#include "UVisualControllerWidget.h"
#include "UStructSingleGraph.h"
#include "UGraphPaintWidget.h"
#include "UComponentPropertySelectionWidget.h"


namespace Ui {
class UTableInfo;
}

class UTableInfo : public UVisualControllerWidget
{
        Q_OBJECT

    public:
        /// Конструктор
        /// Задает врем обновления графика
        /// Создает в себе Виджет графика
        /// Связывает кнопки с действиями Виджета графика
        /// Определяет начальные параметры: размеры СК, подписи осей
        explicit UTableInfo(QWidget *parent = 0, RDK::UApplication* app = NULL);

        /// Деструктор
        ~UTableInfo();

        ///Обновляет график
        ///Обращаемся к ядру, берем матрицу
        ///Ее данные положим на этот график
        virtual void AUpdateInterface();

        /// Запись файла настроек
        virtual void ASaveParameters();
        /// Считывание файла настроек
        virtual void ALoadParameters();
        void updateVetorComponent(std::string compName);



public slots:
        void slotSelectComponent();
        void slotSelectAllComponents();
        void slotDeleteAll();
        void slotDeleteCurrentItem();


private:
        Ui::UTableInfo* ui;


    // Вектор имен элементов, которых нужно отображать в таблице
    //int componentNameVectr;
    //std::vector<std::string> componentNameVector;

};

#endif
