//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UWatchFrameUnit.h"
#include "UListInputFormUnit.h"
#include "USeriesControlUnit.h"
#include "UShowProgressBarUnit.h"
#include "UComponentsListFormUnit.h"
#include "TMatrixFormUnit.h"
//---------------------------------------------------------------------------
#define ISNAN(x) ((x) != (x))
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUWatchFrame *UWatchFrame;

//---------------------------------------------------------------------------
/*
   ������ ������ TUWatchInfo
*/
//---------------------------------------------------------------------------
TUWatchInfo::TUWatchInfo(void)
{
 Type=0;
 YShift=0;
 FullUpdate=false;
 WatchInterval=5;

 SeriesIndex=-1;
 Color=TColor(0);

 XYSize=0;
 Style=psDot;
 LineWidth=3;

 Visible=true;

 // ���������� ������, ��������� ������ �� ��� Y ��� ������ MDMatrix
 MRow=0;
 MCol=0;
 MVectorIndexX=0;
 MVectorIndexY=0;
}

TUWatchInfo::~TUWatchInfo(void)
{
}

TUWatchInfo::TUWatchInfo(const TUWatchInfo &wd)
{
 Type=wd.Type;
 YShift=wd.YShift;
 Color=wd.Color;
 Legend=wd.Legend;
 X=wd.X;
 Y=wd.Y;
 XYSize=wd.XYSize;
 FullUpdate=wd.FullUpdate;
 WatchInterval=wd.WatchInterval;
 SeriesIndex=wd.SeriesIndex;
 Style=wd.Style;
 LineWidth=wd.LineWidth;

 Visible=wd.Visible;

 XDataSourceName=wd.XDataSourceName;
 XOutputIndex=wd.XOutputIndex;
 YDataSourceName=wd.YDataSourceName;
 YOutputIndex=wd.YOutputIndex;
 MRow=wd.MRow;
 MCol=wd.MCol;
 MVectorName=wd.MVectorName;
 MVectorIndexX=wd.MVectorIndexX;
 MVectorIndexY=wd.MVectorIndexY;
}

