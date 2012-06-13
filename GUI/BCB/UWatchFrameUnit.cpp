//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UWatchFrameUnit.h"
#include "UListInputFormUnit.h"
#include "USeriesControlUnit.h"
#include "UShowProgressBarUnit.h"
#include "UComponentsListFormUnit.h"
#include "rdk_initdll.h"
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUWatchFrame *UWatchFrame;

//---------------------------------------------------------------------------
/*
   Методы класса TUWatchInfo
*/
//---------------------------------------------------------------------------
TUWatchInfo::TUWatchInfo(void)
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

 OutputIndex=0;

 OutputElementIndex=0;

}

TUWatchInfo::~TUWatchInfo(void)
{
}

TUWatchInfo::TUWatchInfo(const TUWatchInfo &wd)
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

 DataSourceName=wd.DataSourceName;

 OutputIndex=wd.OutputIndex;

 OutputElementIndex=wd.OutputElementIndex;

}

TUWatchInfo& TUWatchInfo::operator = (const TUWatchInfo& wd)
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

 DataSourceName=wd.DataSourceName;

 OutputIndex=wd.OutputIndex;

 OutputElementIndex=wd.OutputElementIndex;


 return *this;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
__fastcall TUWatchFrame::TUWatchFrame(TComponent* Owner)
        : TFrame(Owner)
{
 SetLegendPosition(5);
 ModifyState=false;
 Chart1->BufferedDisplay=true;
 CacheSize=100;
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


// Сохранение и загрузка описания графика в файл
bool TUWatchFrame::SaveToIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return false;

 return true;
}

bool TUWatchFrame::LoadFromIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return false;

 return true;
}

