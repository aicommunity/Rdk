/* ***********************************************************
   @Copyright Alexander V. Bakhshiev, 2005-2009.
   E-mail:     alexab@ailab.ru
   url:     www.ailab.ru

   This file - part of the RDK


   version 2.0.0 25.06.2009
*********************************************************** */

#include <vcl.h>
#pragma hdrstop

#include "UShowProgressBarUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUShowProgressBarForm *UShowProgressBarForm;
//---------------------------------------------------------------------------
__fastcall TUShowProgressBarForm::TUShowProgressBarForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
// Методы
// ------------------------------------------
// Методы управления окном статуса выполнения
// ------------------------------------------
// Возвращает заголовок окна
String __fastcall TUShowProgressBarForm::GetWinTitle(void)
{
// if(!Visible)
//  return "";

 return Caption;
}


// Устанавливает заголовок окна
void __fastcall TUShowProgressBarForm::SetWinTitle(String title)
{
// if(!Visible)
//  return;

 Caption=title;
 Repaint();
}

// Возвращает заголовки полос состояния
String __fastcall TUShowProgressBarForm::GetBarHeader(unsigned int num)
{
// if(!Visible)
//  return "";

 switch(num)
  {
  case 1:
   return StaticText1->Caption;

  case 2:
   return StaticText2->Caption;
  }
 return ""; 
}

// Устанавливает заголовки полос состояния
void __fastcall TUShowProgressBarForm::SetBarHeader(unsigned int num, String title)
{
// if(!Visible)
//  return;

 switch(num)
  {
  case 1:
   StaticText1->Caption=title;
  break;

  case 2:
   StaticText2->Caption=title;
  break;
  }
 Repaint();
}

// Возвращает состояние конкретной полосы
int __fastcall TUShowProgressBarForm::GetBarStatus(unsigned int num)
{
// if(!Visible)
//  return 0;

 switch(num)
  {
  case 1:
   return ProgressBar1->Position;

  case 2:
   return ProgressBar2->Position;
  }

 return 0; 
}

// Увеличивает состояние конкретной полосы
void __fastcall TUShowProgressBarForm::IncBarStatus(unsigned int num, int val)
{
// if(!Visible)
//  return;

 switch(num)
  {
  case 1:
   ProgressBar1->Position=ProgressBar1->Position+val;
  break;

  case 2:
   ProgressBar2->Position=ProgressBar2->Position+val;
  break;
  }
 ProgressBar1->Repaint();
 ProgressBar2->Repaint();
 ProgressBar1->Update();
 ProgressBar2->Update();
}

// Уменьшает состояние конкретной полосы
void __fastcall TUShowProgressBarForm::DecBarStatus(unsigned int num, int val)
{
// if(!Visible)
//  return;

 switch(num)
  {
  case 1:
   ProgressBar1->Position=ProgressBar1->Position-val;
  break;

  case 2:
   ProgressBar2->Position=ProgressBar2->Position-val;
  break;
  }
 ProgressBar1->Repaint();
 ProgressBar2->Repaint();
 ProgressBar1->Update();
 ProgressBar2->Update();
}

// Сбрасывает состояние конкретной полосы
void __fastcall TUShowProgressBarForm::ResetBarStatus(unsigned int num, int currvalue, int total)
{
// if(!Visible)
//  return;

 switch(num)
  {
  case 1:
   ProgressBar1->Max=total;
   ProgressBar1->Position=currvalue;
  break;

  case 2:
   ProgressBar2->Max=total;
   ProgressBar2->Position=currvalue;
  break;
  }
 ProgressBar1->Repaint();
 ProgressBar2->Repaint();
 ProgressBar1->Update();
 ProgressBar2->Update();
}
// ------------------------------------------
//---------------------------------------------------------------------------
void __fastcall TUShowProgressBarForm::FormShow(TObject *Sender)
{
// ProgressBar1->Position=0;
// ProgressBar2->Position=0;
 ProgressBar1->Repaint();
 ProgressBar2->Repaint();
}
//---------------------------------------------------------------------------