TUWatchInfo& TUWatchInfo::operator = (const TUWatchInfo& wd)
{
 Type=wd.Type;
 YShift=wd.YShift;
 Color=wd.Color;
 Legend=wd.Legend;
 X=wd.X;
 Y=wd.Y;
 XYSize=wd.XYSize;
 FullUpdate=wd.FullUpdate;
 WatchInterval=wd.WatchInterval;
 SeriesIndex=wd.SeriesIndex;
 Visible=wd.Visible;
 Style=wd.Style;
 LineWidth=wd.LineWidth;

 XDataSourceName=wd.XDataSourceName;
 XOutputIndex=wd.XOutputIndex;
 YDataSourceName=wd.YDataSourceName;
 YOutputIndex=wd.YOutputIndex;
 MRow=wd.MRow;
 MCol=wd.MCol;

 MVectorName=wd.MVectorName;
 MVectorIndexX=wd.MVectorIndexX;
 MVectorIndexY=wd.MVectorIndexY;

 return *this;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TUWatchFrame::TUWatchFrame(TComponent* Owner)
        : TUVisualControllerFrame(Owner)
{
 SetLegendPosition(5);
 ModifyState=false;
 Chart1->BufferedDisplay=true;
 CacheSize=10;
 WatchInterval=-1;
 UpdateInterval=200;
 AlwaysUpdateFlag=true;
 CacheIndex=0;
 AutoMinYValue=true;
 AutoMaxYValue=true;
 SelectedSeriesIndex=-1;
}

__fastcall TUWatchFrame::~TUWatchFrame(void)
{
 Clear();
} 
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// ������
// ------------------------------
// ������ ���������� ����������
// ------------------------------
// ���������� 'true', ���� ������ � ������ ���� ��������,
// ��� ���� ����� ���� ���������/�������
bool __fastcall TUWatchFrame::GetModifyState(void)
{
 if(ModifyState)
  {
   ModifyState=false;
   return true;
  }

 return false;
}
// ------------------------------

// ------------------------------
// ������ ��������� �����������
// ------------------------------
// ��������/��������� ������������������� �� ��� X
void __fastcall TUWatchFrame::SetXAutoScale(bool value)
{
 if(value)
 {
  Chart1->TopAxis->Automatic=true;
  Chart1->BottomAxis->Automatic=true;
 }
 else
 {
  Chart1->TopAxis->Automatic=false;
  Chart1->BottomAxis->Automatic=false;
 }
}

// ���������� ��������� ������������������� �� ��� X
bool __fastcall TUWatchFrame::GetXAutoScale(void)
{
 return Chart1->TopAxis->Automatic;
}

// ��������/��������� ������������������� �� ��� Y
void __fastcall TUWatchFrame::SetYAutoScale(bool value)
{
 if(value)
 {
  Chart1->LeftAxis->Automatic=true;
  Chart1->RightAxis->Automatic=true;
 }
 else
 {
  Chart1->LeftAxis->Automatic=false;
  Chart1->RightAxis->Automatic=false;
 }
}

// ���������� ��������� ������������������� �� ��� Y
bool __fastcall TUWatchFrame::GetYAutoScale(void)
{
 return Chart1->LeftAxis->Automatic;
}

// ������������� ������� ��������� �� ��� X
void __fastcall TUWatchFrame::SetXMin(double value)
{
  Chart1->TopAxis->Minimum=value;
  Chart1->BottomAxis->Minimum=value;
}

void __fastcall TUWatchFrame::SetXMax(double value)
{
  Chart1->TopAxis->Maximum=value;
  Chart1->BottomAxis->Maximum=value;
}

// ������������� ������� ��������� �� ��� Y
void __fastcall TUWatchFrame::SetYMin(double value)
{
  Chart1->LeftAxis->Minimum=value;
  Chart1->RightAxis->Minimum=value;
}

void __fastcall TUWatchFrame::SetYMax(double value)
{
  Chart1->LeftAxis->Maximum=value;
  Chart1->RightAxis->Maximum=value;
}

// ���������� ������� ��������� �� ��� X
double __fastcall TUWatchFrame::GetXMin(void)
{
 return Chart1->TopAxis->Minimum;
}

double __fastcall TUWatchFrame::GetXMax(void)
{
 return Chart1->TopAxis->Maximum;
}

// ���������� ������� ��������� �� ��� Y
double __fastcall TUWatchFrame::GetYMin(void)
{
 return Chart1->LeftAxis->Minimum;
}

double __fastcall TUWatchFrame::GetYMax(void)
{
 return Chart1->LeftAxis->Maximum;
}

// ������������� ��������� �������
void __fastcall TUWatchFrame::SetGraphTitle(AnsiString title)
{
 Chart1->Title->Text->Clear();
 Chart1->Title->Text->Add(title);

 if(title.Length() == 0)
  Chart1->Title->Visible=false;
 else
  Chart1->Title->Visible=true;
}

// ������������� ��������� �������� �� ����
void __fastcall TUWatchFrame::SetXLabelVisible(bool value)
{
 Chart1->BottomAxis->Labels=value;
}

void __fastcall TUWatchFrame::SetYLabelVisible(bool value)
{
 Chart1->LeftAxis->Labels=value;
}

// ���������� ��������� �������� �� ����
bool __fastcall TUWatchFrame::GetXLabelVisible(void)
{
 return Chart1->BottomAxis->Labels;
}

bool __fastcall TUWatchFrame::GetYLabelVisible(void)
{
 return Chart1->LeftAxis->Labels;
}

// ������������� ������� �� ����
void __fastcall TUWatchFrame::SetXLabelTitle(AnsiString value)
{
 Chart1->BottomAxis->Title->Caption=value;
}

void __fastcall TUWatchFrame::SetYLabelTitle(AnsiString value)
{
 Chart1->LeftAxis->Title->Caption=value;
}

// ���������� ������� �� ����
AnsiString __fastcall TUWatchFrame::GetXLabelTitle(void)
{
 return Chart1->BottomAxis->Title->Caption;
}

AnsiString __fastcall TUWatchFrame::GetYLabelTitle(void)
{
 return Chart1->LeftAxis->Title->Caption;
}

// ������������� ��������� �������
void __fastcall TUWatchFrame::SetLegendVisible(bool value)
{
 Chart1->Legend->Visible=value;
 Chart1->Legend->DividingLines->Visible=false;
}

// ���������� ��������� �������
bool __fastcall TUWatchFrame::GetLegendVisible(void)
{
 return Chart1->Legend->Visible;
}

// ������������� ��������� �������
// 0 - �����
// 1 - ����� (�� �������)
// 2 - ������
// 3 - ������ (�� �������)
// 4 - ������
// 5 - �����
void __fastcall TUWatchFrame::SetLegendPosition(int value)
{
 switch(value)
 {
 case 0:
  Chart1->Legend->Alignment=laLeft;
  Chart1->Legend->ResizeChart=true;
 break;

 case 1:
  Chart1->Legend->Alignment=laLeft;
  Chart1->Legend->ResizeChart=false;
 break;

 case 2:
  Chart1->Legend->Alignment=laRight;
  Chart1->Legend->ResizeChart=true;
 break;

 case 3:
  Chart1->Legend->Alignment=laRight;
  Chart1->Legend->ResizeChart=false;
 break;

 case 4:
  Chart1->Legend->Alignment=laTop;
  Chart1->Legend->ResizeChart=true;
 break;

 case 5:
  Chart1->Legend->Alignment=laBottom;
  Chart1->Legend->ResizeChart=true;
 break;
 }
}

// ���������� ��������� �������
int __fastcall TUWatchFrame::GetLegendPosition(void)
{
 switch(Chart1->Legend->Alignment)
 {
 case laLeft:
  if(Chart1->Legend->ResizeChart)
   return 0;
  else
   return 1;

 case laRight:
  if(Chart1->Legend->ResizeChart)
   return 2;
  else
   return 3;

 case laTop:
  return 4;

 case laBottom:
  return 5;
 }
 return -1;
}

// ���������� �������� ����������
double __fastcall TUWatchFrame::GetWatchInterval(void)
{
 return WatchInterval;
}

// ������������� �������� ����������
void __fastcall TUWatchFrame::SetWatchInterval(double value)
{
 if(WatchInterval == value)
  return;

 WatchInterval=value;
 for(size_t i=0;i<NameList.size();i++)
  NameList[i].WatchInterval = WatchInterval;
}

// ������ ���� ������������ ������
int TUWatchFrame::GetCacheSize(void) const
{
 return CacheSize;
}

bool TUWatchFrame::SetCacheSize(int value)
{
 if(CacheSize == value)
  return true;

 if(value<0)
  return false;

 CacheSize=value;
 return true;
}

// ������������� ������������ ������� � ������ ������� ���
bool TUWatchFrame::GetAutoMinYValue(void) const
{
 return AutoMinYValue;
}

bool TUWatchFrame::GetAutoMaxYValue(void) const
{
 return AutoMaxYValue;
}

void TUWatchFrame::SetAutoMinYValue(bool value)
{
 AutoMinYValue=value;
 Chart1->LeftAxis->AutomaticMinimum=value;
}

void TUWatchFrame::SetAutoMaxYValue(bool value)
{
 AutoMaxYValue=value;
 Chart1->LeftAxis->AutomaticMaximum=value;
}
// ------------------------------

// ------------------------------
// ������ ���������� ������������
// ------------------------------
// ���������� ������ ����������
TUWatchInfo* __fastcall TUWatchFrame::Get(int seriesindex)
{
 if(seriesindex < 0 || seriesindex >= (int)NameList.size())
  return 0;

 return &NameList[seriesindex];
}

// ���������� ����� ����� ������ ����������
int __fastcall TUWatchFrame::GetNumWatches(void)
{
 return (int)NameList.size();
}


// ���������� ������ ����������
int __fastcall TUWatchFrame::Add(TUWatchInfo& wd)
{
 SelectedSeriesIndex=-1;

 // ���������, ���� �� ����� � ������ �� �������
 int seriesindex=-1;

 vector<TUWatchInfo>::iterator I;
 I=NameList.begin();
 int i=0;
 while(I != NameList.end())
  {
   if(I->YDataSourceName == wd.YDataSourceName &&
	I->YOutputIndex == wd.YOutputIndex && I->MCol == wd.MCol && I->MRow == wd.MRow)
	return i;

   ++I; ++i;
  }

 NameList.resize(NameList.size()+1);
 seriesindex=NameList.size()-1;
 NameList[seriesindex]=wd;

   RDK::UELockPtr<RDK::UEnvironment> env=RDK::GetEnvironmentLock();

   RDK::UControllerDataReader * data=env->RegisterDataReader(wd.YDataSourceName,wd.YOutputIndex,wd.MRow,wd.MCol);
   if(data)
   {
	data->SetTimeInterval(wd.WatchInterval);
   }

 // ��������� ����� ������
 TFastLineSeries *ser;

 ser=new TFastLineSeries(Chart1);
 ser->ParentChart=Chart1;
 ser->Title=wd.Legend.c_str();
 ser->ColorSource=wd.Color;
 ser->SeriesColor=wd.Color;
 ser->Pen->Style=wd.Style;
 ser->Pen->Width=wd.LineWidth;

 // ...������� ����� � �����
 StepUpdate();

// AddSeries(NameList.size()-1);

 // ... ��������� ���������...

 ModifyState=true;
 return seriesindex;
}

// ���������� ������ ���������� �� ����� ���������� � ������� ������
// ���������� ������ �����
int __fastcall TUWatchFrame::Add(int type, const string &xname, const string &yname, const string &xoutput, const string &youtput, int mrow, int mcol, double yshift, TPenStyle style, TColor color)
{
 SelectedSeriesIndex=-1;
 TUWatchInfo wd;
 wd.FullUpdate=false;
 //wd.WatchInterval=watchinterval;

 wd.YShift=yshift;
 wd.XOutputIndex=xoutput;

 wd.YOutputIndex=youtput;
 wd.MRow=mrow;
 wd.MCol=mcol;
 wd.Type=type;

 if(wd.Type == 0x200)
 {
   wd.Legend=yname+std::string(":")+youtput;
   wd.Legend+=string("(")+RDK::sntoa(mrow)+string(",");
   wd.Legend+=RDK::sntoa(mcol)+string(")");
 }
 else
 {
   wd.Legend=yname+std::string(":")+youtput;
 }

 if(color == 0) // ������ ����������� �����
  wd.Color=Chart1->GetFreeSeriesColor(true);
 else
  wd.Color=color;

 wd.XDataSourceName=xname;
 wd.YDataSourceName=yname;

 if(xname.empty())
  wd.XYSize=1;
 wd.Style=style;
 return Add(wd);
}


/*
int __fastcall TUWatchFrame::Add(int type, const string &xname, const string &yname, const string &xoutput, const string &youtput, const string &mvectorname, int mvectorindexx, int mvectorindexy, double yshift, TPenStyle style, TColor color)
{
 SelectedSeriesIndex=-1;
 TUWatchInfo wd;
 wd.FullUpdate=true;

 wd.YShift=yshift;
 wd.XOutputIndex=xoutput;

 wd.YOutputIndex=youtput;
 wd.MVectorName = mvectorname;
 wd.MVectorIndexX = mvectorindexx;
 wd.MVectorIndexY = mvectorindexy;
 wd.Type=type;

 if(wd.Type == 0x400)
 {
  if(!yname.empty())
  {
   wd.Legend=yname;
   wd.Legend+=string("(")+mvectorname+string(",");
   wd.Legend+=RDK::sntoa(mvectorindexx)+string(",");
   wd.Legend+=RDK::sntoa(mvectorindexy)+string(")");
  }
 }
 else
 {
 }

 if(color == 0) // ������ ����������� �����
  wd.Color=Chart1->GetFreeSeriesColor(true);
 else
  wd.Color=color;

 wd.XDataSourceName=xname;
 wd.YDataSourceName=yname;

 if(xname.empty())
  wd.XYSize=1;
 wd.Style=style;
 return Add(wd);
}
    */

// �������� ����������
void __fastcall TUWatchFrame::Del(int seriesindex)
{
 if(seriesindex >= (int)NameList.size())
  return;
 SelectedSeriesIndex=-1;

 TChartSeries *ser;

 ser=Chart1->Series[seriesindex];
 Chart1->RemoveSeries(ser);
 delete ser;
 // ...... ��������� �������

 vector<TUWatchInfo>::iterator I=NameList.begin();

 I+=seriesindex;
 NameList.erase(I);

 ModifyState=true;
}

// ������� ��� ����������
void __fastcall TUWatchFrame::Clear(void)
{
 SelectedSeriesIndex=-1;
 map<string,TUWatchInfo*>::iterator I;

 if(NameList.size() == 0)
  return;

// ClearSeries();
 TChartSeries *ser;

 for(int i=0;i<Chart1->SeriesCount();)
  {
   ser=Chart1->Series[i];
   Chart1->RemoveSeries(ser);
   delete ser;
   ModifyState=true;
  }

 // ...... ��������� �������
 NameList.clear();

 ModifyState=true;
}

// ��������� ����������� �����
// ������������ ��� ���������� ������
// ���� seriesindex < 0 �� ��������� ��� �����
void __fastcall TUWatchFrame::SeriesDisable(int seriesindex)
{
 if(seriesindex >= (int)NameList.size())
  return;

 if(seriesindex<0)
 {
  for(int i=0;i<Chart1->SeriesCount();i++)
   Chart1->Series[i]->Active=false;
 }
 else
  Chart1->Series[seriesindex]->Active=false;
}

// �������� ����������� �����
// ������������ ��� ���������� ������
// ���� seriesindex < 0 �� �������� ��� �����
void __fastcall TUWatchFrame::SeriesEnable(int seriesindex)
{
 if(seriesindex >= Chart1->SeriesCount())
  return;

 if(seriesindex<0)
 {
  for(int i=0;i<Chart1->SeriesCount();i++)
   Chart1->Series[i]->Active=true;
 }
 else
  Chart1->Series[seriesindex]->Active=true;
}

// ���������� ���������� �� 'stepcount' ��������� ����� ��������������
void __fastcall TUWatchFrame::StepUpdate(void)
{
 RDK::UELockPtr<RDK::UEnvironment> env=RDK::GetEnvironmentLock();
 for(int seriesindex=0; seriesindex<(int)NameList.size();seriesindex++)
 {
  TUWatchInfo *wd;

  // ������������ ���������� � ������
  wd=&NameList[seriesindex];

  if(Chart1->SeriesCount()<=seriesindex)
   continue;

  TChartSeries* series=Chart1->Series[seriesindex];

  if(!series)
   continue;

  static_cast<TFastLineSeries*>(series)->DrawAllPoints=true;

  if(!wd->Visible)
   continue;

   std::list<double>::iterator buffIX, buffIY;
   RDK::UControllerDataReader* data=env->GetDataReader(wd->YDataSourceName.c_str(), wd->YOutputIndex, wd->MRow, wd->MCol);
   if(!data)
	continue;

   int data_size=data->XData.size();

   wd->X.set_length(data_size);
   wd->Y.set_length(data_size);

   buffIX=data->XData.begin();
   buffIY=data->YData.begin();
   int i=0;
   for(;buffIX != data->XData.end();buffIX++,buffIY++,i++)
   {
	wd->X[i]=*buffIX;
	wd->Y[i]=*buffIY+wd->YShift;
   }

   wd->XYSize=data_size;

  // ������� ������ ���������� ������ ����������...
  static_cast<TFastLineSeries*>(series)->AutoRepaint=false;

  if(wd->XYSize>0 && wd->X.get_length() == wd->XYSize)
  {
   series->XValues->Value=wd->X;
   series->XValues->Count=wd->XYSize;
   series->XValues->Modified=true;

   series->YValues->Value=wd->Y;
   series->YValues->Count=wd->XYSize;
   series->YValues->Modified=true;
  }

  Chart1->BottomAxis->Automatic=true;

  static_cast<TFastLineSeries*>(series)->AutoRepaint=true;
  ModifyState=true;
 }
}
// ------------------------------

// ������� ������ ���� ����������
void __fastcall TUWatchFrame::Reset(void)
{
 // ������������ ���������� � ������
 for(int i=0;i<Chart1->SeriesCount();i++)
  {
   Chart1->Series[i]->Clear();
   if(GetWatchInterval()>0)
   {
	Chart1->BottomAxis->Minimum=0;
	Chart1->BottomAxis->Maximum=GetWatchInterval();
   }
   else
   {
	Chart1->BottomAxis->Minimum=0;
	Chart1->BottomAxis->Maximum=1;
	Chart1->BottomAxis->Automatic=true;
   }
   ModifyState=true;
  }
// TChartSeries *ser(0);
// for(int i=0;i<Chart1->SeriesCount();)
//  {
//   ser=Chart1->Series[i];
//   Chart1->RemoveSeries(ser);
//   delete ser;
//   ModifyState=true;
//  }

 CacheIndex=0;

 for(int seriesindex=0; seriesindex<(int)NameList.size();seriesindex++)
 {
  TUWatchInfo *wd;

  // ������������ ���������� � ������
  wd=&NameList[seriesindex];
 }
}


// ���������� ����� ������ ���� ����������� �����
void __fastcall TUWatchFrame::GetWatchList(map<int, TUWatchInfo> &buffer)
{
 buffer.clear();
 for(size_t i=0;i<NameList.size();i++)
  buffer[i]=NameList[i];
}

// ���������� ����� ������ ���� ������� �����
void __fastcall TUWatchFrame::GetVisibleList(map<int, TUWatchInfo> &buffer)
{
 buffer.clear();
 for(size_t i=0;i<NameList.size();i++)
  if(NameList[i].Visible)
   buffer[i]=NameList[i];
}

// ���������� ����� ������ ���� ��������� �����
void __fastcall TUWatchFrame::GetInvisibleList(map<int, TUWatchInfo> &buffer)
{
 buffer.clear();
 for(size_t i=0;i<NameList.size();i++)
  if(!NameList[i].Visible)
   buffer[i]=NameList[i];
}


// �������� ������� �� ��������� ������
void __fastcall TUWatchFrame::ChangeLegend(int seriesindex, string legend)
{
 if(seriesindex >= (int)NameList.size())
  return;

// ChangeSeriesLegend(seriesindex,legend);
 if(seriesindex >=Chart1->SeriesCount())
  return;

 Chart1->Series[seriesindex]->Title=legend.c_str();
 ModifyState=true;
 // ...��� ��������� ����������

 NameList[seriesindex].Legend=legend;
}

// �������� ���� ��������� �����
void __fastcall TUWatchFrame::ChangeColor(int seriesindex, TColor color)
{
 if(seriesindex >= (int)NameList.size())
  return;

// ChangeSeriesColor(seriesindex,color);
 if(seriesindex >=Chart1->SeriesCount())
  return;

 Chart1->Series[seriesindex]->ColorSource=color;
 Chart1->Series[seriesindex]->SeriesColor=color;
 Chart1->Series[seriesindex]->Pen->Color=color;
 ModifyState=true;
 // ...��� ��������� ����������

 NameList[seriesindex].Color=color;
}

// �������� ��� ����� ��������� �����
void __fastcall TUWatchFrame::ChangeLineStyle(int seriesindex, TPenStyle style)
{
 if(seriesindex >= (int)NameList.size())
  return;

 if(seriesindex >=Chart1->SeriesCount())
  return;

 Chart1->Series[seriesindex]->Pen->Style=style;
 ModifyState=true;
 // ...��� ��������� ����������

 NameList[seriesindex].Style=style;
}

// �������� ���������� � ��������� �����
void __fastcall TUWatchFrame::ChangeVisible(int seriesindex, bool visible)
{
 if(seriesindex < 0 || seriesindex >= (int)NameList.size())
  return;

 NameList[seriesindex].Visible=visible;
 Chart1->Series[seriesindex]->Active=visible;
}


// �������� �������� �� ��� Y
void __fastcall TUWatchFrame::ChangeYShift(int seriesindex, double yshift)
{
 if(seriesindex >= (int)NameList.size())
  return;

 if(seriesindex >=Chart1->SeriesCount())
  return;

 ModifyState=true;
 // ...��� ��������� ����������

 NameList[seriesindex].YShift=yshift;
}


// �������� ������� �����
void __fastcall  TUWatchFrame::ChangeLineWidth(int seriesindex, int width)
{
 if(seriesindex >= (int)NameList.size())
  return;

 if(seriesindex >=Chart1->SeriesCount())
  return;

 Chart1->Series[seriesindex]->Pen->Width=width;
 ModifyState=true;
 // ...��� ��������� ����������

 NameList[seriesindex].LineWidth=width;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// -----------------------------
// ������ ����������� ����������
// -----------------------------
// ��������� ������ �����
void __fastcall TUWatchFrame::TBSeriesModify(TObject *Sender)
{
 USeriesControlForm->Execute(this);
}

// ���������� ������� � ����
/*void __fastcall TWatchFrame::TBGraphSave(TObject *Sender)
{
 if(!SaveDialog1->Execute())
  return;

 Chart1->SaveToBitmapFile(SaveDialog1->FileName);
} */

// ��������� ����������� ������� � bmp
bool TUWatchFrame::SaveToBitmap(const AnsiString &filename)
{
 if(filename == "")
  return false;

 Chart1->SaveToBitmapFile(filename);
 return true;
}

// ��������� ����������� � ��������
bool TUWatchFrame::SaveToMetafile(const AnsiString &filename)
{
 if(filename == "")
  return false;

 Chart1->SaveToMetafileEnh(filename);
 return true;
}

// ��������� ����������� ������� � jpeg
bool TUWatchFrame::SaveToJpeg(const AnsiString &filename)
{
 if(filename == "")
  return false;

 TJPEGImage *jpg=new TJPEGImage;

 //Chart->CopyToClipboardBitmap();
 //jpg->LoadFromClipboardFormat(
 jpg->Assign(Chart1->BackImage->Bitmap);
 jpg->SaveToFile(filename);
/* TMemoryStream *stream=new TMemoryStream;

 stream->SetSize(Chart->Cop

 delete stream;*/
 delete jpg;

 return true;
}

// ��������� ����������� � ������� ���� �� �������
bool TUWatchFrame::Save(void)
{
 SavePictureDialog->DefaultExt="";
 SavePictureDialog->FileName="";
 if(!SavePictureDialog->Execute())
  return true;

 AnsiString ext=SavePictureDialog->FileName.SubString(SavePictureDialog->FileName.Length()-2,3);
 switch(SavePictureDialog->FilterIndex)
 {
 case 1:
  if(ext != "bmp")
   SavePictureDialog->FileName=SavePictureDialog->FileName+".bmp";
  SaveToBitmap(SavePictureDialog->FileName);
 break;

 case 2:
  if(ext != "emf")
   SavePictureDialog->FileName=SavePictureDialog->FileName+".emf";
  SaveToMetafile(SavePictureDialog->FileName);

 break;

 case 3:
  if(ext != "jpg")
   SavePictureDialog->FileName=SavePictureDialog->FileName+".jpg";
  SaveToJpeg(SavePictureDialog->FileName);
 break;
 }

 return true;
}
// -------------------------

// -----------------------------
// ������ ���������� ���������� �����������
// -----------------------------
// �����, ���������� ����� ������ ������
void TUWatchFrame::AAfterReset(void)
{
 Reset();
}

// ���������� ����������
void TUWatchFrame::AUpdateInterface(void)
{
 StepUpdate();
 Chart1->Repaint();
}

// ������� ���������� � �������� ���������
void TUWatchFrame::AClearInterface(void)
{
 Clear();
}

// ��������� ��������� ���������� � xml
void TUWatchFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
 char *out=0;
 int len;
 char buffer[32];
 string *sp;
 bool collectstate=false;

 if(NameList.size() == 0)
  return;

 String s=UShowProgressBarForm->GetBarHeader(1);
 UShowProgressBarForm->ResetBarStatus(1,0,NameList.size());

 xml.DelNodeInternalContent();
 // ��������� �� ������ ���� �������� �����
 for(int seriesindex=0;seriesindex<(int)NameList.size();seriesindex++)
  {
   UShowProgressBarForm->SetBarHeader(1,s+" - "+NameList[seriesindex].Legend.c_str()+":");
   xml.SelectNodeForce(RDK::sntoa(seriesindex));

   xml.WriteString("Legend",NameList[seriesindex].Legend);
   xml.WriteFloat("YShift",NameList[seriesindex].YShift);
   xml.WriteInteger("Color",NameList[seriesindex].Color);
   xml.WriteInteger("Style",NameList[seriesindex].Style);
   xml.WriteBool("Visible",NameList[seriesindex].Visible);
   xml.WriteInteger("LineWidth",NameList[seriesindex].LineWidth);
   xml.WriteString("XDataSourceName",NameList[seriesindex].XDataSourceName);
   xml.WriteString("XOutputIndexNew",NameList[seriesindex].XOutputIndex);
   xml.WriteString("YDataSourceName",NameList[seriesindex].YDataSourceName);
   xml.WriteString("YOutputIndexNew",NameList[seriesindex].YOutputIndex);
   xml.WriteInteger("XYSize", NameList[seriesindex].XYSize);
   xml.WriteFloat("WatchInterval", NameList[seriesindex].WatchInterval);

   xml.WriteInteger("MRow", NameList[seriesindex].MRow);
   xml.WriteInteger("MCol", NameList[seriesindex].MCol);
   xml.WriteInteger("Type", NameList[seriesindex].Type);

   if(collectstate)
	{
	 delete[] out;
	 out=new char[31*Chart1->Series[seriesindex]->Count()+1];
	 *out='\0';
	 len=0;
	 for(int k=0;k<Chart1->Series[seriesindex]->Count();k++)
	 {
	  *buffer='\0';
	  sprintf(buffer,"%f|",Chart1->Series[seriesindex]->XValue[k]);
	  memcpy(out+len,buffer,strlen(buffer));
	  len+=strlen(buffer);
	 }
	 out[len]='\0';
	 xml.WriteString("XDATA",out);

	 *out='\0';
	 len=0;
	 for(int k=0;k<Chart1->Series[seriesindex]->Count();k++)
	 {
	  *buffer='\0';
	  sprintf(buffer,"%f|",Chart1->Series[seriesindex]->YValue[k]);
	  memcpy(out+len,buffer,strlen(buffer));
	  len+=strlen(buffer);
	 }
	 out[len]='\0';
	 xml.WriteString("YDATA",out);
	}

   xml.SelectUp();
   UShowProgressBarForm->IncBarStatus(1);
  }

 xml.WriteFloat("WatchInterval",WatchInterval);
 xml.WriteBool("AutoMinYValue",AutoMinYValue);
 xml.WriteBool("AutoMaxYValue",AutoMaxYValue);

 ModifyState=false;
}

// ��������� ��������� ���������� �� xml
void TUWatchFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 map<string,TUWatchInfo*>::iterator I;
 TFastLineSeries *grseries;
 string xs,ys;
 double x,y;
 string::size_type ix1,ix2,iy1,iy2;
 bool collectstate=false;

 Clear();

 int num_series=xml.GetNumNodes();

 String s=UShowProgressBarForm->GetBarHeader(1);
 UShowProgressBarForm->ResetBarStatus(1,0,num_series);

 for(int i=0;i<num_series;i++)
  {
   xml.SelectNode(i);
   if(xml.GetNodeName() == "UpdateInterval" || xml.GetNodeName() == "WatchInterval" || xml.GetNodeName() == "ComponentControlName" || xml.GetNodeName() == "AlwaysUpdateFlag"
	   || xml.GetNodeName() == "AutoMinYValue"  || xml.GetNodeName() == "AutoMaxYValue")
   {
	xml.SelectUp();
	UShowProgressBarForm->IncBarStatus(1);
	continue;
   }
   UShowProgressBarForm->SetBarHeader(1,s+String(" - ")+String(xml.GetNodeName().c_str())+":");

   {
    TUWatchInfo *wd;
	TUWatchInfo wd_data;
	wd=&wd_data;
	wd->Legend=xml.ReadString("Legend","");
	wd->YShift=xml.ReadFloat("YShift",0);
	wd->Color=(TColor)xml.ReadInteger("Color",clTeeColor);
	wd->Style=(TPenStyle)xml.ReadInteger("Style",psSolid);
	wd->LineWidth=xml.ReadInteger("LineWidth",1);
	wd->XDataSourceName=xml.ReadString("XDataSourceName","");
	wd->XOutputIndex=xml.ReadString("XOutputIndexNew","");
	wd->YDataSourceName=xml.ReadString("YDataSourceName","");
	wd->YOutputIndex=xml.ReadString("YOutputIndexNew","");
	wd->XYSize=xml.ReadInteger("XYSize", 1);
	wd->Visible=xml.ReadBool("Visible",true);
	wd->WatchInterval=xml.ReadFloat("WatchInterval", 5);

	wd->MRow=xml.ReadInteger("MRow", 0);
	wd->MCol=xml.ReadInteger("MCol", 0);
	wd->Type=xml.ReadInteger("Type", 0);

	int grindex=Add(wd_data);
	grseries=dynamic_cast<TFastLineSeries*>(Chart1->Series[grindex]);
	if(collectstate)
	{
	 grseries->Clear();
	 ix1=0; ix2=0;
	 iy1=0; iy2=0;
	 xs=xml.ReadString("XDATA","").c_str();
	 ys=xml.ReadString("YDATA","").c_str();
	 grseries->Active=false;
	 while( ((ix2+1) < xs.size()) && ((ix2=xs.find_first_of('|',ix2+1)) != string::npos) )
	  {
	   if(iy2+1 >= ys.size())
		{
		 grseries->Clear();
		 break;
		}

	   iy2=ys.find_first_of('|',iy2+1);
//       x=StrToFloat(xs.substr(ix1,ix2-ix1).c_str());
//       y=StrToFloat(ys.substr(iy1,iy2-iy1).c_str());
	   x=atof(xs.substr(ix1,ix2-ix1).c_str());
	   y=atof(ys.substr(iy1,iy2-iy1).c_str());
	   grseries->AddXY(x,y,"",wd->Color);
	   ix1=ix2+1;
	   iy1=iy2+1;
	  }
	 grseries->Active=true;
	}
   }

   xml.SelectUp();
   UShowProgressBarForm->IncBarStatus(1);
  }

 SetWatchInterval(xml.ReadFloat("WatchInterval",5));
 SetAutoMinYValue(xml.ReadBool("AutoMinYValue",true));
 SetAutoMaxYValue(xml.ReadBool("AutoMaxYValue",true));
 ModifyState=false;
}
// -----------------------------

