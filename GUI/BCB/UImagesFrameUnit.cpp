//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UImagesFrameUnit.h"
#include "UComponentsListFormUnit.h"
#include "rdk_initdll.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUImagesFrame *UImagesFrame;
//---------------------------------------------------------------------------
__fastcall TUImagesFrame::TUImagesFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 ReflectionXFlag=false;
 MyComponentsListForm=new TUComponentsListForm(this);
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
 for(size_t i=0;i<Images.size();i++)
 {
  Images[i].resize(height);
  StringIds[i].resize(height);
  ComponentIndexes[i].resize(height);
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
}


// Устанавливает заданное изображение в ячейку с координатами i,j
bool TUImagesFrame::SetBitmap(int i, int j, const RDK::UBitmap &bitmap)
{
 if(i <0 || j<0 || i>= DrawGrid->ColCount || j>= DrawGrid->RowCount)
  return false;

// bitmap>>Images[i][j]->Picture->Bitmap;

 SetImage(i, j, bitmap.GetWidth(), bitmap.GetHeight(), bitmap.GetColorModel(), bitmap.GetData());
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
 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   const RDK::UBitmap* bmp=(const RDK::UBitmap*)Model_GetComponentOutput(StringIds[i][j].c_str(), ComponentIndexes[i][j]);
   if(bmp)
	SetBitmap(i, j, *bmp);
  }
 }
 DrawGrid->Repaint();
 DrawGrid->Update();
}


// Сохраняет параметры интерфейса в xml
void TUImagesFrame::ASaveParameters(RDK::Serialize::USerStorageXML &xml)
{

 xml.WriteInteger("NumCellWidth",GetNumCellWidth());
 xml.WriteInteger("NumCellHeight",GetNumCellHeight());

 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   std::string name=RDK::sntoa(int(i))+std::string("_")+RDK::sntoa(int(j));
   xml.WriteString(std::string("CellName")+name,StringIds[i][j].c_str());
   xml.WriteInteger(std::string("CellIndex")+name,ComponentIndexes[i][j]);
  }
 }

 xml.WriteBool("ShowLegendCheckBox",ShowLegendCheckBox->Checked);
}

// Загружает параметры интерфейса из xml
void TUImagesFrame::ALoadParameters(RDK::Serialize::USerStorageXML &xml)
{
 int x=xml.ReadInteger("NumCellWidth",1);
 int y=xml.ReadInteger("NumCellHeight",1);
 SetNumCells(x,y);

 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   std::string name=RDK::sntoa(int(i))+std::string("_")+RDK::sntoa(int(j));
   StringIds[i][j]=xml.ReadString(std::string("CellName")+name,"");
   ComponentIndexes[i][j]=xml.ReadInteger(std::string("CellIndex")+name,0);
  }
 }

 ShowLegendCheckBox->Checked=xml.ReadBool("ShowLegendCheckBox",true);

 UpdateInterface();
}
/*
// Сохраняет информацию об источниках данных в заданный ini файл
void TUImagesFrame::SaveToIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return;

 ini->WriteInteger(section,"NumCellWidth",GetNumCellWidth());
 ini->WriteInteger(section,"NumCellHeight",GetNumCellHeight());

 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   String name=IntToStr(int(i))+String("_")+IntToStr(int(j));
   ini->WriteString(section,String("CellName")+name,StringIds[i][j].c_str());
   ini->WriteInteger(section,String("CellIndex")+name,ComponentIndexes[i][j]);
  }
 }
}

// Загружает информацию об источниках данных из заданного ini файла
void TUImagesFrame::LoadFromIni(TMemIniFile *ini, const String &section)
{
 if(!ini)
  return;

 int x=ini->ReadInteger(section,"NumCellWidth",1);
 int y=ini->ReadInteger(section,"NumCellHeight",1);
 SetNumCells(x,y);

 for(size_t i=0;i<Images.size();i++)
 {
  for(size_t j=0;j<Images[i].size();j++)
  {
   String name=IntToStr(int(i))+String("_")+IntToStr(int(j));
   StringIds[i][j]=AnsiString(ini->ReadString(section,String("CellName")+name,"")).c_str();
   ComponentIndexes[i][j]=ini->ReadInteger(section,String("CellIndex")+name,0);
  }
 }
 UpdateInterface();
} */
// --------------------------

//---------------------------------------------------------------------------
void __fastcall TUImagesFrame::DrawGridDrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State)
{
 if(int(Images.size())<=ACol || int(Images[ACol].size())<=ARow)
  return;

 dynamic_cast<TDrawGrid *>(Sender)->Canvas->
		StretchDraw(Rect, Images[ACol][ARow]->Picture->Graphic);
 if(ShowLegendCheckBox->Checked)
  dynamic_cast<TDrawGrid *>(Sender)->Canvas->TextOut(Rect.Left,Rect.Top,Model_GetComponentLongName(StringIds[ACol][ARow].c_str()));
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

 if(MyComponentsListForm->ShowComponentSelect() != mrOk)
  return;

 StringIds[DrawGrid->Col][DrawGrid->Row]=MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongId();
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

