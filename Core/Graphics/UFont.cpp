/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UFONT_CPP
#define UFONT_CPP

#include "UFont.h"
#include "UAGraphics.h"
#include "UGraphicsIO.h"
#include "Libraries/Basic/UBAResize.h"

namespace RDK {

// ****************************************************************************
// ���������� UAFont
// ****************************************************************************
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UAFont::UAFont(void)
{
 // ��� ������
 Name="Font";

 // ������ ������ � ��������
 Height=16;

 // ������������� �������� � ��������� �� ������ ������
 Interval=-35;

 Scale=1;
}

UAFont::UAFont(const UAFont &font)
{
 *this=font;
}

UAFont::~UAFont(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ����������
// --------------------------
// ��� ������
const string& UAFont::GetName(void) const
{
 return Name;
}

bool UAFont::SetName(const string& name)
{
 if(Name == name)
  return true;

 Name=name;
 return true;
}

// ������ ������ � ��������
int UAFont::GetHeight(void) const
{
 return Height;
}

// ������������� �������� � ��������� �� ������ ������
float UAFont::GetInterval(void) const
{
 return Interval;
}

bool UAFont::SetInterval(float value)
{
 if(Interval == value)
  return true;

 Interval=value;
 return true;
}

// ������� � % �� ���������
float UAFont::GetScale(void) const
{
 return Scale;
}

bool UAFont::SetScale(float value)
{
 if(Scale == value)
  return true;

 Scale=value;
 return true;
}
// --------------------------


// --------------------------
// ������ ����������
// --------------------------
int UAFont::CalcWidth(const wstring &str)
{
 int res=0;
 for(size_t i=0;i<str.size();i++)
  res+=CalcWidth(str[i]);

 if(str.size()>0)
  res+=static_cast<int>(((str.size()-1)*Interval*Scale)/100);

 return res;
}

int UAFont::CalcWidth(const string &str)
{
 int res=0;
 for(size_t i=0;i<str.size();i++)
  res+=CalcWidth(str[i]);

 if(str.size()>0)
  res+=static_cast<int>(((str.size()-1)*Interval*Scale)/100);

 return res;
}


// ������������ �������� ������ � ������� ������� �����
bool UAFont::Draw(wchar_t ch, UAGraphics *graphics)
{
 if(!graphics)
  return false;

 DrawSymbol(ch,graphics);

 return true;
}

bool UAFont::Draw(char ch, UAGraphics *graphics)
{
 if(!graphics)
  return false;

 DrawSymbol(ch,graphics);

 return true;
}

bool UAFont::Draw(const wstring &str, UAGraphics *graphics)
{
 if(!graphics)
  return false;

 int x=graphics->GetPenX();
 int y=graphics->GetPenY();
 for(size_t i=0;i<str.size();i++)
 {
  DrawSymbol(str[i],graphics);
  int width=CalcWidth(str[i]);
  x+=static_cast<int>(width+(Interval*Scale*width)/100);
  graphics->SetPenPos(x,y);
 }
 return false;
}

bool UAFont::Draw(const string &str, UAGraphics *graphics)
{
 if(!graphics)
  return false;

 int x=graphics->GetPenX();
 int y=graphics->GetPenY();
 for(size_t i=0;i<str.size();i++)
 {
  DrawSymbol(str[i],graphics);
  int width=CalcWidth(str[i]);
  x+=static_cast<int>(width+(Interval*Scale*width)/100);
  graphics->SetPenPos(x,y);
 }
 return false;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� �����������
UAFont& UAFont::operator = (const UAFont &font)
{
 // ��� ������
 Name=font.Name;

 // ������ ������ � ��������
 Height=font.Height;

 // ������������� �������� � ��������� �� ������ ������
 Interval=font.Interval;

 Scale=font.Scale;

 return *this;
}
// --------------------------


// ****************************************************************************
// ���������� UBitmapFont
// ****************************************************************************
// --------------------------
// ������������ � �����������
// --------------------------
UBitmapFont::UBitmapFont(void)
 : UAFont()
{
}

UBitmapFont::UBitmapFont(const UBitmapFont &font)
 : UAFont(font)
{
 *this=font;
}

UBitmapFont::~UBitmapFont(void)
{
}
// --------------------------

// --------------------------
// ������ ������� � ����������
// --------------------------
// ������� � % �� ���������
bool UBitmapFont::SetScale(float value)
{
 if(!UAFont::SetScale(value))
  return false;

 if(Scale == 1)
  ScaledTable.clear();
 else
 {
  std::map<wchar_t,UBitmapFontSymbol>::iterator I,J;

  I=Table.begin();
  J=Table.end();
  while(I != J)
  {
   ScaledTable[I->first].Data.SetRes(static_cast<int>(I->second.Data.GetWidth()*Scale),
                                     static_cast<int>(I->second.Data.GetHeight()*Scale),
                                     I->second.Data.GetColorModel());
   UBResizeEdges(I->second.Data,ScaledTable[I->first].Data);
   ++I;
  }
 }
 return true;
}
// --------------------------

// --------------------------
// ������ ����������
// --------------------------
// ���������� ������ ������� � ��������
int UBitmapFont::CalcWidth(wchar_t ch)
{
 return static_cast<int>(Table[ch].Data.GetWidth()*Scale);
}

int UBitmapFont::CalcWidth(char ch)
{
 return static_cast<int>(Table[ch].Data.GetWidth()*Scale);
}
// --------------------------

// --------------------------
// ������ �������� � ����������
// --------------------------
// ��������� ������ ��������� ������� ch �� �����
bool UBitmapFont::Load(wchar_t ch, const string &filename)
{
 basic_ifstream<char> file(filename.c_str(),ios::in | ios::binary);

 if(!file)
  return false;

 file>>Table[ch].Data;
 Table[ch].Data.ReflectionX();
 if(Scale != 1)
 {
  ScaledTable[ch].Data.SetRes(static_cast<int>(Table[ch].Data.GetWidth()*Scale),
                               static_cast<int>(Table[ch].Data.GetHeight()*Scale),
                               Table[ch].Data.GetColorModel());
  UBResizeEdges(Table[ch].Data,ScaledTable[ch].Data);
 }

 return true;
};

bool UBitmapFont::Save(wchar_t ch, const string &filename)
{
 basic_ofstream<char> file(filename.c_str(),ios::out | ios::binary);

 if(!file)
  return false;

 file<<Table[ch].Data;

 return true;
};

// ���������/��������� ������ �������� � ������ �� ��������� [ch1,ch2] �� �����
// ������� �������� � ����� ����������� ��������������� rect
// ������ ������� size;
bool UBitmapFont::Load(const string &filename, const UBPoint &size, const UBRect &rect, wchar_t ch1, wchar_t ch2)
{
 basic_ifstream<char> file(filename.c_str(),ios::in | ios::binary);
 UBitmap bmp;

 if(!file)
  return false;

 file>>bmp;
 bmp.ReflectionX();

 int i=0,j=0;
// int step_x=
 for(wchar_t k=ch1;k<ch2;k++)
 {
  UBitmap &char_bmp=Table[k].Data;
  char_bmp.SetRes(size.X,size.Y, bmp.GetColorModel());
  bmp.GetRect(i*size.X, j*size.Y,char_bmp);
  if(Scale != 1)
  {
   ScaledTable[k].Data.SetRes(static_cast<int>(char_bmp.GetWidth()*Scale),
							   static_cast<int>(char_bmp.GetHeight()*Scale),
							   char_bmp.GetColorModel());
   UBResizeEdges(char_bmp,ScaledTable[k].Data);
  }
  i+=size.X;
  if(i>=bmp.GetWidth()-size.X)
  {
   i=0;
   j+=size.Y;
  }
 }

 return true;
}

bool UBitmapFont::Save(const string &filename, const UBPoint &size, const UBRect &rect, wchar_t ch1, wchar_t ch2)
{

 return true;
}
// --------------------------

// --------------------------
// ���������
// --------------------------
// �������� �����������
UBitmapFont& UBitmapFont::operator = (const UBitmapFont &font)
{
 static_cast<UAFont * const>(this)->operator = (font);
 Table=font.Table;
 ScaledTable=font.ScaledTable;
 return *this;
};
// --------------------------

// --------------------------
// ������� ������ ����������
// --------------------------
// ������������ �������� ������ � ������� ������� �����
void UBitmapFont::DrawSymbol(wchar_t ch, UAGraphics *graphics)
{
 if(Scale == 1)
  graphics->Bitmap(graphics->GetPenX(), graphics->GetPenY(), Table[ch].Data,2);
 else
  graphics->Bitmap(graphics->GetPenX(), graphics->GetPenY(), ScaledTable[ch].Data,2);
}

void UBitmapFont::DrawSymbol(char ch, UAGraphics *graphics)
{
 if(Scale == 1)
  graphics->Bitmap(graphics->GetPenX(), graphics->GetPenY(), Table[ch].Data,2);
 else
  graphics->Bitmap(graphics->GetPenX(), graphics->GetPenY(), ScaledTable[ch].Data,2);
}
// --------------------------

}

#endif
