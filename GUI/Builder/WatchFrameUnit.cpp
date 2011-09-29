//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "WatchFrameUnit.h"
#include "ListInputFormUnit.h"
#include "SeriesControlUnit.h"
#include "ShowProgressBarUnit.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TWatchFrame *WatchFrame;

//---------------------------------------------------------------------------
/*
   Методы класса OutGateWatchData
*/
//---------------------------------------------------------------------------
OutGateWatchData::OutGateWatchData(void)
{
 YShift=0;
 FullUpdate=false;
 WatchInterval=-1;

 SeriesIndex=-1;
 DataSource=0;
 Color=TColor(0);

 X=0;
 Y=0;
 XYSize=0;
 Style=psDot;
 LineWidth=3;

 XCurrentMax=0;
 YCurrentMax=0;
 XCurrentMin=0;
 YCurrentMin=0;

 Visible=true;
}

OutGateWatchData::~OutGateWatchData(void)
{
}

OutGateWatchData::OutGateWatchData(const OutGateWatchData &wd)
{
 YShift=wd.YShift;
 Color=wd.Color;
 Legend=wd.Legend;
 X=wd.X;
 Y=wd.Y;
 XYSize=wd.XYSize;
 FullUpdate=wd.FullUpdate;
 WatchInterval=wd.WatchInterval;
 SeriesIndex=wd.SeriesIndex;
 DataSource=wd.DataSource;
 Style=wd.Style;
 LineWidth=wd.LineWidth;

 XCurrentMax=wd.XCurrentMax;
 YCurrentMax=wd.YCurrentMax;
 XCurrentMin=wd.XCurrentMin;
 YCurrentMin=wd.YCurrentMin;

 Visible=wd.Visible;
}

OutGateWatchData& OutGateWatchData::operator = (const OutGateWatchData& wd)
{
 YShift=wd.YShift;
 Color=wd.Color;
 Legend=wd.Legend;
 X=wd.X;
 Y=wd.Y;
 XYSize=wd.XYSize;
 FullUpdate=wd.FullUpdate;
 WatchInterval=wd.WatchInterval;
 SeriesIndex=wd.SeriesIndex;
 DataSource=wd.DataSource;
 Visible=wd.Visible;
 Style=wd.Style;
 LineWidth=wd.LineWidth;

 XCurrentMax=wd.XCurrentMax;
 YCurrentMax=wd.YCurrentMax;
 XCurrentMin=wd.XCurrentMin;
 YCurrentMin=wd.YCurrentMin;

 return *this;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TWatchFrame::TWatchFrame(TComponent* Owner)
        : TFrame(Owner)
{
 SetLegendPosition(5);
 ModifyState=false;
 Chart1->BufferedDisplay=true;
 CacheSize=100;
}

__fastcall TWatchFrame::~TWatchFrame(void)
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
bool __fastcall TWatchFrame::GetModifyState(void)
{
 if(ModifyState)
  {
   ModifyState=false;
   return true;
  }

 return false;
}


// Собирает информацию об открытых сериях в файл 'watchname'
// Если 'collectstate' == 'true', то сохраняет также накопленную информацию
bool __fastcall TWatchFrame::CollectInfo(string watchname, bool collectstate)
{
 TMemIniFile *ini;
 char *out=0;
 int len;
 char buffer[32];
 string *sp;

 if(NameList.size() == 0)
  return true;

 ini=new TMemIniFile(watchname.c_str());
 if(!ini)
  return false;

 ini->Clear();

 String s=ShowProgressBarForm->GetBarHeader(1);
 ShowProgressBarForm->ResetBarStatus(1,0,NameList.size());

 // Пробегаем по списку всех открытых серий
 for(int seriesindex=0;seriesindex<(int)NameList.size();seriesindex++)
  {
   ShowProgressBarForm->SetBarHeader(1,s+" - "+NameList[seriesindex].Legend.c_str()+":");

   ini->WriteString(IntToStr(seriesindex),"Legend",NameList[seriesindex].Legend.c_str());
   ini->WriteString(IntToStr(seriesindex),"YShift",FloatToStr(NameList[seriesindex].YShift));
   ini->WriteString(IntToStr(seriesindex),"Color",IntToStr(NameList[seriesindex].Color));
   ini->WriteString(IntToStr(seriesindex),"Style",IntToStr(NameList[seriesindex].Style));
   ini->WriteString(IntToStr(seriesindex),"Visible",IntToStr(int(NameList[seriesindex].Visible)));
   ini->WriteString(IntToStr(seriesindex),"LineWidth",IntToStr(int(NameList[seriesindex].LineWidth)));

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
	 ini->WriteString(IntToStr(seriesindex),"XDATA",out);

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
	 ini->WriteString(IntToStr(seriesindex),"YDATA",out);
	}

   ShowProgressBarForm->IncBarStatus(1);
  }

 ini->UpdateFile();
 delete ini;
 ModifyState=false;
 return true;
}

