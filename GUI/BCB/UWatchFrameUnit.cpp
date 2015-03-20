//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UWatchFrameUnit.h"
#include "UListInputFormUnit.h"
#include "USeriesControlUnit.h"
#include "UShowProgressBarUnit.h"
#include "UComponentsListFormUnit.h"
#include "rdk_initdll.h"
#include "myrdk.h"
//---------------------------------------------------------------------------
#define ISNAN(x) ((x) != (x))
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUWatchFrame *UWatchFrame;

//---------------------------------------------------------------------------
/*
   Методы класса TUWatchInfo
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

 X=0;
 Y=0;
 XYSize=0;
 Style=psDot;
 LineWidth=3;

 Visible=true;

 XOutputIndexOld=0;
 XOutputElementIndex=0;
 YOutputIndexOld=0;
 YOutputElementIndex=0;

 // Координаты выхода, хранящего данные по оси Y для случая MDMatrix
 MRow=0;
 MCol=0;
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
 XOutputIndexOld=wd.XOutputIndexOld;
 XOutputElementIndex=wd.XOutputElementIndex;
 YDataSourceName=wd.YDataSourceName;
 YOutputIndex=wd.YOutputIndex;
 YOutputIndexOld=wd.YOutputIndexOld;
 YOutputElementIndex=wd.YOutputElementIndex;
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
 XOutputIndexOld=wd.XOutputIndexOld;
 XOutputElementIndex=wd.XOutputElementIndex;
 YDataSourceName=wd.YDataSourceName;
 YOutputIndex=wd.YOutputIndex;
 YOutputIndexOld=wd.YOutputIndexOld;
 YOutputElementIndex=wd.YOutputElementIndex;
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
}

__fastcall TUWatchFrame::~TUWatchFrame(void)
{
 Clear();
} 
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Методы
// ------------------------------
// Методы управления состоянием
// ------------------------------
// Возвращает 'true', если данные в сериях были изменены,
// или если серии были добавлены/удалены
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
// Методы настройки отображения
// ------------------------------
// Включает/отключает автомасштабирование по оси X
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

// Возвращает состояние автомасштабирования по оси X
bool __fastcall TUWatchFrame::GetXAutoScale(void)
{
 return Chart1->TopAxis->Automatic;
}

// Включает/отключает автомасштабирование по оси Y
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

// Возвращает состояние автомасштабирования по оси Y
bool __fastcall TUWatchFrame::GetYAutoScale(void)
{
 return Chart1->LeftAxis->Automatic;
}

// Устанавливает границы изменения по оси X
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

// Устанавливает границы изменения по оси Y
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

// Возвращает границы изменения по оси X
double __fastcall TUWatchFrame::GetXMin(void)
{
 return Chart1->TopAxis->Minimum;
}

double __fastcall TUWatchFrame::GetXMax(void)
{
 return Chart1->TopAxis->Maximum;
}

// Возвращает границы изменения по оси Y
double __fastcall TUWatchFrame::GetYMin(void)
{
 return Chart1->LeftAxis->Minimum;
}

double __fastcall TUWatchFrame::GetYMax(void)
{
 return Chart1->LeftAxis->Maximum;
}

// Устанавливает заголовок графика
void __fastcall TUWatchFrame::SetGraphTitle(AnsiString title)
{
 Chart1->Title->Text->Clear();
 Chart1->Title->Text->Add(title);

 if(title.Length() == 0)
  Chart1->Title->Visible=false;
 else
  Chart1->Title->Visible=true;
}

// Устанавливает видимость подписей по осям
void __fastcall TUWatchFrame::SetXLabelVisible(bool value)
{
 Chart1->BottomAxis->Labels=value;
}

void __fastcall TUWatchFrame::SetYLabelVisible(bool value)
{
 Chart1->LeftAxis->Labels=value;
}

// Возвращает видимость подписей по осям
bool __fastcall TUWatchFrame::GetXLabelVisible(void)
{
 return Chart1->BottomAxis->Labels;
}

bool __fastcall TUWatchFrame::GetYLabelVisible(void)
{
 return Chart1->LeftAxis->Labels;
}

// Устанавливает подписи по осям
void __fastcall TUWatchFrame::SetXLabelTitle(AnsiString value)
{
 Chart1->BottomAxis->Title->Caption=value;
}

void __fastcall TUWatchFrame::SetYLabelTitle(AnsiString value)
{
 Chart1->LeftAxis->Title->Caption=value;
}

// Возвращает подписи по осям
AnsiString __fastcall TUWatchFrame::GetXLabelTitle(void)
{
 return Chart1->BottomAxis->Title->Caption;
}

AnsiString __fastcall TUWatchFrame::GetYLabelTitle(void)
{
 return Chart1->LeftAxis->Title->Caption;
}

// Устанавливает видимость легенды
void __fastcall TUWatchFrame::SetLegendVisible(bool value)
{
 Chart1->Legend->Visible=value;
 Chart1->Legend->DividingLines->Visible=false;
}

// Возвращает видимость легенды
bool __fastcall TUWatchFrame::GetLegendVisible(void)
{
 return Chart1->Legend->Visible;
}

// Устанавливает положение легенды
// 0 - Слева
// 1 - Слева (на графике)
// 2 - Справа
// 3 - Справа (на графике)
// 4 - Сверху
// 5 - Снизу
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

// Возвращает положение легенды
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

// Возвращает интервал наблюдения
double __fastcall TUWatchFrame::GetWatchInterval(void)
{
 return WatchInterval;
}

// Устанавливает интервал наблюдения
void __fastcall TUWatchFrame::SetWatchInterval(double value)
{
 if(WatchInterval == value)
  return;

 WatchInterval=value;
 for(size_t i=0;i<NameList.size();i++)
  NameList[i].WatchInterval = WatchInterval;
}

// Размер кеша отображаемых данных
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
// ------------------------------

// ------------------------------
// Методы управления наблюдениями
// ------------------------------
// Возвращает данные наблюдения
TUWatchInfo* __fastcall TUWatchFrame::Get(int seriesindex)
{
 if(seriesindex < 0 || seriesindex >= (int)NameList.size())
  return 0;

 return &NameList[seriesindex];
}

// Возвращает общее число данных наблюдения
int __fastcall TUWatchFrame::GetNumWatches(void)
{
 return (int)NameList.size();
}


// Добавление нового наблюдения
int __fastcall TUWatchFrame::Add(TUWatchInfo& wd)
{
 if(wd.XOutputIndex.empty() || wd.YOutputIndex.empty())
 {
 // Проверяем, есть ли серия с такими же данными
 int seriesindex=-1;

 vector<TUWatchInfo>::iterator I;
 I=NameList.begin();
 int i=0;
 while(I != NameList.end())
  {
   if((wd.Type != 0x200 && wd.Type !=0x400 && ((wd.Y && (wd.Y == I->Y)) ||
   (I->XDataSourceName == wd.XDataSourceName && I->XOutputIndexOld == wd.XOutputIndexOld &&
	I->XOutputElementIndex == wd.XOutputElementIndex &&
	I->YDataSourceName == wd.YDataSourceName && I->YOutputIndexOld == wd.YOutputIndexOld &&
	I->YOutputElementIndex == wd.YOutputElementIndex))) ||
	(wd.Type == 0x200 && I->YDataSourceName == wd.YDataSourceName && wd.MRow == I->MRow && wd.MCol == I->MCol && wd.Y == I->Y))
	return i;

   if(wd.Type == 0x400 && I->YDataSourceName == wd.YDataSourceName && wd.MVectorName == I->MVectorName && wd.MVectorIndexX == I->MVectorIndexX && wd.MVectorIndexY == I->MVectorIndexY)
	return i;
   ++I; ++i;
  }

 NameList.resize(NameList.size()+1);
 seriesindex=NameList.size()-1;
 NameList[seriesindex]=wd;


 // Добавляем новый график
 TFastLineSeries *ser;

 ser=new TFastLineSeries(Chart1);
 ser->ParentChart=Chart1;
 ser->Title=wd.Legend.c_str();
 ser->ColorSource=wd.Color;
 ser->SeriesColor=wd.Color;
 ser->Pen->Style=wd.Style;
 ser->Pen->Width=wd.LineWidth;

 // ...заносим точки в серию
 StepUpdate();

// AddSeries(NameList.size()-1);

 // ... добавляем остальное...

 ModifyState=true;
 return seriesindex;
 }
 else
 {
 // Проверяем, есть ли серия с такими же данными
 int seriesindex=-1;

 vector<TUWatchInfo>::iterator I;
 I=NameList.begin();
 int i=0;
 while(I != NameList.end())
  {
   if((wd.Type != 0x200 && wd.Type !=0x400 && ((wd.Y && (wd.Y == I->Y)) ||
   (I->XDataSourceName == wd.XDataSourceName && I->XOutputIndex == wd.XOutputIndex &&
	I->XOutputElementIndex == wd.XOutputElementIndex &&
	I->YDataSourceName == wd.YDataSourceName && I->YOutputIndex == wd.YOutputIndex &&
	I->YOutputElementIndex == wd.YOutputElementIndex))) ||
	(wd.Type == 0x200 && I->YDataSourceName == wd.YDataSourceName && wd.MRow == I->MRow && wd.MCol == I->MCol && wd.Y == I->Y))
	return i;

   if(wd.Type == 0x400 && I->YDataSourceName == wd.YDataSourceName && wd.MVectorName == I->MVectorName && wd.MVectorIndexX == I->MVectorIndexX && wd.MVectorIndexY == I->MVectorIndexY)
	return i;
   ++I; ++i;
  }

 NameList.resize(NameList.size()+1);
 seriesindex=NameList.size()-1;
 NameList[seriesindex]=wd;


 // Добавляем новый график
 TFastLineSeries *ser;

 ser=new TFastLineSeries(Chart1);
 ser->ParentChart=Chart1;
 ser->Title=wd.Legend.c_str();
 ser->ColorSource=wd.Color;
 ser->SeriesColor=wd.Color;
 ser->Pen->Style=wd.Style;
 ser->Pen->Width=wd.LineWidth;

 // ...заносим точки в серию
 StepUpdate();

// AddSeries(NameList.size()-1);

 // ... добавляем остальное...

 ModifyState=true;
 return seriesindex;
 }
}

// Добавление нового наблюдения по имени компонента и индексу выхода
// Возвращает индекс серии
int __fastcall TUWatchFrame::Add(int type, const string &xname, const string &yname, const string &xoutput, int xoutindex, const string &youtput, int youtindex, int mrow, int mcol, double yshift, TPenStyle style, TColor color)
{
 TUWatchInfo wd;
 wd.FullUpdate=false;
 //wd.WatchInterval=watchinterval;

 wd.YShift=yshift;
 wd.XOutputIndex=xoutput;
 wd.XOutputElementIndex=xoutindex;

 wd.YOutputIndex=youtput;
 wd.YOutputElementIndex=youtindex;
 wd.MRow=mrow;
 wd.MCol=mcol;
 wd.Type=type;

 if(wd.Type == 0x200)
 {
  if(!yname.empty())
  {
   wd.Legend=yname;
   wd.Legend+=string("(")+RDK::sntoa(mrow)+string(",");
   wd.Legend+=RDK::sntoa(mcol)+string(")");
  }
 }
 else
 {
  if(!yname.empty())
  {
   wd.Legend=yname;
   wd.Legend+=string("[")+RDK::sntoa(youtput)+string(":");
   wd.Legend+=RDK::sntoa(youtindex)+string("]");
  }
  else
  if(!xname.empty())
  {
   wd.Legend=xname;
   wd.Legend+=string("[")+RDK::sntoa(xoutput)+string(":");
   wd.Legend+=RDK::sntoa(xoutindex)+string("]");
  }
 }

 if(color == 0) // Подбор подходящего цвета
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

int __fastcall TUWatchFrame::Add(int type, const string &xname, const string &yname, int xoutput, int xoutindex, int youtput, int youtindex, int mrow, int mcol, double yshift, TPenStyle style, TColor color)
{
 TUWatchInfo wd;
 wd.FullUpdate=false;
 //wd.WatchInterval=watchinterval;

 wd.YShift=yshift;
 wd.XOutputIndexOld=xoutput;
 wd.XOutputElementIndex=xoutindex;

 wd.YOutputIndexOld=youtput;
 wd.YOutputElementIndex=youtindex;
 wd.MRow=mrow;
 wd.MCol=mcol;
 wd.Type=type;

 if(wd.Type == 0x200)
 {
  if(!yname.empty())
  {
   wd.Legend=yname;
   wd.Legend+=string("[")+RDK::sntoa(youtput)+string("]");
   wd.Legend+=string("(")+RDK::sntoa(mrow)+string(",");
   wd.Legend+=RDK::sntoa(mcol)+string(")");
  }
 }
 else
 {
  if(!yname.empty())
  {
   wd.Legend=yname;
   wd.Legend+=string("[")+RDK::sntoa(youtput)+string(":");
   wd.Legend+=RDK::sntoa(youtindex)+string("]");
  }
  else
  if(!xname.empty())
  {
   wd.Legend=xname;
   wd.Legend+=string("[")+RDK::sntoa(xoutput)+string(":");
   wd.Legend+=RDK::sntoa(xoutindex)+string("]");
  }
 }

 if(color == 0) // Подбор подходящего цвета
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

int __fastcall TUWatchFrame::Add(int type, const string &xname, const string &yname, const string &xoutput, int xoutindex, const string &youtput, int youtindex, const string &mvectorname, int mvectorindexx, int mvectorindexy, double yshift, TPenStyle style, TColor color)
{
 TUWatchInfo wd;
 wd.FullUpdate=true;

 wd.YShift=yshift;
 wd.XOutputIndex=xoutput;
 wd.XOutputElementIndex=xoutindex;

 wd.YOutputIndex=youtput;
 wd.YOutputElementIndex=youtindex;
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
  if(!yname.empty())
  {
   wd.Legend=yname;
   wd.Legend+=string("[")+RDK::sntoa(youtput)+string(":");
   wd.Legend+=RDK::sntoa(youtindex)+string("]");
  }
  else
  if(!xname.empty())
  {
   wd.Legend=xname;
   wd.Legend+=string("[")+RDK::sntoa(xoutput)+string(":");
   wd.Legend+=RDK::sntoa(xoutindex)+string("]");
  }
 }

 if(color == 0) // Подбор подходящего цвета
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


// Удаление наблюдения
void __fastcall TUWatchFrame::Del(int seriesindex)
{
 if(seriesindex >= (int)NameList.size())
  return;
// DelSeries(seriesindex);

 TChartSeries *ser;

 ser=Chart1->Series[seriesindex];
 Chart1->RemoveSeries(ser);
 delete ser;
 // ...... остальное удаляем

 vector<TUWatchInfo>::iterator I=NameList.begin();

 I+=seriesindex;
 NameList.erase(I);

 ModifyState=true;
}

// Удаляет все наблюдения
void __fastcall TUWatchFrame::Clear(void)
{
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

 // ...... остальное удаляем
 NameList.clear();

 ModifyState=true;
}

// Отключает отображение серии
// используется при обновлении данных
// если seriesindex < 0 то отключает все серии
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

// Включает отображение серии
// используется при обновлении данных
// если seriesindex < 0 то включает все серии
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

// Обновление информации за 'stepcount' прошедших шагов интегрирования
void __fastcall TUWatchFrame::StepUpdate(void)
{
 for(int seriesindex=0; seriesindex<(int)NameList.size();seriesindex++)
 {
  TUWatchInfo *wd;

  // Корректируем информацию в сериях
  wd=&NameList[seriesindex];

  TChartSeries* series=Chart1->Series[seriesindex];

  static_cast<TFastLineSeries*>(series)->DrawAllPoints=true;
//  static_cast<TFastLineSeries*>(series)->Stairs=true;

  if(!wd->Visible)
   continue;

   const double *x=0, *y=0;
   const RDK::MDMatrix<double> *ym=0;
   double xdata;
   std::vector<double> vxdata, vydata;

   if(wd->Type == 0x400)
   {
     std::string componentName = UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
	 ym=(const RDK::MDMatrix<double>*)(Model_GetComponentOutputAsMatrix(wd->YDataSourceName.c_str(), wd->YOutputIndex.c_str()));
	 int data_size=0;                                                                                                 
	 if(wd->MVectorName == "Row")
	 {
	  data_size=ym->GetCols();
	  if(componentName=="PulseReceiver")
	  {	
	   vxdata.assign(2*data_size,0);
	   vydata.assign(2*data_size,0);
	  }
	  else
	  {	
	   vxdata.assign(data_size,0);
	   vydata.assign(data_size,0);
	  }
	  if(componentName!="PulseReceiver")
	  {
	  for(int i=0; i<data_size; i++)
	   {
		vxdata[i]=(*ym)(wd->MVectorIndexX, i);
		vydata[i]=(*ym)(wd->MVectorIndexY, i);
	   }
	  }
	  else
	  {
	   for(int i=0; i<data_size; i++)
	   {
		vxdata[2*i]=(*ym)(wd->MVectorIndexX, i);
		vxdata[2*i+1]=(*ym)(wd->MVectorIndexX, i);
		vydata[2*i+1]=(*ym)(wd->MVectorIndexY, i);
		if(vydata[2*i+1]==1)
		 vydata[2*i]=0;
		else if (vydata[2*i+1]==0)
		 vydata[2*i]=1;
	   }
	  }
	 }
	 else if(wd->MVectorName == "Col")
	 {
	  data_size=ym->GetRows();
	  vxdata.assign(data_size,0);
	  vydata.assign(data_size,0);
	  for(int i=0; i<data_size; i++)
	  {
	   vxdata[i]=(*ym)(i, wd->MVectorIndexX);
	   vydata[i]=(*ym)(i, wd->MVectorIndexY);
	  }
	 }
	 y=&vydata[0];
	 x=&vxdata[0];
	 wd->XYSize=data_size;
   }
   else
   if(wd->YDataSourceName.size() && wd->XDataSourceName.size()==0)
   {
	 xdata=Model_GetDoubleTime();
	 x=&xdata;

	 ym=(const RDK::MDMatrix<double>*)(Model_GetComponentOutputAsMatrixByIndex(wd->YDataSourceName.c_str(), wd->YOutputIndexOld));
	 if(!ym)
	 {
	  y=(double*)Model_GetComponentOutputData(wd->YDataSourceName.c_str(), wd->YOutputIndexOld);
	  if(!y)
	   continue;
	 }
	 else
	 {
	  if(ym->GetRows()<=wd->MRow || ym->GetCols()<=wd->MCol)
	   continue;
	  y=&(*ym)(wd->MRow,wd->MCol);
	  if(!y)
	   continue;
	 }
   }
   else
   if(wd->YDataSourceName.size()==0 && wd->XDataSourceName.size())
   {
	int xdata_size=Model_GetComponentOutputDataSize(wd->XDataSourceName.c_str(), wd->XOutputIndexOld);
	 vxdata.assign(xdata_size*3+2,0);
	 vydata.assign(xdata_size*3+2,0);
	 double* xx=(double*)Model_GetComponentOutputData(wd->XDataSourceName.c_str(), wd->XOutputIndexOld);

	 if(!xx || !xdata_size)
	 {
	  vxdata[0]=Model_GetDoubleTime();
	  vydata[0]=0;
	 }
	 else
	 {
	  vxdata[0]=xx[0]-0.0001;
	  vydata[0]=0;
	  for(int i=0,j=1;i<xdata_size;i++,j+=3)
	  {
	   vxdata[j]=xx[i]-0.0001;
	   vydata[j]=0;
	   vxdata[j+1]=xx[i];
	   vydata[j+1]=1;
	   vxdata[j+2]=xx[i]+0.0001;
	   vydata[j+2]=0;
	  }
	  vxdata[xdata_size*3+1]=Model_GetDoubleTime();
	  vydata[xdata_size*3+1]=0;
	 }

	 y=&vydata[0];
	 x=&vxdata[0];
	 wd->XYSize=xdata_size*3+2;
   }
   else
   {
	int xdata_size=Model_GetComponentOutputDataSize(wd->XDataSourceName.c_str(), wd->XOutputIndexOld);
	int ydata_size=Model_GetComponentOutputDataSize(wd->YDataSourceName.c_str(), wd->YOutputIndexOld);
	int data_size=(xdata_size<ydata_size)?xdata_size:ydata_size;
	 vxdata.assign(data_size,0);
	 vydata.assign(data_size,0);
	 double* xx=(double*)Model_GetComponentOutputData(wd->XDataSourceName.c_str(), wd->XOutputIndexOld);
	 double* yy=(double*)Model_GetComponentOutputData(wd->YDataSourceName.c_str(), wd->YOutputIndexOld);
	 if(!xx || !yy)
	  continue;
	 for(int i=0,j=0;i<data_size;i++,j++)
	 {
	  vxdata[j]=xx[i];
	  vydata[j]=yy[i];
	 }

	 y=&vydata[0];
	 x=&vxdata[0];
	 wd->XYSize=data_size;
   }

  // Смотрим способ обновления данных наблюдения...
  if(wd->FullUpdate)
  {
   series->Clear();

//   if(!wd->YDataSourceName.size())
//	continue;

   static_cast<TFastLineSeries*>(series)->AutoRepaint=false;

	for(int i=0;i<wd->XYSize;i++)
	{
	 if(!x || !y || ISNAN(x[i]) || ISNAN(y[i]))
	  continue;
	 series->AddXY(x[i],y[i]+wd->YShift,"",wd->Color);
	}
   static_cast<TFastLineSeries*>(series)->AutoRepaint=true;
  }
  else
  {
   if(wd->WatchInterval < 0)
	continue;

//   if(!wd->YDataSourceName.size())
//	continue;

   static_cast<TFastLineSeries*>(series)->AutoRepaint=false;
   for(int i=0;i<wd->XYSize;i++)
   {
	if(!x || !y || ISNAN(x[i]) || ISNAN(y[i]))
	 continue;
	series->AddXY(x[i],y[i]+wd->YShift,"",wd->Color);
   }

   Chart1->BottomAxis->Automatic=false;
   if(wd->WatchInterval>0)
   {
	double ser_max,ser_min;
	do{
	 if(series->Count() > 1)
	 {
	  ser_max=series->XValue[series->Count()-1];
	  ser_min=series->XValue[1];
	 }
	 else
	 {
	  ser_max=0;
	  ser_min=0;
	 }

	 if(ser_max-wd->WatchInterval<Chart1->BottomAxis->Maximum)
	 {
	  Chart1->BottomAxis->Minimum=ser_max-wd->WatchInterval;
	  Chart1->BottomAxis->Maximum=ser_max;
	 }
	 else
	 {
	  Chart1->BottomAxis->Maximum=ser_max;
	  Chart1->BottomAxis->Minimum=ser_max-wd->WatchInterval;
	 }

	 if(fabs(ser_max-ser_min) > wd->WatchInterval)
	 {
	  series->Delete(0);
//	  series->Delete(0);
	 }
	}while (fabs(ser_max-ser_min) > wd->WatchInterval);
   }
   static_cast<TFastLineSeries*>(series)->AutoRepaint=true;
  }
  ModifyState=true;
 }
}
// ------------------------------

// Удаляет данные всех наблюдений
void __fastcall TUWatchFrame::Reset(void)
{
 // Корректируем информацию в сериях
 for(int i=0;i<Chart1->SeriesCount();i++)
  {
   Chart1->Series[i]->Clear();
   ModifyState=true;
  }

 CacheIndex=0;

 for(int seriesindex=0; seriesindex<(int)NameList.size();seriesindex++)
 {
  TUWatchInfo *wd;

  // Корректируем информацию в сериях
  wd=&NameList[seriesindex];
 }
}


// Возвращает копию списка всех наблюдаемых серий
void __fastcall TUWatchFrame::GetWatchList(map<int, TUWatchInfo> &buffer)
{
 buffer.clear();
 for(size_t i=0;i<NameList.size();i++)
  buffer[i]=NameList[i];
}

// Возвращает копию списка всех видимых серий
void __fastcall TUWatchFrame::GetVisibleList(map<int, TUWatchInfo> &buffer)
{
 buffer.clear();
 for(size_t i=0;i<NameList.size();i++)
  if(NameList[i].Visible)
   buffer[i]=NameList[i];
}

// Возвращает копию списка всех невидимых серий
void __fastcall TUWatchFrame::GetInvisibleList(map<int, TUWatchInfo> &buffer)
{
 buffer.clear();
 for(size_t i=0;i<NameList.size();i++)
  if(!NameList[i].Visible)
   buffer[i]=NameList[i];
}


// Заменяет подпись по выбранной серией
void __fastcall TUWatchFrame::ChangeLegend(int seriesindex, string legend)
{
 if(seriesindex >= (int)NameList.size())
  return;

// ChangeSeriesLegend(seriesindex,legend);
 if(seriesindex >=Chart1->SeriesCount())
  return;

 Chart1->Series[seriesindex]->Title=legend.c_str();
 ModifyState=true;
 // ...Тут остальные обновления

 NameList[seriesindex].Legend=legend;
}

// Заменяет цвет выбранной серии
void __fastcall TUWatchFrame::ChangeColor(int seriesindex, TColor color)
{
 if(seriesindex >= (int)NameList.size())
  return;

// ChangeSeriesColor(seriesindex,color);
 if(seriesindex >=Chart1->SeriesCount())
  return;

 Chart1->Series[seriesindex]->ColorSource=color;
 Chart1->Series[seriesindex]->SeriesColor=color;
 ModifyState=true;
 // ...Тут остальные обновления

 NameList[seriesindex].Color=color;
}

// Заменяет тип линии выбранной серии
void __fastcall TUWatchFrame::ChangeLineStyle(int seriesindex, TPenStyle style)
{
 if(seriesindex >= (int)NameList.size())
  return;

 if(seriesindex >=Chart1->SeriesCount())
  return;

 Chart1->Series[seriesindex]->Pen->Style=style;
 ModifyState=true;
 // ...Тут остальные обновления

 NameList[seriesindex].Style=style;
}

// Изменяет информацио о видимости серии
void __fastcall TUWatchFrame::ChangeVisible(int seriesindex, bool visible)
{
 if(seriesindex < 0 || seriesindex >= (int)NameList.size())
  return;

 NameList[seriesindex].Visible=visible;
 Chart1->Series[seriesindex]->Active=visible;
}


// Изменяет смещение по оси Y
void __fastcall TUWatchFrame::ChangeYShift(int seriesindex, double yshift)
{
 if(seriesindex >= (int)NameList.size())
  return;

 if(seriesindex >=Chart1->SeriesCount())
  return;

 ModifyState=true;
 // ...Тут остальные обновления

 NameList[seriesindex].YShift=yshift;
}


// Изменяет толщину линии
void __fastcall  TUWatchFrame::ChangeLineWidth(int seriesindex, int width)
{
 if(seriesindex >= (int)NameList.size())
  return;

 if(seriesindex >=Chart1->SeriesCount())
  return;

 Chart1->Series[seriesindex]->Pen->Width=width;
 ModifyState=true;
 // ...Тут остальные обновления

 NameList[seriesindex].LineWidth=width;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

// -----------------------------
// Методы визуального управления
// -----------------------------
// Изменение данных серий
void __fastcall TUWatchFrame::TBSeriesModify(TObject *Sender)
{
 USeriesControlForm->Execute(this);
}

// Сохранение графика в файл
/*void __fastcall TWatchFrame::TBGraphSave(TObject *Sender)
{
 if(!SaveDialog1->Execute())
  return;

 Chart1->SaveToBitmapFile(SaveDialog1->FileName);
} */

