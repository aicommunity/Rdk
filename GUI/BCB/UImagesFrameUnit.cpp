//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UImagesFrameUnit.h"
#include "UComponentsListFormUnit.h"
#include "rdk_initdll.h"
#include "TUBitmap.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUImagesFrame *UImagesFrame;
//---------------------------------------------------------------------------
__fastcall TUImagesFrame::TUImagesFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 ReflectionXFlag=true;
 MyComponentsListForm=new TUComponentsListForm(this);
 UpdateInterval=100;
 SetNumCells(2, 2);
 SizeMode=0;
}

__fastcall TUImagesFrame::~TUImagesFrame(void)
{
 if(MyComponentsListForm)
  delete MyComponentsListForm;
}


// --------------------------
// Методы управления параметрами
// --------------------------
// Устанавливает размер ячейки
void TUImagesFrame::SetCellRes(int width, int height)
{
 DrawGrid->DefaultColWidth=width;
 DrawGrid->DefaultRowHeight=height;
}

// Устанавливает число ячеек
void TUImagesFrame::SetNumCells(int width, int height)
{
 DrawGrid->ColCount=width;
 DrawGrid->RowCount=height;

 for(size_t i=0;i<Images.size();i++)
  for(size_t j=0;j<Images[i].size();j++)
   delete Images[i][j];

 Images.resize(width);
 StringIds.resize(width);
 ComponentIndexes.resize(width);
 ComponentIndexesOld.resize(width);
 MouseClickComponents.resize(width);
 Legends.resize(width);
 for(size_t i=0;i<Images.size();i++)
 {
  Images[i].resize(height);
  StringIds[i].resize(height);
  ComponentIndexes[i].resize(height);
  ComponentIndexesOld[i].resize(height,-1);
  MouseClickComponents[i].resize(height);
  Legends[i].resize(height);
  for(size_t j=0;j<Images[i].size();j++)
   Images[i][j]=new TImage(this);
 }
}

// Возвращает число ячеек
int TUImagesFrame::GetNumCellWidth(void)
{
 return DrawGrid->ColCount;
}

int TUImagesFrame::GetNumCellHeight(void)
{
 return DrawGrid->RowCount;
}

// Флаг отражения вокруг оси X изображений при выводе
bool TUImagesFrame::GetReflectionXFlag(void)
{
 return ReflectionXFlag;
}

void TUImagesFrame::SetReflectionXFlag(bool value)
{
 ReflectionXFlag=value;
}
// --------------------------

// --------------------------
// Методы управления изображениями
// --------------------------
// Связывает ячейку с идентификатором компонента
void TUImagesFrame::LinkToComponent(int i, int j, const std::string &stringid, int index)
{
 if(i <0 || j<0 || i>= DrawGrid->ColCount || j>= DrawGrid->RowCount)
  return;

 StringIds[i][j]=stringid;
 ComponentIndexes[i][j]=index;
 if(ComponentIndexes[i][j].empty())
  Legends[i][j]=std::string(Model_GetComponentLongName(StringIds[i][j].c_str()))+std::string("[")+RDK::sntoa(ComponentIndexesOld[i][j])+"]";
 else
  Legends[i][j]=std::string(Model_GetComponentLongName(StringIds[i][j].c_str()))+std::string("[")+ComponentIndexes[i][j]+"]";

}


