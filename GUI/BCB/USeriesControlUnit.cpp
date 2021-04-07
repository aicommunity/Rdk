//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "USeriesControlUnit.h"
#include "UListInputFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUSeriesControlForm *USeriesControlForm;
//---------------------------------------------------------------------------
__fastcall TUSeriesControlForm::TUSeriesControlForm(TComponent* Owner)
        : TForm(Owner)
{
 StringGrid1->ColWidths[0]=StringGrid1->Width/7;
 StringGrid1->ColWidths[1]=StringGrid1->Width*6/7;
 StringGrid1->DefaultRowHeight=StringGrid1->Font->Size
								  * StringGrid1->Font->PixelsPerInch / 72+5;
 StringGrid2->ColWidths[0]=StringGrid2->Width/7;
 StringGrid2->ColWidths[1]=StringGrid2->Width*6/7;
 StringGrid2->DefaultRowHeight=StringGrid2->Font->Size
                                  * StringGrid1->Font->PixelsPerInch / 72+5;

 StringGrid1->Cells[0][0]="#";
 StringGrid1->Cells[1][0]="Легенда";
 StringGrid2->Cells[0][0]="#";
 StringGrid2->Cells[1][0]="Легенда";

 GrSender=0;

 GraphChanged=false;
 ColorsChanged=false;
 LegendsChanged=false;
 SeriesChanged=false;
 LineStyleChanged=false;
 LineWidthChanged=false;
}
//---------------------------------------------------------------------------
// Вспомогательные методы
// ------------------------
// Методы управления процессами добавления/удаления точек наблюдения
// ------------------------

// Обновление списков данных
void __fastcall TUSeriesControlForm::UpdateDataLists(void)
{
// map<int,TUWatchInfo>::iterator I,listI;
// TUWatchInfo* og;

 // Обновление списков...
 GrSender->GetVisibleList(WatchList);

 GrSender->GetInvisibleList(PossibleWatchList);
 // ...обновление списков завершено

 Edit1->Text=GrSender->GetXLabelTitle();
 if(Edit1->Text == "")
  {
   CheckBox1->Checked=false;
   Edit1->Enabled=false;
   Edit1->Color=clActiveBorder;
  }
 else
  {
   CheckBox1->Checked=true;
   Edit1->Enabled=true;
   Edit1->Color=clWindow;
  }

 Edit2->Text=GrSender->GetYLabelTitle();
 if(Edit2->Text == "")
  {
   CheckBox2->Checked=false;
   Edit2->Enabled=false;
   Edit2->Color=clActiveBorder;
  }
 else
  {
   CheckBox2->Checked=true;
   Edit2->Enabled=true;
   Edit2->Color=clWindow;
  }

 if(GrSender->GetXLabelVisible() == true)
  CheckBox3->Checked=true;
 else
  CheckBox3->Checked=false;

 if(GrSender->GetYLabelVisible() == true)
  CheckBox4->Checked=true;
 else
  CheckBox4->Checked=false;

 ComboBox3->ItemIndex=GrSender->GetLegendPosition();
}

// Обновляет информацию в окнах и состояние всех элементов управления
void __fastcall TUSeriesControlForm::UpdateInfo(void)
{
 map<int,TUWatchInfo>::iterator I;
// map<int,TUWatchInfo>::iterator listI;

 StringGrid1->RowCount=2;
 StringGrid1->Cells[0][StringGrid1->RowCount-1]="";
 StringGrid1->Cells[1][StringGrid1->RowCount-1]="";
 StringGrid2->RowCount=2;
 StringGrid2->Cells[0][StringGrid2->RowCount-1]="";
 StringGrid2->Cells[1][StringGrid2->RowCount-1]="";

 I=WatchList.begin();
 while(I != WatchList.end())
  {
   StringGrid2->Cells[0][StringGrid2->RowCount-1]=IntToStr(I->first);
   StringGrid2->Cells[1][StringGrid2->RowCount-1]=I->second.Legend.c_str();
   int width=StringGrid2->Canvas->TextWidth(I->second.Legend.c_str());
   if(StringGrid2->ColWidths[1]<width)
	StringGrid2->ColWidths[1]=width;

   StringGrid2->RowCount++;
   ++I;
  }
 if(StringGrid2->RowCount > 2)
  StringGrid2->RowCount--;

 I=PossibleWatchList.begin();
 while(I != PossibleWatchList.end())
  {
   StringGrid1->Cells[0][StringGrid1->RowCount-1]=IntToStr(I->first);
   StringGrid1->Cells[1][StringGrid1->RowCount-1]=I->second.Legend.c_str();
   int width=StringGrid1->Canvas->TextWidth(I->second.Legend.c_str());
   if(StringGrid1->ColWidths[1]<width)
	StringGrid1->ColWidths[1]=width;

   StringGrid1->RowCount++;
   ++I;
  }
 if(StringGrid1->RowCount > 2)
  StringGrid1->RowCount--;

 if(PossibleWatchList.empty())
  {
   Button1->Enabled=false;
   Button2->Enabled=false;
  }
 else
  {
   Button1->Enabled=true;
   Button2->Enabled=true;
  }

 if(WatchList.empty())
  {
   Button3->Enabled=false;
   Button4->Enabled=false;
  }
 else
  {
   Button3->Enabled=true;
   Button4->Enabled=true;
  }

 if(GrSender->GetLegendVisible())
  CheckBox5->Checked=false;
 else
  CheckBox5->Checked=true;

 WatchIntervalEdit->Text=FloatToStrF(GrSender->GetWatchInterval(),ffFixed,5,5);
 UpdateIntervalEdit->Text=IntToStr(int(GrSender->GetUpdateInterval()));

 AutoMinYValueCheckBox->Checked=GrSender->GetAutoMinYValue();
 AutoMaxYValueCheckBox->Checked=GrSender->GetAutoMaxYValue();

 UpdateSelectedWatch();
}