// Сохраняет изображение графика в bmp
bool TUWatchFrame::SaveToBitmap(const AnsiString &filename)
{
 if(filename == "")
  return false;

 Chart1->SaveToBitmapFile(filename);
 return true;
}

// Сохраняет изображение в метафайл
bool TUWatchFrame::SaveToMetafile(const AnsiString &filename)
{
 if(filename == "")
  return false;

 Chart1->SaveToMetafileEnh(filename);
 return true;
}

// Сохраняет изображение графика в jpeg
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

// Сохраняет изображение с выбором типа из диалога
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
// Методы управления визуальным интерфейсом
// -----------------------------
// Метод, вызываемый после сброса модели
void TUWatchFrame::AAfterReset(void)
{
 Reset();
}

// Обновление интерфейса
void TUWatchFrame::AUpdateInterface(void)
{
 StepUpdate();
 Chart1->Repaint();
}

// Возврат интерфейса в исходное состояние
void TUWatchFrame::AClearInterface(void)
{
 Clear();
}

// Сохраняет параметры интерфейса в xml
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
 // Пробегаем по списку всех открытых серий
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
   xml.WriteInteger("XOutputIndex",NameList[seriesindex].XOutputIndexOld);
   xml.WriteInteger("XOutputElementIndex",NameList[seriesindex].XOutputElementIndex);
   xml.WriteString("YDataSourceName",NameList[seriesindex].YDataSourceName);
   xml.WriteString("YOutputIndexNew",NameList[seriesindex].YOutputIndex);
   xml.WriteInteger("YOutputIndex",NameList[seriesindex].YOutputIndexOld);
   xml.WriteInteger("YOutputElementIndex",NameList[seriesindex].YOutputElementIndex);
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

 ModifyState=false;
}