// Устанавливает заданное изображение в ячейку с координатами i,j
bool TUImagesFrame::SetBitmap(int i, int j, const RDK::UBitmap &bitmap)
{
 if(i <0 || j<0 || i>= DrawGrid->ColCount || j>= DrawGrid->RowCount)
  return false;

 UBitmapToTBitmap(bitmap,Images[i][j]->Picture->Bitmap,true);
 if(ShowHistogramCheckBox->Checked)
 {
  RDK::UBitmap HistBmp;
  TBitmap *bmp=Images[i][j]->Picture->Bitmap;

  if(bitmap.GetColorModel() == RDK::ubmY8)
  {
   RDK::UBHistogram Hist;
   Hist.Prepare(HistBmp.GetColorModel());
   Hist.Calc(bitmap,0);

   int y=bmp->Height-1;
 //  bmp->Canvas->Rectangle(1,y,256,y-101);

   bmp->Canvas->Pen->Color=clBlack;
   bmp->Canvas->Pen->Style=psSolid;
   bmp->Canvas->Pen->Width=1;
   float divisor=Hist.GetMax().Number.Int;
   if(Hist.GetMax().Number.Int>0)
   for(int i=0;i<Hist.GetSize();i++)
   {
	int item=(float(Hist[i].Number.Int)/divisor)*100.0;
	bmp->Canvas->MoveTo(i+1,y);
	bmp->Canvas->LineTo(i+1,y-item);
   }
  }
  else
  if(bitmap.GetColorModel() == RDK::ubmRGB24)
  {
   RDK::UBHistogram HistR,HistG,HistB;
   HistB.Calc(bitmap,0);
   HistG.Calc(bitmap,1);
   HistR.Calc(bitmap,2);

   int y=bmp->Height-1;
 //  bmp->Canvas->Rectangle(1,y,256,y-101);

   bmp->Canvas->Pen->Style=psSolid;
   bmp->Canvas->Pen->Width=1;
   for(int i=0;i<HistR.GetSize();i++)
   {
	float divisor=HistR.GetMax().Number.Int;
	if(HistR.GetMax().Number.Int>0)
	{
	 bmp->Canvas->Pen->Color=clRed;
	 int item=(float(HistR[i].Number.Int)/divisor)*33.0;
	 bmp->Canvas->MoveTo(i+1,y);
	 bmp->Canvas->LineTo(i+1,y-item);
	}

	divisor=HistG.GetMax().Number.Int;
	if(HistG.GetMax().Number.Int>0)
	{
	 bmp->Canvas->Pen->Color=clGreen;
	 int item=(float(HistG[i].Number.Int)/divisor)*33.0;
	 bmp->Canvas->MoveTo(i+1,y-33);
	 bmp->Canvas->LineTo(i+1,y-item-33);
	}

	divisor=HistB.GetMax().Number.Int;
	if(HistB.GetMax().Number.Int>0)
	{
	 bmp->Canvas->Pen->Color=clBlue;
	 int item=(float(HistB[i].Number.Int)/divisor)*33.0;
	 bmp->Canvas->MoveTo(i+1,y-66);
	 bmp->Canvas->LineTo(i+1,y-item-66);
	}
   }
  }
 }

// SetImage(i, j, bitmap.GetWidth(), bitmap.GetHeight(), bitmap.GetColorModel(), bitmap.GetData());
 return true;
}

bool TUImagesFrame::SetImage(int i, int j, const TImage *image)
{
 if(i <0 || j<0 || i>= DrawGrid->ColCount || j>= DrawGrid->RowCount)
  return false;

 Images[i][j]->Assign(image->Picture->Bitmap);
 return true;
}

