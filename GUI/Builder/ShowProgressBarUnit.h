#ifndef ShowProgressBarUnitH
#define ShowProgressBarUnitH

/* ***********************************************************
   @Copyright Alexander V. Bakhshiev, 2005-2009.
   E-mail:     alexab@ailab.ru
   url:     www.ailab.ru

   This file - part of the RDK


   version 2.0.0 25.06.2009
*********************************************************** */

#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TShowProgressBarForm : public TForm
{
__published:    // IDE-managed Components
        TProgressBar *ProgressBar1;
        TProgressBar *ProgressBar2;
        TStaticText *StaticText1;
        TStaticText *StaticText2;
        void __fastcall FormShow(TObject *Sender);
private:    // User declarations
public:        // User declarations
        __fastcall TShowProgressBarForm(TComponent* Owner);

public: // ������
        // ------------------------------------------
        // ������ ���������� ����� ������� ����������
        // ------------------------------------------
        // ���������� ��������� ����
        String __fastcall GetWinTitle(void);

        // ������������� ��������� ����
        void __fastcall SetWinTitle(String title);

        // ���������� ��������� ����� ���������
        String __fastcall GetBarHeader(unsigned int num);

        // ������������� ��������� ����� ���������
        void __fastcall SetBarHeader(unsigned int num, String title);

        // ���������� ��������� ���������� ������
        int __fastcall GetBarStatus(unsigned int num);

        // ����������� ��������� ���������� ������
        void __fastcall IncBarStatus(unsigned int num, int val=1);

        // ��������� ��������� ���������� ������
        void __fastcall DecBarStatus(unsigned int num, int val=1);

        // ���������� ��������� ���������� ������
        void __fastcall ResetBarStatus(unsigned int num, int currvalue, int total);
        // ------------------------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TShowProgressBarForm *ShowProgressBarForm;
//---------------------------------------------------------------------------
#endif