// Собирает информацию об открытых сериях в файл 'watchname'
// Если 'collectstate' == 'true', то сохраняет также накопленную информацию
bool __fastcall TUWatchFrame::CollectInfo(string watchname, bool collectstate)
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

 String s=UShowProgressBarForm->GetBarHeader(1);
 UShowProgressBarForm->ResetBarStatus(1,0,NameList.size());

 // Пробегаем по списку всех открытых серий
 for(int seriesindex=0;seriesindex<(int)NameList.size();seriesindex++)
  {
   UShowProgressBarForm->SetBarHeader(1,s+" - "+NameList[seriesindex].Legend.c_str()+":");

   ini->WriteString(IntToStr(seriesindex),"Legend",NameList[seriesindex].Legend.c_str());
   ini->WriteString(IntToStr(seriesindex),"YShift",FloatToStr(NameList[seriesindex].YShift));
   ini->WriteString(IntToStr(seriesindex),"Color",IntToStr(NameList[seriesindex].Color));
   ini->WriteString(IntToStr(seriesindex),"Style",IntToStr(NameList[seriesindex].Style));
   ini->WriteString(IntToStr(seriesindex),"Visible",IntToStr(int(NameList[seriesindex].Visible)));
   ini->WriteString(IntToStr(seriesindex),"LineWidth",IntToStr(int(NameList[seriesindex].LineWidth)));
   ini->WriteString(IntToStr(seriesindex),"DataSourceName",NameList[seriesindex].DataSourceName.c_str());
   ini->WriteString(IntToStr(seriesindex),"OutputIndex",IntToStr(int(NameList[seriesindex].OutputIndex)));
   ini->WriteString(IntToStr(seriesindex),"OutputElementIndex",IntToStr(int(NameList[seriesindex].OutputElementIndex)));

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

   UShowProgressBarForm->IncBarStatus(1);
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
bool __fastcall TUWatchFrame::RestoreInfo(string watchname, bool collectstate)
{
 map<string,TUWatchInfo*>::iterator I;
 TMemIniFile *ini;
 TStringList *series;
 TUWatchInfo *wd;
 TFastLineSeries *grseries;
 string xs,ys;
 double x,y;
 string::size_type ix1,ix2,iy1,iy2;

 ini=new TMemIniFile(watchname.c_str());
 if(!ini)
  return false;

 Clear();

 series=new TStringList;
 ini->ReadSections(series);

 String s=UShowProgressBarForm->GetBarHeader(1);
 UShowProgressBarForm->ResetBarStatus(1,0,series->Count);

 size_t oldnamelistsize=NameList.size();

 for(int i=0;i<series->Count;i++)
  {
   UShowProgressBarForm->SetBarHeader(1,s+" - "+series->Strings[i]+":");

   int j=StrToInt(series->Strings[i]);
 /*  if(j<int(oldnamelistsize))
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
   else   */
   {
	TUWatchInfo wd_data;
	wd=&wd_data;
//	NameList.resize(NameList.size()+1);
//	wd=&NameList[NameList.size()-1];
	wd->Legend=AnsiString(ini->ReadString(IntToStr(j),"Legend","")).c_str();
	wd->YShift=StrToFloat(ini->ReadString(IntToStr(j),"YShift","0"));
	wd->Color=(TColor)StrToInt(ini->ReadString(IntToStr(j),"Color",IntToStr(clTeeColor)));
	wd->Style=(TPenStyle)StrToInt(ini->ReadString(IntToStr(j),"Style",IntToStr(psSolid)));
	wd->LineWidth=StrToInt(ini->ReadString(IntToStr(j),"LineWidth",1));
	wd->DataSourceName=AnsiString(ini->ReadString(IntToStr(j),"DataSourceName","")).c_str();
	wd->OutputIndex=StrToInt(ini->ReadString(IntToStr(j),"OutputIndex","0"));
	wd->OutputElementIndex=StrToInt(ini->ReadString(IntToStr(j),"OutputElementIndex","0"));


/*	grseries=new TFastLineSeries(Chart1);
	grseries->ParentChart=Chart1;
	grseries->Title=wd->Legend.c_str();
	grseries->ColorSource=wd->Color;
	grseries->SeriesColor=wd->Color;
	grseries->LinePen->Style=wd->Style;*/
	int grindex=Add(wd_data);
	grseries=dynamic_cast<TFastLineSeries*>(Chart1->Series[grindex]);
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

   UShowProgressBarForm->IncBarStatus(1);
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

TUWatchInfo* __fastcall TUWatchFrame::Get(void *datasource)
{
 for(size_t i=0;i<NameList.size();i++)
  if(NameList[i].DataSource == datasource)
   return &NameList[i];

 return 0;
}

// Возвращает общее число данных наблюдения
int __fastcall TUWatchFrame::GetNumWatches(void)
{
 return (int)NameList.size();
}


// Добавление нового наблюдения
int __fastcall TUWatchFrame::Add(TUWatchInfo& wd)
{
 // Проверяем, есть ли серия с такими же данными
 int seriesindex=-1;

 vector<TUWatchInfo>::iterator I;
 I=NameList.begin();
 int i=0;
 while(I != NameList.end())
  {
   if((wd.Y && (wd.Y == I->Y)) ||
   (I->DataSourceName == wd.DataSourceName && I->OutputIndex == wd.OutputIndex &&
    I->OutputElementIndex == wd.OutputElementIndex))
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

// Добавление нового наблюдения по имени компонента и индексу выхода
// Возвращает индекс серии
int __fastcall TUWatchFrame::Add(const string &name, int output, int outindex, double yshift, TPenStyle style, TColor color)
{
 TUWatchInfo wd;
 wd.FullUpdate=false;
 //wd.WatchInterval=watchinterval;

 wd.YShift=yshift;
 wd.Legend=name;
 wd.OutputIndex=output;
 wd.OutputElementIndex=outindex;

/* if(itemd->GetOwner())
  itemd->GetLongName(static_pointer_cast<NAContainer>(itemd->GetOwner()->GetOwner()),wd.Legend);
 else
  itemd->GetLongName(0,wd.Legend);*/
 wd.Legend+=string("[")+RDK::sntoa(output)+string(":");
 wd.Legend+=RDK::sntoa(outindex)+string("]");
// wd.DataSource=item;

 if(color == 0) // Подбор подходящего цвета
  wd.Color=Chart1->GetFreeSeriesColor(true);
 else
  wd.Color=color;

 wd.DataSourceName=name;
// wd.X=&NAContainer::GetDoubleTime();
// wd.Y=&(itemd->GetOutputData(output).Double[outindex]);
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
            /*
// Обновление данных серии
// Добавление одной точки в серию
void __fastcall TUWatchFrame::SeriesUpdate(int seriesindex, double x, double y)
{
 if(seriesindex < 0 || seriesindex >= (int)NameList.size())
  return;

 TUWatchInfo *wd;


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

void __fastcall TUWatchFrame::SeriesUpdate(void* datasource, double x, double y)
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
void __fastcall TUWatchFrame::SeriesUpdate(int seriesindex, double* x, double* y, int size)
{
 if(seriesindex < 0 || seriesindex >= (int)NameList.size())
  return;

 TUWatchInfo *wd;

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

void __fastcall TUWatchFrame::SeriesUpdate(void* datasource, double* x, double* y, int size)
{
 for(int i=0;i<(int)NameList.size();i++)
  if(NameList[i].DataSource == datasource)
  {
   SeriesUpdate(i,x,y,size);
   return;
  }
}           */

// Обновление информации за 'stepcount' прошедших шагов интегрирования
void __fastcall TUWatchFrame::StepUpdate(bool speedup)
{
 for(int seriesindex=0; seriesindex<(int)NameList.size();seriesindex++)
 {
  TUWatchInfo *wd;
  const double *x=0,*y=0;

  // Корректируем информацию в сериях
  wd=&NameList[seriesindex];

  if((!wd->X || !wd->Y) && !wd->DataSourceName.size())
   continue;

   double xdata;
   if(wd->DataSourceName.size())
   {
	xdata=Model_GetDoubleTime();
	x=&xdata;
	y=(double*)Model_GetComponentOutputData(wd->DataSourceName.c_str(), wd->OutputIndex);
	if(!y)
	 continue;
   }
   else
   {
	x=wd->X;
	y=wd->Y;
   }

  // Кеш максимума и минимума за заданный интервал времени
  for(int i=0;i<wd->XYSize;i++)
  {
   if(wd->YCurrentMax<=y[i])
   {
	wd->XCurrentMax=x[i];
	wd->YCurrentMax=y[i];
   }
   if(wd->YCurrentMin>=y[i])
   {
	wd->XCurrentMin=x[i];
	wd->YCurrentMin=y[i];
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
  TUWatchInfo *wd;

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

   if((!wd->X || !wd->Y || wd->XYSize == 0) && !wd->DataSourceName.size())
	continue;

   const double *x=0, *y=0;
   double xdata;
   if(wd->DataSourceName.size())
   {
	xdata=Model_GetDoubleTime();
	x=&xdata;
	y=(double*)Model_GetComponentOutputData(wd->DataSourceName.c_str(), wd->OutputIndex);
	if(!y)
	 continue;
   }
   else
   {
	x=wd->X;
	y=wd->Y;
   }

   static_cast<TFastLineSeries*>(series)->AutoRepaint=false;
   for(int i=0;i<wd->XYSize;i++)
	series->AddXY(x[i],y[i]+wd->YShift,"",wd->Color);
   static_cast<TFastLineSeries*>(series)->AutoRepaint=true;
  }
  else
  {
   if(wd->WatchInterval == 0)
	continue;

   if((!wd->X || !wd->Y || wd->XYSize == 0) && !wd->DataSourceName.size())
	continue;

   const double *x=0, *y=0;
   double xdata;
   if(wd->DataSourceName.size())
   {
	xdata=Model_GetDoubleTime();
	x=&xdata;
	y=(double*)Model_GetComponentOutputData(wd->DataSourceName.c_str(), wd->OutputIndex);
	if(!y)
	 continue;
   }
   else
   {
	x=wd->X;
	y=wd->Y;
   }

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
  wd->XCurrentMin=0;
  wd->YCurrentMin=0;
  wd->XCurrentMax=0;
  wd->YCurrentMax=0;
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
//---------------------------------------------------------------------------
// -----------------------------



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
 if(UComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 Add(UComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName(),UComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput());
}
//---------------------------------------------------------------------------