// Восстанавливает серии по информации из файла 'watchname'
// Если 'collectstate' == 'true', то восстанавливает также накопленную
// информацию
// (Все открытые наблюдения будут предварительно закрыты)
bool __fastcall TWatchFrame::RestoreInfo(string watchname, bool collectstate)
{
 map<string,OutGateWatchData*>::iterator I;
 TMemIniFile *ini;
 TStringList *series;
 OutGateWatchData *wd;
 TFastLineSeries *grseries;
 string xs,ys;
 double x,y;
 string::size_type ix1,ix2,iy1,iy2;

 ini=new TMemIniFile(watchname.c_str());
 if(!ini)
  return false;

// Clear();

 series=new TStringList;
 ini->ReadSections(series);

 String s=ShowProgressBarForm->GetBarHeader(1);
 ShowProgressBarForm->ResetBarStatus(1,0,series->Count);

 size_t oldnamelistsize=NameList.size();

 for(int i=0;i<series->Count;i++)
  {
   ShowProgressBarForm->SetBarHeader(1,s+" - "+series->Strings[i]+":");

/*   if(!Model->GetOutGate(series->Strings[i].c_str()))
	{
	 ShowProgressBarForm->IncBarStatus(1);
	 continue;
	}*/
   int j=StrToInt(series->Strings[i]);
   if(j<int(oldnamelistsize))
   {
	// Заменяет подпись под выбранной серией
	ChangeLegend(j, AnsiString(ini->ReadString(IntToStr(j),"Legend","")).c_str());

	// Заменяет цвет выбранной серии
	ChangeColor(j, (TColor)StrToInt(ini->ReadString(IntToStr(j),"Color",IntToStr(clTeeColor))));

	// Заменяет тип линии выбранной серии
	ChangeLineStyle(j, (TPenStyle)StrToInt(ini->ReadString(IntToStr(j),"Style",IntToStr(psSolid))));

	// Изменяет информацио о видимости серии
	ChangeVisible(j, ini->ReadInteger(IntToStr(j),"Visible",1));

	ChangeYShift(j, StrToFloat(ini->ReadString(IntToStr(j),"YShift","0")));

	ChangeLineWidth(j, StrToInt(ini->ReadString(IntToStr(j),"LineWidth","1")));
   }
   else
   {
	NameList.resize(NameList.size()+1);
	wd=&NameList[NameList.size()-1];
	wd->Legend=AnsiString(ini->ReadString(IntToStr(j),"Legend","")).c_str();
	wd->YShift=StrToFloat(ini->ReadString(IntToStr(j),"YShift","0"));
	wd->Color=(TColor)StrToInt(ini->ReadString(IntToStr(j),"Color",IntToStr(clTeeColor)));
	wd->Style=(TPenStyle)StrToInt(ini->ReadString(IntToStr(j),"Style",IntToStr(psSolid)));
	wd->LineWidth=StrToInt(ini->ReadString(IntToStr(j),"LineWidth",1));

	grseries=new TFastLineSeries(Chart1);
	grseries->ParentChart=Chart1;
	grseries->Title=wd->Legend.c_str();
	grseries->ColorSource=wd->Color;
	grseries->SeriesColor=wd->Color;
	grseries->LinePen->Style=wd->Style;
	if(collectstate)
	{
	 grseries->Clear();
	 ix1=0; ix2=0;
	 iy1=0; iy2=0;
	 xs=AnsiString(ini->ReadString(IntToStr(j),"XDATA","")).c_str();
	 ys=AnsiString(ini->ReadString(IntToStr(j),"YDATA","")).c_str();
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

   ShowProgressBarForm->IncBarStatus(1);
  }

 delete series;
 delete ini;
 ModifyState=false;
 return true;
}
// ------------------------------

// ------------------------------
// Методы настройки отображения
// ------------------------------
// Включает/отключает автомасштабирование по оси X
void __fastcall TWatchFrame::SetXAutoScale(bool value)
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
bool __fastcall TWatchFrame::GetXAutoScale(void)
{
 return Chart1->TopAxis->Automatic;
}

// Включает/отключает автомасштабирование по оси Y
void __fastcall TWatchFrame::SetYAutoScale(bool value)
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
bool __fastcall TWatchFrame::GetYAutoScale(void)
{
 return Chart1->LeftAxis->Automatic;
}

// Устанавливает границы изменения по оси X
void __fastcall TWatchFrame::SetXMin(double value)
{
  Chart1->TopAxis->Minimum=value;
  Chart1->BottomAxis->Minimum=value;
}

void __fastcall TWatchFrame::SetXMax(double value)
{
  Chart1->TopAxis->Maximum=value;
  Chart1->BottomAxis->Maximum=value;
}

// Устанавливает границы изменения по оси Y
void __fastcall TWatchFrame::SetYMin(double value)
{
  Chart1->LeftAxis->Minimum=value;
  Chart1->RightAxis->Minimum=value;
}

void __fastcall TWatchFrame::SetYMax(double value)
{
  Chart1->LeftAxis->Maximum=value;
  Chart1->RightAxis->Maximum=value;
}

// Возвращает границы изменения по оси X
double __fastcall TWatchFrame::GetXMin(void)
{
 return Chart1->TopAxis->Minimum;
}

double __fastcall TWatchFrame::GetXMax(void)
{
 return Chart1->TopAxis->Maximum;
}

// Возвращает границы изменения по оси Y
double __fastcall TWatchFrame::GetYMin(void)
{
 return Chart1->LeftAxis->Minimum;
}

double __fastcall TWatchFrame::GetYMax(void)
{
 return Chart1->LeftAxis->Maximum;
}

// Устанавливает заголовок графика
void __fastcall TWatchFrame::SetGraphTitle(AnsiString title)
{
 Chart1->Title->Text->Clear();
 Chart1->Title->Text->Add(title);

 if(title.Length() == 0)
  Chart1->Title->Visible=false;
 else
  Chart1->Title->Visible=true;
}

// Устанавливает видимость подписей по осям
void __fastcall TWatchFrame::SetXLabelVisible(bool value)
{
 Chart1->BottomAxis->Labels=value;
}

void __fastcall TWatchFrame::SetYLabelVisible(bool value)
{
 Chart1->LeftAxis->Labels=value;
}

// Возвращает видимость подписей по осям
bool __fastcall TWatchFrame::GetXLabelVisible(void)
{
 return Chart1->BottomAxis->Labels;
}

bool __fastcall TWatchFrame::GetYLabelVisible(void)
{
 return Chart1->LeftAxis->Labels;
}

// Устанавливает подписи по осям
void __fastcall TWatchFrame::SetXLabelTitle(AnsiString value)
{
 Chart1->BottomAxis->Title->Caption=value;
}

void __fastcall TWatchFrame::SetYLabelTitle(AnsiString value)
{
 Chart1->LeftAxis->Title->Caption=value;
}

// Возвращает подписи по осям
AnsiString __fastcall TWatchFrame::GetXLabelTitle(void)
{
 return Chart1->BottomAxis->Title->Caption;
}

AnsiString __fastcall TWatchFrame::GetYLabelTitle(void)
{
 return Chart1->LeftAxis->Title->Caption;
}

// Устанавливает видимость легенды
void __fastcall TWatchFrame::SetLegendVisible(bool value)
{
 Chart1->Legend->Visible=value;
 Chart1->Legend->DividingLines->Visible=false;
}

// Возвращает видимость легенды
bool __fastcall TWatchFrame::GetLegendVisible(void)
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
void __fastcall TWatchFrame::SetLegendPosition(int value)
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
int __fastcall TWatchFrame::GetLegendPosition(void)
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

// Размер кеша отображаемых данных
int TWatchFrame::GetCacheSize(void) const
{
 return CacheSize;
}

bool TWatchFrame::SetCacheSize(int value)
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
OutGateWatchData* __fastcall TWatchFrame::Get(int seriesindex)
{
 if(seriesindex < 0 || seriesindex >= (int)NameList.size())
  return 0;

 return &NameList[seriesindex];
}

OutGateWatchData* __fastcall TWatchFrame::Get(void *datasource)
{
 for(size_t i=0;i<NameList.size();i++)
  if(NameList[i].DataSource == datasource)
   return &NameList[i];

 return 0;
}

// Возвращает общее число данных наблюдения
int __fastcall TWatchFrame::GetNumWatches(void)
{
 return (int)NameList.size();
}


// Добавление нового наблюдения
int __fastcall TWatchFrame::Add(OutGateWatchData& wd)
{
 // Проверяем, есть ли серия с такими же данными
 int seriesindex=-1;

 vector<OutGateWatchData>::iterator I;
 I=NameList.begin();
 int i=0;
 while(I != NameList.end())
  {
   if(wd.Y == I->Y)
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

// Удаление наблюдения
void __fastcall TWatchFrame::Del(int seriesindex)
{
 if(seriesindex >= (int)NameList.size())
  return;
// DelSeries(seriesindex);

 TChartSeries *ser;

 ser=Chart1->Series[seriesindex];
 Chart1->RemoveSeries(ser);
 delete ser;
 // ...... остальное удаляем

 vector<OutGateWatchData>::iterator I=NameList.begin();

 I+=seriesindex;
 NameList.erase(I);

 ModifyState=true;
}

// Удаляет все наблюдения
void __fastcall TWatchFrame::Clear(void)
{
 map<string,OutGateWatchData*>::iterator I;

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
void __fastcall TWatchFrame::SeriesDisable(int seriesindex)
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
void __fastcall TWatchFrame::SeriesEnable(int seriesindex)
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

// Обновление данных серии
// Добавление одной точки в серию
void __fastcall TWatchFrame::SeriesUpdate(int seriesindex, double x, double y)
{
 if(seriesindex < 0 || seriesindex >= (int)NameList.size())
  return;

 OutGateWatchData *wd;


 // Корректируем информацию в сериях
 wd=&NameList[seriesindex];

 if(!wd->Visible)
  return;

 // Смотрим способ обновления данных наблюдения...
 if(wd->FullUpdate)
 {
  Chart1->Series[seriesindex]->Clear();
  Chart1->Series[seriesindex]->AddXY(x,y+wd->YShift,"",wd->Color);
 }
 else
 {
  if(wd->WatchInterval == 0)
   return;

  Chart1->Series[seriesindex]->AddXY(x,y+wd->YShift,"",wd->Color);
 }
 ModifyState=true;
}

void __fastcall TWatchFrame::SeriesUpdate(void* datasource, double x, double y)
{
 for(int i=0;i<(int)NameList.size();i++)
  if(NameList[i].DataSource == datasource)
  {
   SeriesUpdate(i,x,y);
   return;
  }
}

// Обновление данных серии
// Добавление массива точек в серию
void __fastcall TWatchFrame::SeriesUpdate(int seriesindex, double* x, double* y, int size)
{
 if(seriesindex < 0 || seriesindex >= (int)NameList.size())
  return;

 OutGateWatchData *wd;

 // Корректируем информацию в сериях
 wd=&NameList[seriesindex];

 if(!wd->Visible)
  return;

 
 // Смотрим способ обновления данных наблюдения...
 if(wd->FullUpdate)
 {
  Chart1->Series[seriesindex]->Clear();

  if(!x || !y || size == 0)
   return;

  for(int i=0;i<size;i++)
   Chart1->Series[seriesindex]->AddXY(x[i],y[i]+wd->YShift,"",wd->Color);
 }
 else
 {
  if(wd->WatchInterval == 0)
   return;

  if(!x || !y || size == 0)
   return;

  for(int i=0;i<size;i++)
   Chart1->Series[seriesindex]->AddXY(x[i],y[i]+wd->YShift,"",wd->Color);
  if(wd->WatchInterval>0)
  {
   double ser_max,ser_min;
   if(Chart1->Series[seriesindex]->Count() > 0)
   {
	ser_max=Chart1->Series[seriesindex]->XValue[Chart1->Series[seriesindex]->Count()-1];
	ser_min=Chart1->Series[seriesindex]->XValue[0];
   }
   else
   {
	ser_max=0;
	ser_min=0;
   }

   if(ser_max-ser_min > wd->WatchInterval)
    Chart1->Series[seriesindex]->Delete(0);
  }
 }
 ModifyState=true;
}

void __fastcall TWatchFrame::SeriesUpdate(void* datasource, double* x, double* y, int size)
{
 for(int i=0;i<(int)NameList.size();i++)
  if(NameList[i].DataSource == datasource)
  {
   SeriesUpdate(i,x,y,size);
   return;
  }
}

// Обновление информации за 'stepcount' прошедших шагов интегрирования
void __fastcall TWatchFrame::StepUpdate(bool speedup)
{
 for(int seriesindex=0; seriesindex<(int)NameList.size();seriesindex++)
 {
  OutGateWatchData *wd;

  // Корректируем информацию в сериях
  wd=&NameList[seriesindex];

  if(!wd->X || !wd->Y)
   continue;

  // Кеш максимума и минимума за заданный интервал времени
  for(int i=0;i<wd->XYSize;i++)
  {
   if(wd->YCurrentMax<=wd->Y[i])
   {
	wd->XCurrentMax=wd->X[i];
	wd->YCurrentMax=wd->Y[i];
   }
   if(wd->YCurrentMin>=wd->Y[i])
   {
	wd->XCurrentMin=wd->X[i];
	wd->YCurrentMin=wd->Y[i];
   }
  }

  ++CacheIndex;
 }

 if(speedup && CacheIndex < CacheSize)
  return;
 else
 {
  CacheIndex=0;
 }

// Chart1->AutoRepaint=false;

 for(int seriesindex=0; seriesindex<(int)NameList.size();seriesindex++)
 {
  OutGateWatchData *wd;

  // Корректируем информацию в сериях
  wd=&NameList[seriesindex];

  TChartSeries* series=Chart1->Series[seriesindex];

  static_cast<TFastLineSeries*>(series)->DrawAllPoints=!speedup;
//  static_cast<TFastLineSeries*>(series)->Stairs=true;

  if(!wd->Visible)
   continue;

  // Смотрим способ обновления данных наблюдения...
  if(wd->FullUpdate)
  {
   series->Clear();

   if(!wd->X || !wd->Y || wd->XYSize == 0)
	continue;

   static_cast<TFastLineSeries*>(series)->AutoRepaint=false;
   for(int i=0;i<wd->XYSize;i++)
	series->AddXY(wd->X[i],wd->Y[i]+wd->YShift,"",wd->Color);
   static_cast<TFastLineSeries*>(series)->AutoRepaint=true;
  }
  else
  {
   if(wd->WatchInterval == 0)
	continue;

   if(!wd->X || !wd->Y || wd->XYSize == 0)
	continue;

   static_cast<TFastLineSeries*>(series)->AutoRepaint=false;
   if(wd->XCurrentMin<wd->XCurrentMax)
   {
	series->AddXY(wd->XCurrentMin,wd->YCurrentMin+wd->YShift,"",wd->Color);
	series->AddXY(wd->XCurrentMax,wd->YCurrentMax+wd->YShift,"",wd->Color);
   }
   else
   {
	series->AddXY(wd->XCurrentMax,wd->YCurrentMax+wd->YShift,"",wd->Color);
	series->AddXY(wd->XCurrentMin,wd->YCurrentMin+wd->YShift,"",wd->Color);
   }

   if(wd->WatchInterval>0)
   {
	double ser_max,ser_min;
	do{
	 if(series->Count() > 0)
	 {
	  ser_max=series->XValue[series->Count()-1];
	  ser_min=series->XValue[0];
	 }
	 else
	 {
	  ser_max=0;
	  ser_min=0;
	 }

	 Chart1->BottomAxis->Automatic=false;
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

	 if(ser_max-ser_min > wd->WatchInterval)
	 {
	  series->Delete(0);
//	  series->Delete(0);
	 }
	}while (ser_max-ser_min > wd->WatchInterval);
   }
   static_cast<TFastLineSeries*>(series)->AutoRepaint=false;
  }
  ModifyState=true;
  wd->XCurrentMax=0;
  wd->YCurrentMax=0;
  wd->XCurrentMin=0;
  wd->YCurrentMin=0;
 }
}
// ------------------------------

// Удаляет данные всех наблюдений
void __fastcall TWatchFrame::Reset(void)
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
  OutGateWatchData *wd;

  // Корректируем информацию в сериях
  wd=&NameList[seriesindex];
  wd->XCurrentMin=0;
  wd->YCurrentMin=0;
  wd->XCurrentMax=0;
  wd->YCurrentMax=0;
 }
}


// Возвращает копию списка всех наблюдаемых серий
void __fastcall TWatchFrame::GetWatchList(map<int, OutGateWatchData> &buffer)
{
 buffer.clear();
 for(size_t i=0;i<NameList.size();i++)
  buffer[i]=NameList[i];
}

// Возвращает копию списка всех видимых серий
void __fastcall TWatchFrame::GetVisibleList(map<int, OutGateWatchData> &buffer)
{
 buffer.clear();
 for(size_t i=0;i<NameList.size();i++)
  if(NameList[i].Visible)
   buffer[i]=NameList[i];
}

// Возвращает копию списка всех невидимых серий
void __fastcall TWatchFrame::GetInvisibleList(map<int, OutGateWatchData> &buffer)
{
 buffer.clear();
 for(size_t i=0;i<NameList.size();i++)
  if(!NameList[i].Visible)
   buffer[i]=NameList[i];
}


// Заменяет подпись по выбранной серией
void __fastcall TWatchFrame::ChangeLegend(int seriesindex, string legend)
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
void __fastcall TWatchFrame::ChangeColor(int seriesindex, TColor color)
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
void __fastcall TWatchFrame::ChangeLineStyle(int seriesindex, TPenStyle style)
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
void __fastcall TWatchFrame::ChangeVisible(int seriesindex, bool visible)
{
 if(seriesindex < 0 || seriesindex >= (int)NameList.size())
  return;

 NameList[seriesindex].Visible=visible;
 Chart1->Series[seriesindex]->Active=visible;
}


// Изменяет смещение по оси Y
void __fastcall TWatchFrame::ChangeYShift(int seriesindex, double yshift)
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
void __fastcall  TWatchFrame::ChangeLineWidth(int seriesindex, int width)
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
void __fastcall TWatchFrame::TBSeriesModify(TObject *Sender)
{
 SeriesControlForm->Execute(this);
}

// Сохранение графика в файл
/*void __fastcall TWatchFrame::TBGraphSave(TObject *Sender)
{
 if(!SaveDialog1->Execute())
  return;

 Chart1->SaveToBitmapFile(SaveDialog1->FileName);
} */

// Сохраняет изображение графика в bmp
bool TWatchFrame::SaveToBitmap(const AnsiString &filename)
{
 if(filename == "")
  return false;

 Chart1->SaveToBitmapFile(filename);
 return true;
}

// Сохраняет изображение в метафайл
bool TWatchFrame::SaveToMetafile(const AnsiString &filename)
{
 if(filename == "")
  return false;

 Chart1->SaveToMetafileEnh(filename);
 return true;
}

// Сохраняет изображение графика в jpeg
bool TWatchFrame::SaveToJpeg(const AnsiString &filename)
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
bool TWatchFrame::Save(void)
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
//---------------------------------------------------------------------------
// -----------------------------



void __fastcall TWatchFrame::N1Click(TObject *Sender)
{
 TBSeriesModify(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TWatchFrame::bmp1Click(TObject *Sender)
{
 Save();
}
//---------------------------------------------------------------------------