// Загружает параметры интерфейса из xml
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
   if(xml.GetNodeName() == "UpdateInterval" || xml.GetNodeName() == "WatchInterval" || xml.GetNodeName() == "ComponentControlName" || xml.GetNodeName() == "AlwaysUpdateFlag")
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
	wd->XOutputIndexOld=xml.ReadInteger("XOutputIndex",0);
	wd->XOutputIndex=xml.ReadString("XOutputIndexNew","");
	wd->XOutputElementIndex=xml.ReadInteger("XOutputElementIndex",0);
	wd->YDataSourceName=xml.ReadString("YDataSourceName","");
	wd->YOutputIndexOld=xml.ReadInteger("YOutputIndex",0);
	wd->YOutputIndex=xml.ReadString("YOutputIndexNew","");
	wd->YOutputElementIndex=xml.ReadInteger("YOutputElementIndex",0);
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

void __fastcall TUWatchFrame::AddWatch1Click(TObject *Sender)
{
 UComponentsListForm->ComponentsListFrame1->PageControl1->ActivePageIndex=2;
 if(UComponentsListForm->ShowIOSelect() != mrOk)
  return;

 std::string comp_name=UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 std::string comp_output=UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
 std::string::size_type i=comp_output.find("DataOutput");
 if(i == std::string::npos)
 {
  return;
 }
 comp_output=comp_output.substr(10);

 Add(0, "",comp_name,0,0,RDK::atoi(comp_output),0,0,0);
}
//---------------------------------------------------------------------------