//---------------------------------------------------------------------------



void __fastcall TUWatchFrame::N1Click(TObject *Sender)
{
 TBSeriesModify(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TUWatchFrame::bmp1Click(TObject *Sender)
{
 Save();
}
//---------------------------------------------------------------------------


void __fastcall TUWatchFrame::AddXPulseWatch1Click(TObject *Sender)
{
 UComponentsListForm->ComponentsListFrame1->PageControl1->ActivePageIndex=2;
 if(UComponentsListForm->ShowIOSelect() != mrOk)
  return;

 std::string comp_name=UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 std::string comp_output=UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();

 int num_watches=GetNumWatches();
 int yshift=num_watches+1;

 Add(0x100, comp_name, "", comp_output, "", 0, 0, yshift,psSolid, TColor(0));
}
//---------------------------------------------------------------------------

void __fastcall TUWatchFrame::DeleteAll1Click(TObject *Sender)
{
 if(Application->MessageBox(L"Delete all watches! Are you sure?",L"Warining",MB_YESNO) == ID_YES)
  Clear();
}
//---------------------------------------------------------------------------

void __fastcall TUWatchFrame::AddTimeMatrixWatch1Click(TObject *Sender)
{
 UComponentsListForm->ComponentsListFrame1->PageControl1->ActivePageIndex=2;
 if(UComponentsListForm->ShowIOSelect() != mrOk)
  return;

//  const RDK::MDMatrix<double> *ym_double=0;
//  const RDK::MDMatrix<int> *ym_int=0;
//  ym_double=(const RDK::MDMatrix<double>*)(Model_GetComponentOutputAsMatrix(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(), UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput().c_str()));


 int type=0x0;
  if(MatrixForm->SelectMatrix(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName(),
								UComponentsListForm->ComponentsListFrame1->GetSelectedComponentPropertyName()))
  {
   type=0x200;
   if(MatrixForm->ShowModal() != mrOk)
	return;
  }
   /*
  UListInputForm->PresentSelect=true;
  UListInputForm->MustInput=true;
  vector<string> listvals;
  listvals.resize(ym->GetRows());
  for(size_t i=0;i<listvals.size();i++)
   listvals[i]=RDK::sntoa(i,2);
  UListInputForm->Init("Input Row Number",listvals,"0");
  if(UListInputForm->ShowModal() != mrOk)
   return;
  int row=StrToInt(UListInputForm->Edit->Text);

  listvals.resize(ym->GetCols());
  for(size_t i=0;i<listvals.size();i++)
   listvals[i]=RDK::sntoa(i,2);
  UListInputForm->Init("Input Col Number",listvals,"0");
  if(UListInputForm->ShowModal() != mrOk)
   return;
  int col=StrToInt(UListInputForm->Edit->Text);
  */
  int col=MatrixForm->SelectedCol;
  int row=MatrixForm->SelectedRow;
  std::string componentName = UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
  std::string componentOutput = UComponentsListForm->ComponentsListFrame1->GetSelectedComponentPropertyName();
 Add(type, "",componentName,"",componentOutput,row,col);
}
//---------------------------------------------------------------------------



void __fastcall TUWatchFrame::DeleteActiveWatch1Click(TObject *Sender)
{
 if(SelectedSeriesIndex < 0)
 {
  Application->MessageBox(L"Nothing to do! Please select series first.",L"Error", MB_OK);
  return;
 }

 String str=L"Are you sure to delete ";
 str=str+NameList[SelectedSeriesIndex].Legend.c_str();
 str=str+" series?";
 if(Application->MessageBox(str.c_str(),L"Warning", MB_YESNO) == ID_NO)
  return;

 Del(SelectedSeriesIndex);
 SelectedSeriesIndex=-1;
}
//---------------------------------------------------------------------------

void __fastcall TUWatchFrame::Chart1ClickSeries(TCustomChart *Sender, TChartSeries *Series,
          int ValueIndex, TMouseButton Button, TShiftState Shift, int X,
          int Y)
{
 SelectedSeriesIndex=Series->SeriesIndex;
}
//---------------------------------------------------------------------------