// Обновляет информацию по выбранной наблюдаемой точки съёма
void __fastcall TUSeriesControlForm::UpdateSelectedWatch(void)
{
 if(StringGrid2->Row < 1 || (StringGrid2->Row == 1 && StringGrid2->Cells[0][1] == ""))
  {
   Edit3->Text="";
   Edit4->Text="";
   GroupBox1->Enabled=false;
   Edit4->Color=clActiveBorder;
   ComboBox1->ItemIndex=-1;
   ComboBox2->ItemIndex=0;
  }
 else
  {
   GroupBox1->Enabled=true;
   int index=StrToInt(StringGrid2->Cells[0][StringGrid2->Row]);
   Edit3->Text=WatchList[index].Legend.c_str();
   Edit4->Text=FloatToStr(WatchList[index].YShift);
   Edit4->Color=clWindow;
   InnerCPanel->Color=WatchList[index].Color;
   ColorDialog1->Color=WatchList[index].Color;
   ComboBox1->ItemIndex=WatchList[index].Style;
   ComboBox2->ItemIndex=WatchList[index].LineWidth-1;
   InnerCPanel->Repaint();
  }
}
// ------------------------

// Методы
// ------------------------
// Методы управления формой
// ------------------------
// Метод, который должен быть вызван для открытия диалога
void __fastcall TUSeriesControlForm::Execute(TUWatchFrame* grsender)
{
 if(Visible)
  return;

 if(!grsender)
  return;

 GrSender=grsender;
 UpdateDataLists(); // Обновление списков
 UpdateInfo();

 GraphChanged=false;
 ColorsChanged=false;
 LegendsChanged=false;
 SeriesChanged=false;
 LineStyleChanged=false;
 LineWidthChanged=false;

 Show();
}
// ------------------------
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::OutterCPanelDblClick(TObject *Sender)
{
 if(!ColorDialog1->Execute())
  return;

 InnerCPanel->Color=ColorDialog1->Color;
 InnerCPanel->Repaint();
 InnerCPanel->Update();
 WatchList[StrToInt(StringGrid2->Cells[0][StringGrid2->Row])].Color
                   =ColorDialog1->Color;
 ColorsChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::InnerCPanelDblClick(TObject *Sender)
{
 OutterCPanelDblClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::FormHide(TObject *Sender)
{
 map<int,TUWatchInfo>::iterator I;

 GrSender=0;
 WatchList.clear();
 PossibleWatchList.clear();
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::CheckBox1Click(TObject *Sender)
{
 if(CheckBox1->Checked == true)
  {
   Edit1->Enabled=true;
   Edit1->Color=clWindow;
  }
 else
  {
   Edit1->Enabled=false;
   Edit1->Color=clActiveBorder;
  }
 GraphChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::CheckBox2Click(TObject *Sender)
{
 if(CheckBox2->Checked == true)
  {
   Edit2->Enabled=true;
   Edit2->Color=clWindow;
  }
 else
  {
   Edit2->Enabled=false;
   Edit2->Color=clActiveBorder;
  }
 GraphChanged=true;
}
//---------------------------------------------------------------------------



void __fastcall TUSeriesControlForm::StringGrid2Click(TObject *Sender)
{
 UpdateSelectedWatch();
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::Button1Click(TObject *Sender)
{
 map<int,TUWatchInfo>::iterator I;

 I=PossibleWatchList.begin();
 while(I != PossibleWatchList.end())
  {
   WatchList[I->first]=I->second;
   ++I;
  }
 PossibleWatchList.clear();
 UpdateInfo();
 SeriesChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::Button4Click(TObject *Sender)
{
 map<int,TUWatchInfo>::iterator I;

 I=WatchList.begin();
 while(I != WatchList.end())
  {
   PossibleWatchList[I->first]=I->second;
   ++I;
  }
 WatchList.clear();
 UpdateInfo();
 SeriesChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::Button2Click(TObject *Sender)
{
 if(StringGrid1->Row < 1 || (StringGrid1->Row == 1 && StringGrid1->Cells[0][1] == ""))
  return;

 map<int,TUWatchInfo>::iterator I;

 I=PossibleWatchList.find(StrToInt(StringGrid1->Cells[0][StringGrid1->Row]));
 WatchList[I->first]=I->second;
 PossibleWatchList.erase(I);
 UpdateInfo();
 SeriesChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::Button3Click(TObject *Sender)
{
 if(StringGrid2->Row < 1 || (StringGrid2->Row == 1 && StringGrid2->Cells[0][1] == ""))
  return;

 map<int,TUWatchInfo>::iterator I;

 I=WatchList.find(StrToInt(StringGrid2->Cells[0][StringGrid2->Row]));
 PossibleWatchList[I->first]=I->second;
 WatchList.erase(I);
 UpdateInfo();
 SeriesChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::StringGrid2DblClick(TObject *Sender)
{
 Button3Click(Sender);       
}
//---------------------------------------------------------------------------



void __fastcall TUSeriesControlForm::Edit3DblClick(TObject *Sender)
{
 map<int,TUWatchInfo>::iterator I;
 string s;
 vector<string> listvals;

 // Подготовка формы запроса легенды
 UListInputForm->PresentSelect=false;
 UListInputForm->MustInput=true;
 UListInputForm->Sort=false;

 I=WatchList.begin();
 while(I != WatchList.end())
  {
   listvals.push_back(I->second.Legend);
   ++I;
  }
 I=PossibleWatchList.begin();
 while(I != PossibleWatchList.end())
  {
   listvals.push_back(I->second.Legend);
   ++I;
  }

 s="Подпись для ";
 s+=AnsiString(StringGrid2->Cells[1][StringGrid2->Row]).c_str();
 UListInputForm->Init(s,listvals,"");
 // ...подготовка формы запроса легенды завершена
 if(UListInputForm->ShowModal() != mrOk)
  return;

 Edit3->Text=UListInputForm->Edit->Text;
 WatchList[StrToInt(StringGrid2->Cells[0][StringGrid2->Row])].Legend
				   =AnsiString(UListInputForm->Edit->Text).c_str();
 LegendsChanged=true;
}
//---------------------------------------------------------------------------
void __fastcall TUSeriesControlForm::BitBtn1Click(TObject *Sender)
{
 map<int,TUWatchInfo>::iterator I;

 if(SeriesChanged)
  {
   I=WatchList.begin();
   while(I != WatchList.end())
	{
	 GrSender->ChangeVisible(I->first, true);
	 ++I;
	}

   I=WatchList.begin();
   while(I != WatchList.end())
	{
	 GrSender->ChangeYShift(I->first, I->second.YShift);
	 ++I;
	}

   I=PossibleWatchList.begin();
   while(I != PossibleWatchList.end())
	{
	 GrSender->ChangeVisible(I->first, false);
	 ++I;
	}

   I=DelWatchList.end();
   if(I !=DelWatchList.begin())
    do
	{
	 --I;
	 GrSender->Del(I->first);
	}
	while(I != DelWatchList.begin());

  }
 else
  {
   if(ColorsChanged)
    {
     I=WatchList.begin();
     while(I != WatchList.end())
      {
	   GrSender->ChangeColor(I->first,I->second.Color);
	   ++I;
	  }
	}
   if(LegendsChanged)
	{
	 I=WatchList.begin();
	 while(I != WatchList.end())
	  {
	   GrSender->ChangeLegend(I->first,I->second.Legend);
	   ++I;
	  }
	}
   if(LineStyleChanged)
   {
	 I=WatchList.begin();
	 while(I != WatchList.end())
	  {
	   GrSender->ChangeLineStyle(I->first,I->second.Style);
	   ++I;
	  }
   }
   if(LineWidthChanged)
   {
	 I=WatchList.begin();
	 while(I != WatchList.end())
	  {
	   GrSender->ChangeLineWidth(I->first,I->second.LineWidth);
	   ++I;
	  }
   }
  }

 if(GraphChanged)
  {
   if(CheckBox2->Checked)
	GrSender->SetYLabelTitle(Edit2->Text);
   else
	GrSender->SetYLabelTitle("");

   if(CheckBox1->Checked)
	GrSender->SetXLabelTitle(Edit1->Text);
   else
	GrSender->SetXLabelTitle("");

   if(CheckBox3->Checked)
	GrSender->SetXLabelVisible(true);
   else
	GrSender->SetXLabelVisible(false);

   if(CheckBox4->Checked)
	GrSender->SetYLabelVisible(true);
   else
	GrSender->SetYLabelVisible(false);

   if(CheckBox5->Checked)
	GrSender->SetLegendVisible(false);
   else
	GrSender->SetLegendVisible(true);

   GrSender->SetLegendPosition(ComboBox3->ItemIndex);

   GrSender->SetWatchInterval(StrToFloat(WatchIntervalEdit->Text));
   GrSender->SetAutoMinYValue(AutoMinYValueCheckBox->Checked);
   GrSender->SetAutoMaxYValue(AutoMaxYValueCheckBox->Checked);
  }

 try
 {
  GrSender->SetUpdateInterval(StrToInt(UpdateIntervalEdit->Text));
 }
 catch(EConvertError &ex)
 {

 }
 Hide();
}
//---------------------------------------------------------------------------


void __fastcall TUSeriesControlForm::CheckBox3Click(TObject *Sender)
{
 GraphChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::CheckBox4Click(TObject *Sender)
{
 GraphChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::StringGrid1DblClick(TObject *Sender)
{
 Button2Click(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::Edit4KeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
 if(Key == VK_RETURN)
  {
   Edit4Exit(Sender);
  }
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::Edit4Exit(TObject *Sender)
{
 if(StringGrid2->Row < 1 || (StringGrid2->Row == 1 && StringGrid2->Cells[0][1] == ""))
  return;

 int index=-1;

 try
 {
  index=StrToInt(StringGrid2->Cells[0][StringGrid2->Row]);
 }
 catch(EConvertError &)
 {
  return;
 }

 if(index<0)
  return;

 double yshift=0.0;

 try
 {
  yshift=StrToFloat(Edit4->Text);
 }
 catch(EConvertError &)
 {
  return;
 }

 if(WatchList.find(index) == WatchList.end())
  return;

 WatchList[index].YShift=yshift;
 SeriesChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::Edit1Change(TObject *Sender)
{
 GraphChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::Edit2Change(TObject *Sender)
{
 GraphChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::BitBtn2Click(TObject *Sender)
{
 Hide();
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::CheckBox5Click(TObject *Sender)
{
 GraphChanged=true;
}
//---------------------------------------------------------------------------



void __fastcall TUSeriesControlForm::ComboBox1Select(TObject *Sender)
{
 if(StringGrid2->Row < 1 || (StringGrid2->Row == 1 && StringGrid2->Cells[0][1] == ""))
  return;

 WatchList[StrToInt(StringGrid2->Cells[0][StringGrid2->Row])].Style=TPenStyle(ComboBox1->ItemIndex);
 LineStyleChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::ComboBox2Select(TObject *Sender)
{
 if(StringGrid2->Row < 1 || (StringGrid2->Row == 1 && StringGrid2->Cells[0][1] == ""))
  return;

 WatchList[StrToInt(StringGrid2->Cells[0][StringGrid2->Row])].LineWidth=ComboBox2->ItemIndex+1;
 LineWidthChanged=true;
}
//---------------------------------------------------------------------------


void __fastcall TUSeriesControlForm::ComboBox3Select(TObject *Sender)
{
 GraphChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::DelPointButtonClick(TObject *Sender)
{
 if(StringGrid2->Row < 1 || (StringGrid2->Row == 1 && StringGrid2->Cells[0][1] == ""))
  return;

 map<int,TUWatchInfo>::iterator I;

 int del_index=StrToInt(StringGrid2->Cells[0][StringGrid2->Row]);
 I=WatchList.find(del_index);
 DelWatchList[I->first]=I->second;
 WatchList.erase(I);
 UpdateInfo();
 SeriesChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::WatchIntervalEditChange(TObject *Sender)
{
 GraphChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::FormShow(TObject *Sender)
{
 DelWatchList.clear();
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::AutoMinYValueCheckBoxClick(TObject *Sender)
{
 GraphChanged=true;
}
//---------------------------------------------------------------------------

void __fastcall TUSeriesControlForm::AutoMaxYValueCheckBoxClick(TObject *Sender)
{
 GraphChanged=true;
}
//---------------------------------------------------------------------------

