#ifndef U_TABLE_INFO_WIDGET_H
#define U_TABLE_INFO_WIDGET_H

#include "UVisualControllerWidget.h"
#include "UStructSingleGraph.h"
#include "UGraphPaintWidget.h"
#include "UComponentPropertySelectionWidget.h"


namespace Ui {
class UTableInfo;
}

///������ ��������� ������� � �������� ���������� ��������� ����������� � ��������� �� �������������
class UTableInfo : public UVisualControllerWidget
{
        Q_OBJECT

    public:
        /// �����������
        /// ������ ���� ���������� �������
        /// ������� � ���� ������ �������
        /// ��������� ������ � ���������� ������� �������
        /// ���������� ��������� ���������: ������� ��, ������� ����
        explicit UTableInfo(QWidget *parent = 0, RDK::UApplication* app = NULL);

        /// ����������
        ~UTableInfo();

        ///��������� ������
        ///���������� � ����, ����� �������
        ///�� ������ ������� �� ���� ������
        virtual void AUpdateInterface();

        /// ������ ����� ��������
        virtual void ASaveParameters();
        /// ���������� ����� ��������
        virtual void ALoadParameters();
        void updateVetorComponent(std::string compName);



public slots:
        ///����� ����������, ���������� � ������� ���������� �������� ���������� �������� � �������
        void slotSelectComponent();
        ///����� ������� �����������
        void slotSelectAllComponents();
        /// �������� ���� �� ������� ����������� �� �������
        /// �������: Model, Overhead, Full Step, Gui
        void slotDeleteAll();
        ///����� ����������, ���������� � ������� ���������� ���� ��� ��������� ������ �������� ���������� �������� � �������
        /// ���� ��������� �� ������� � ���� ������ �����������. ������ �� ����������
        void slotAddGroupComponents();



private:
        Ui::UTableInfo* ui;


    // ������ ���� ���������, ������� ����� ���������� � �������
    //int componentNameVectr;
    //std::vector<std::string> componentNameVector;

};

#endif