void __fastcall TUWatchFrame::AddXPulseWatch1Click(TObject *Sender)
{
 UComponentsListForm->ComponentsListFrame1->PageControl1->ActivePageIndex=2;
 if(UComponentsListForm->ShowIOSelect() != mrOk)
  return;

 std::string comp_name=UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 std::string comp_output=UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
 std::string::size_type i=comp_output.find("DataOutput");
 if(i == std::string::npos)
 {
  return;
 }
 comp_output=comp_output.substr(10);


 int num_watches=GetNumWatches();
 int yshift=num_watches+1;

 Add(0x100, comp_name,"",RDK::atoi(comp_output),0,0,0,0,0,yshift);
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

  const RDK::MDMatrix<double> *ym=0;
  ym=(const RDK::MDMatrix<double>*)(Model_GetComponentOutputAsMatrix(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(), UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput().c_str()));
  if(!ym)
   return;

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

  std::string componentName = UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
  std::string componentOutput = UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
 Add(0x200, "",componentName,"",0,componentOutput,0,row,col);
}
//---------------------------------------------------------------------------

void __fastcall TUWatchFrame::AddTimeYWatchold1Click(TObject *Sender)
{
 UComponentsListForm->ComponentsListFrame1->PageControl1->ActivePageIndex=2;
 if(UComponentsListForm->ShowIOSelect() != mrOk)
  return;

 int index=UComponentsListForm->ComponentsListFrame1->OutputsStringGrid->Row-1;
 if(index>=0)
  Add(0, "", UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName(), 0, 0, index, 0, 0, 0);
// Add(0, "",UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName(),0,0,UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput(),0,0,0);
}
//---------------------------------------------------------------------------