bool TUImagesFrame::SetImage(int i, int j, int width, int height, int colormodel, unsigned char *buffer)
{
 struct {
  TLogPalette lpal;
  TPaletteEntry dummy[256];
 } SysPal;
 int linebytelength=0;

 Graphics::TBitmap* target=Images[i][j]->Picture->Bitmap;

 if(!target || !buffer)
  return false;

 target->Width=width;
 target->Height=height;

 if(!width || !height)
  return false;

 switch(colormodel)
 {
 case 1:
 break;

 case 3:
  target->HandleType=bmDIB;
  target->PixelFormat=pf24bit;
  /*
   if(!(width % 4))
	memcpy(target->ScanLine[height-1],buffer,
		  width*height*3);
   else */
   {
	linebytelength=width*3;
	if(ReflectionXFlag)
	 for(int i=0;i<height;++i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
	else
	 for(int i=height-1;i>=height;--i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
   }
 break;

 case 4:
   target->HandleType=bmDIB;
   target->PixelFormat=pf32bit;

/*   if(!(width % 4))
	memcpy(target->ScanLine[height-1],buffer,
		  width*height*4);
   else */
   {
    linebytelength=width*4;
	if(ReflectionXFlag)
	 for(int i=0;i<height;++i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
	else
	 for(int i=height-1;i>=0;--i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
   }
 break;

 case 400:
   target->HandleType=bmDIB;
   target->PixelFormat=pf8bit;
   SysPal.lpal.palVersion = 0x300;
   SysPal.lpal.palNumEntries = 256;
   for(int i=0;i<256;i++)
   {
    SysPal.lpal.palPalEntry[i].peRed=SysPal.lpal.palPalEntry[i].peGreen=SysPal.lpal.palPalEntry[i].peBlue=i;
    SysPal.lpal.palPalEntry[i].peFlags = 0;
   }

   if(target->Palette)
    DeleteObject(target->Palette);
   target->Palette = CreatePalette((const tagLOGPALETTE *)&SysPal.lpal);

 /*  if(!(width % 4))
	memcpy(target->ScanLine[height-1],buffer,
		  width*height);
   else */
   {
    linebytelength=width;
	if(ReflectionXFlag)
	 for(int i=0;i<height;++i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
	else
	 for(int i=height-1;i>=0;--i,buffer+=linebytelength)
	  memcpy(target->ScanLine[i],buffer,
		  linebytelength);
   }
 break;
 }
 return true;
}

// Устанавливает заданное изображение в текущую ячейку
/*bool TImagesFrame::SetBitmap(RDK::UBitmap &bitmap)
{
 return SetBitmap(DrawGrid->Col,DrawGrid->Row,bitmap);
} */

bool TUImagesFrame::SetImage(const TImage *image)
{
 return SetImage(DrawGrid->Col,DrawGrid->Row,image);
}

// Возвращает координаты текущей ячейки
int TUImagesFrame::GetX(void)
{
 return DrawGrid->Col;
}

int TUImagesFrame::GetY(void)
{
 return DrawGrid->Row;
}

// Возвращает изображение в выбранной ячейке
Graphics::TBitmap* TUImagesFrame::GetImage(int i, int j)
{
 if(i <0 || j<0 || i>= DrawGrid->ColCount || j>= DrawGrid->RowCount)
  return 0;

 return Images[i][j]->Picture->Bitmap;
}

// Возвращает изображение в текущей ячейке
Graphics::TBitmap* TUImagesFrame::GetImage(void)
{
 if(DrawGrid->Col>=0 && DrawGrid->Col<int(Images.size())
    && DrawGrid->Row>=0 && DrawGrid->Row<int(Images[DrawGrid->Col].size()))
  return Images[DrawGrid->Col][DrawGrid->Row]->Picture->Bitmap;

 return 0;
}
// --------------------------

// --------------------------
// Методы управления фреймом
// --------------------------
void TUImagesFrame::ABeforeCalculate(void)
{

}

void TUImagesFrame::AAfterCalculate(void)
{

}

void TUImagesFrame::AUpdateInterface(void)
{
 if(DrawGrid->Visible)
 {
  for(size_t i=0;i<Images.size();i++)
  {
   for(size_t j=0;j<Images[i].size();j++)
   {
	 if(ComponentIndexes[i][j].empty())
	 {
	  const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentOutputByIndex(StringIds[i][j].c_str(), ComponentIndexesOld[i][j]);
	  if(bmp)
	   SetBitmap(i, j, *bmp);
	  else
	  {
	   StringIds[i][j].clear();
	   ComponentIndexesOld[i][j]=0;
	   ComponentIndexes[i][j].clear();
	  }
	 }
	 else
	 {
	  const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentOutput(StringIds[i][j].c_str(), ComponentIndexes[i][j].c_str());
	  if(bmp)
	  {
	   SetBitmap(i, j, *bmp);
	  }
	  else
	  {
	   StringIds[i][j].clear();
	   ComponentIndexesOld[i][j]=0;
	   ComponentIndexes[i][j].clear();
	  }
	 }
   }
  }

  if(SizeMode == 0)
  {
   TiledSizeRadioButton->Checked=false;
   OriginalSizeRadioButton->Checked=true;
  }
  else
  if(SizeMode == 1)
  {
   TiledSizeRadioButton->Checked=true;
   OriginalSizeRadioButton->Checked=false;
  }

  if(Images.size()>0 && Images[0].size()>0)
  {
   if(SizeMode == 0)
   {
	DrawGrid->DefaultColWidth=Images[0][0]->Picture->Bitmap->Width;
	DrawGrid->DefaultRowHeight=Images[0][0]->Picture->Bitmap->Height;
   }
   else
   if(SizeMode == 1)
   {
	DrawGrid->DefaultColWidth=DrawGrid->ClientWidth/Images.size()-1;
	DrawGrid->DefaultRowHeight=DrawGrid->ClientHeight/Images[0].size()-1;
   }
  }

  DrawGrid->Repaint();
  DrawGrid->Update();
 }

 if(ScrollBox1->Visible)
 {
  if(DrawGrid->Col < 0 || DrawGrid->Row <0)
   return;

  const RDK::UBitmap* bmp=0;
  if(ComponentIndexes[DrawGrid->Col][DrawGrid->Row].empty())
   bmp=(const RDK::UBitmap*)Model_GetComponentOutputByIndex(StringIds[DrawGrid->Col][DrawGrid->Row].c_str(), ComponentIndexesOld[DrawGrid->Col][DrawGrid->Row]);
  else
   bmp=(const RDK::UBitmap*)Model_GetComponentOutput(StringIds[DrawGrid->Col][DrawGrid->Row].c_str(), ComponentIndexes[DrawGrid->Col][DrawGrid->Row].c_str());
  if(bmp)
   SetBitmap(DrawGrid->Col, DrawGrid->Row, *bmp);

  Graphics::TBitmap * tbmp=Images[DrawGrid->Col][DrawGrid->Row]->Picture->Bitmap;

   if(SizeMode == 0)
   {
	FullImage->Width=tbmp->Width;
	FullImage->Height=tbmp->Height;
	FullImage->Stretch=false;
	FullImage->AutoSize=false;
   }
   else
   if(SizeMode == 1)
   {
	FullImage->Width=ScrollBox1->ClientWidth;
	FullImage->Height=ScrollBox1->ClientHeight;
	FullImage->Stretch=true;
//	FullImage->AutoSize=true;
   }

  FullImage->Picture->Bitmap->Assign(tbmp);
  FullImage->Top=0;
  FullImage->Left=0;
  FullImage->Repaint();
 }

 Sleep(0);
}

// Возврат интерфейса в исходное состояние
void TUImagesFrame::AClearInterface(void)
{
 SetNumCells(0,0);
}



// Сохраняет параметры интерфейса в xml
void TUImagesFrame::ASaveParameters(RDK::USerStorageXML &xml)
{

 xml.WriteInteger("NumCellWidth",GetNumCellWidth());
 xml.WriteInteger("NumCellHeight",GetNumCellHeight());

 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   std::string name=RDK::sntoa(int(i))+std::string("_")+RDK::sntoa(int(j));
   xml.WriteString(std::string("CellName")+name,StringIds[i][j].c_str());
   xml.WriteInteger(std::string("CellIndex")+name,ComponentIndexesOld[i][j]);
   xml.WriteString(std::string("CellIndexNew")+name,ComponentIndexes[i][j]);
   xml.WriteString(std::string("CellLegend")+name,Legends[i][j]);

   xml.WriteString(std::string("CellMouseClickComponent")+name,MouseClickComponents[i][j].first.c_str());
   xml.WriteString(std::string("CellMouseClickProperty")+name,MouseClickComponents[i][j].second.c_str());
  }
 }

 xml.WriteBool("ShowLegendCheckBox",ShowLegendCheckBox->Checked);
 xml.WriteBool("ShowHistogramCheckBox",ShowHistogramCheckBox->Checked);
 xml.WriteInteger("SizeMode",SizeMode);
}

// Загружает параметры интерфейса из xml
void TUImagesFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
 Images.clear();
 StringIds.clear();
 ComponentIndexes.clear();

 int x=xml.ReadInteger("NumCellWidth",1);
 int y=xml.ReadInteger("NumCellHeight",1);
 SetNumCells(x,y);

 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   std::string name=RDK::sntoa(int(i))+std::string("_")+RDK::sntoa(int(j));
   StringIds[i][j]=xml.ReadString(std::string("CellName")+name,"");
   ComponentIndexes[i][j]=xml.ReadString(std::string("CellIndexNew")+name,"");
   ComponentIndexesOld[i][j]=xml.ReadInteger(std::string("CellIndex")+name,-1);
   Legends[i][j]=xml.ReadString(std::string("CellLegend")+name,Legends[i][j].c_str());
   if(Legends[i][j].empty())
   {
	if(ComponentIndexes[i][j].empty())
	 Legends[i][j]=std::string(Model_GetComponentLongName(StringIds[i][j].c_str()))+std::string("[")+RDK::sntoa(ComponentIndexesOld[i][j])+"]";
	else
     Legends[i][j]=std::string(Model_GetComponentLongName(StringIds[i][j].c_str()))+std::string("[")+ComponentIndexes[i][j]+"]";
   }


   MouseClickComponents[i][j].first=xml.ReadString(std::string("CellMouseClickComponent")+name,"");
   MouseClickComponents[i][j].second=xml.ReadString(std::string("CellMouseClickProperty")+name,"");
  }
 }

 ShowLegendCheckBox->Checked=xml.ReadBool("ShowLegendCheckBox",true);
 ShowHistogramCheckBox->Checked=xml.ReadBool("ShowHistogramCheckBox",false);

 SizeMode=xml.ReadInteger("SizeMode",0);

 UpdateInterface();
}
//---------------------------------------------------------------------------
void __fastcall TUImagesFrame::DrawGridDrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State)
{
 if(int(Images.size())<=ACol || int(Images[ACol].size())<=ARow)
  return;

 dynamic_cast<TDrawGrid *>(Sender)->Canvas->
		StretchDraw(Rect, Images[ACol][ARow]->Picture->Graphic);
 if(ShowLegendCheckBox->Checked)
 {
  dynamic_cast<TDrawGrid *>(Sender)->Canvas->TextOut(Rect.Left,Rect.Top,Legends[ACol][ARow].c_str());
 }
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::SaveToBmpClick(TObject *Sender)
{
 if(DrawGrid->Col < 0 || DrawGrid->Row <0)
  return;

 if(!SavePictureDialog->Execute())
  return;

 Images[DrawGrid->Col][DrawGrid->Row]->Picture->SaveToFile(SavePictureDialog->FileName);
}
//---------------------------------------------------------------------------


void __fastcall TUImagesFrame::SelectSourceClick(TObject *Sender)
{
 if(DrawGrid->Col < 0 || DrawGrid->Row <0)
  return;

 MyComponentsListForm->UpdateInterface(true);
 if(MyComponentsListForm->ShowIOSelect() != mrOk)
  return;

 StringIds[DrawGrid->Col][DrawGrid->Row]=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 ComponentIndexes[DrawGrid->Col][DrawGrid->Row]=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::AddColumnClick(TObject *Sender)
{
 SetNumCells(GetNumCellWidth()+1, GetNumCellHeight());
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::AddRowClick(TObject *Sender)
{
 SetNumCells(GetNumCellWidth(), GetNumCellHeight()+1);
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::DeleteColumnClick(TObject *Sender)
{
 if(DrawGrid->Col < 0 || DrawGrid->Row <0)
  return;

 StringIds.erase(StringIds.begin()+DrawGrid->Col);

 ComponentIndexes.erase(ComponentIndexes.begin()+DrawGrid->Col);

 Images.erase(Images.begin()+DrawGrid->Col);

 SetNumCells(GetNumCellWidth()-1,GetNumCellHeight());
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::DeleteRowClick(TObject *Sender)
{
 if(DrawGrid->Col < 0 || DrawGrid->Row <0)
  return;

 for(size_t i=0;i<StringIds.size();i++)
  StringIds[i].erase(StringIds[i].begin()+DrawGrid->Row);

 for(size_t i=0;i<ComponentIndexes.size();i++)
  ComponentIndexes[i].erase(ComponentIndexes[i].begin()+DrawGrid->Row);

 for(size_t i=0;i<Images.size();i++)
  Images[i].erase(Images[i].begin()+DrawGrid->Row);

 SetNumCells(GetNumCellWidth(),GetNumCellHeight()-1);
 UpdateInterface();
}
//---------------------------------------------------------------------------



void __fastcall TUImagesFrame::DrawGridDblClick(TObject *Sender)
{
  if(DrawGrid->Col < 0 || DrawGrid->Row <0)
   return;
  Graphics::TBitmap * bmp=Images[DrawGrid->Col][DrawGrid->Row]->Picture->Bitmap;

  if(bmp && (bmp->Width == 0 || bmp->Height == 0))
   return;
  DrawGrid->Visible=false;
  ScrollBox1->Visible=true;
 // FullImage->Align=alClient;
  FullImage->Width=bmp->Width;
  FullImage->Height=bmp->Height;
  FullImage->Picture->Bitmap->Assign(bmp);
  FullImage->Top=0;
  FullImage->Left=0;
  FullImage->Repaint();
  if(ShowLegendCheckBox->Checked)
  {
   FullImage->Canvas->Font->Size=12;
   if(ComponentIndexes[DrawGrid->Col][DrawGrid->Row].empty())
	FullImage->Canvas->TextOut(0,0,(std::string(Model_GetComponentLongName(StringIds[DrawGrid->Col][DrawGrid->Row].c_str()))+std::string("[")+RDK::sntoa(ComponentIndexesOld[DrawGrid->Col][DrawGrid->Row])+"]").c_str());
   else
    FullImage->Canvas->TextOut(0,0,(std::string(Model_GetComponentLongName(StringIds[DrawGrid->Col][DrawGrid->Row].c_str()))+std::string("[")+ComponentIndexes[DrawGrid->Col][DrawGrid->Row]+"]").c_str());
  }
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::FullImageDblClick(TObject *Sender)
{
  DrawGrid->Visible=true;
  ScrollBox1->Visible=false;
  UpdateInterface(true);
}
//---------------------------------------------------------------------------
void __fastcall TUImagesFrame::FullImageMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
  presIm=false;
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::FullImageMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
  if(presIm)
  {
	ScrollBox1->HorzScrollBar->Position = ScrollBox1->HorzScrollBar->Position + (x2 - X);
	ScrollBox1->VertScrollBar->Position = ScrollBox1->VertScrollBar->Position + (y2 - Y);
  }
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::FullImageMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y)
{
  presIm=true;
  x2=X;
  y2=Y;
}
//---------------------------------------------------------------------------


void __fastcall TUImagesFrame::DrawGridClick(TObject *Sender)
{
 if(MouseClickComponents.empty() || MouseClickComponents[0].empty())
  return;

 if(DrawGrid->Col<0 || DrawGrid->Row<0)
  return;

 // Заглушка!!
 MouseClickComponents[0][0].first="Tracker";
 MouseClickComponents[0][0].second="MouseClickPoint";

 if(Model_CheckComponent(MouseClickComponents[DrawGrid->Col][DrawGrid->Row].first.c_str()) &&
	!strcmp(Model_GetComponentClassName(MouseClickComponents[DrawGrid->Col][DrawGrid->Row].first.c_str()),"TTrackerSample"))
 {
  RDK::UBPoint point;
  TRect rect=DrawGrid->CellRect(DrawGrid->Col, DrawGrid->Row);
  TPoint tpoint=::Mouse->CursorPos;
  tpoint=ScreenToClient(tpoint);

  //DrawGrid->MouseCoord()

  point.X=double(tpoint.X)*double(Images[DrawGrid->Col][DrawGrid->Row]->Picture->Bitmap->Width)/double(rect.Width());
  point.Y=double(tpoint.Y)*double(Images[DrawGrid->Col][DrawGrid->Row]->Picture->Bitmap->Height)/double(rect.Height());
  Model_SetComponentPropertyData(MouseClickComponents[DrawGrid->Col][DrawGrid->Row].first.c_str(), MouseClickComponents[DrawGrid->Col][DrawGrid->Row].second.c_str(), &point);
 }
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::OriginalSizeRadioButtonClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;
 TiledSizeRadioButton->Checked=false;
 SizeMode=0;
 UpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TUImagesFrame::TiledSizeRadioButtonClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;
 OriginalSizeRadioButton->Checked=false;
 SizeMode=1;
 UpdateInterface();
}
//---------------------------------------------------------------------------

