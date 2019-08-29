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
        void updateVetorComponent(void);



public slots:
        void slotSelectComponent();
        void slotSelectAllComponents();
        void slotDeleteAll();
        void slotDeleteCurrentItem();


private:
        Ui::UTableInfo* ui;

private:
    /// ������ ���� ���������, ������� ����� ���������� � �������
    //int componentNameVectr;
    std::vector<std::string> componentNameVector;

};

#endif