void __fastcall TUWatchFrame::AddTimeVectorWatch1Click(TObject *Sender)
{
  UComponentsListForm->ComponentsListFrame1->PageControl1->ActivePageIndex=2;
  if(UComponentsListForm->ShowIOSelect() != mrOk)
   return;

  const RDK::MDMatrix<double> *ym=0;
  ym=(const RDK::MDMatrix<double>*)(Model_GetComponentOutputAsMatrix(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName().c_str(), UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput().c_str()));
  if(!ym)
   return;

  UListInputForm->PresentSelect=true;
  UListInputForm->MustInput=true;
  vector<string> listvals;
  listvals.resize(2);
  listvals[0]="Row";
  listvals[1]="Col";

  
  UListInputForm->Init("Rows or Cols?",listvals,"0");
  if(UListInputForm->ShowModal() != mrOk)
   return;
  std::string vectorName=AnsiString(UListInputForm->Edit->Text).c_str();

  if(vectorName == "Row")
   listvals.resize(ym->GetRows());
  else if(vectorName == "Col")
   listvals.resize(ym->GetCols());

  for(size_t i=0;i<listvals.size();i++)
   listvals[i]=RDK::sntoa(i,2);
  UListInputForm->Init("Input X"+vectorName+" Number",listvals,"0");
  if(UListInputForm->ShowModal() != mrOk)
   return;
  int vectorIndexX=StrToInt(UListInputForm->Edit->Text);

  for(size_t i=0;i<listvals.size();i++)
  listvals[i]=RDK::sntoa(i,2);
  UListInputForm->Init("Input Y"+vectorName+" Number",listvals,"0");
  if(UListInputForm->ShowModal() != mrOk)
   return;
  int vectorIndexY=StrToInt(UListInputForm->Edit->Text);

  std::string componentName = UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
  std::string componentOutput = UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
  Add(0x400, "",componentName,"",0,componentOutput,0,vectorName,vectorIndexX, vectorIndexY);
}
//---------------------------------------------------------------------------

