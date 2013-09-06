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
//#include "Libraries/Basic/UBAResize.h"

namespace RDK {

// ****************************************************************************
// Реализация UAFont
// ****************************************************************************
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAFont::UAFont(void)
{
 // Имя шрифта
 Name="Font";

 // Высота шрифта в пикселях
 Height=10;

 // Межсимвольный интервал в процентах от высоты шрифта
 Interval=0;

 Scale=1;

 Size=0;
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
// Методы доступа к параметрам
// --------------------------
// Имя шрифта
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

// Высота шрифта в пикселях
int UAFont::GetHeight(void) const
{
 return Height;
}

// Межсимвольный интервал в процентах от высоты шрифта
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

// Масштаб в % от оригинала
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

// Метрический размер шрифта
int UAFont::GetSize(void) const
{
 return Size;
}

bool UAFont::SetSize(int size)
{
 if(Size == size)
  return true;

 Size=size;
 return true;
}
// --------------------------


// --------------------------
// Методы управления
// --------------------------
// Вычисление длины и высоты строки текста
void UAFont::CalcTextSize(const wstring &str, int &width, int &height)
{
 width=0;
 for(size_t i=0;i<str.size();i++)
  width+=CalcWidth(str[i]);

 if(str.size()>0)
  width+=static_cast<int>(((str.size()-1)*Interval*Scale)/100);

 height=Height;
}

void UAFont::CalcTextSize(const string &str, int &width, int &height)
{
 width=0;
 for(size_t i=0;i<str.size();i++)
  width+=CalcWidth(str[i]);

 if(str.size()>0)
  width+=static_cast<int>(((str.size()-1)*Interval*Scale)/100);

 height=Height;
}

// Вычисление, сколько символов строки, начиная с символа index, войдет по
// ширине в заданное число пикселей
int UAFont::CalcTextLength(const string &str, int index, int width)
{
 int calc_width, calc_height;
 int pos;

 pos=int(str.size())-index;
 CalcTextSize(str.substr(index,pos), calc_width, calc_height);
 while(calc_width>width && pos>0)
 {
  --pos;
  CalcTextSize(str.substr(index,pos), calc_width, calc_height);
 }
 return (pos)?pos:1;
}

int UAFont::CalcTextLength(const wstring &str, int index, int width)
{
 int calc_width, calc_height;
 int pos;

 pos=int(str.size())-index;
 CalcTextSize(str.substr(index,pos), calc_width, calc_height);
 while(calc_width>width && pos>0)
 {
  --pos;
  CalcTextSize(str.substr(index,pos), calc_width, calc_height);
 }
 return (pos)?pos:1;
}

// Отрисовывает заданный символ в текущей позиции канвы
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
 return true;
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
 return true;
}

bool UAFont::DrawRect(const wstring &str, const UBRect &rect, int align, UAGraphics *graphics)
{
 if(!graphics)
  return false;

 int curr_index=0;
 int curr_x=rect.X, curr_y=rect.Y;
 int width,height;
 int num_symbols_in_line=0;
 do
 {
  num_symbols_in_line=CalcTextLength(str,curr_index,rect.Width);
  switch(align)
  {
  case alRight:
   CalcTextSize(str.substr(curr_index,num_symbols_in_line),width,height);
   curr_x=rect.X+rect.Width-width;
  break;

  case alCenter:
   CalcTextSize(str.substr(curr_index,num_symbols_in_line),width,height);
   curr_x=rect.X+(rect.Width-width)/2;
  break;

  case alJustify:
  break;

  case alLeft:
  default:
  ;
  }

  graphics->SetPenPos(curr_x,curr_y);
  Draw(str.substr(curr_index,num_symbols_in_line),graphics);
  curr_index+=num_symbols_in_line;
//  curr_y+=Height+Height/2;
  curr_y+=Height;
 } while(curr_index<int(str.size()));

 return true;
}

bool UAFont::DrawRect(const string &str, const UBRect &rect, int align, UAGraphics *graphics)
{
 if(!graphics)
  return false;

 int curr_index=0;
 int curr_x=rect.X, curr_y=rect.Y;
 int width,height;
 int num_symbols_in_line=0;
 do
 {
  num_symbols_in_line=CalcTextLength(str,curr_index,rect.Width);
  if(num_symbols_in_line == int(str.size()))
   curr_y+=(rect.Height-(Height*3)/2)/2;
  switch(align)
  {
  case alRight:
   CalcTextSize(str.substr(curr_index,num_symbols_in_line),width,height);
   curr_x=rect.X+rect.Width-width;
  break;

  case alCenter:
   CalcTextSize(str.substr(curr_index,num_symbols_in_line),width,height);
   curr_x=rect.X+(rect.Width-width)/2;
  break;

  case alJustify:
  break;

  case alLeft:
  default:
  ;
  }

  graphics->SetPenPos(curr_x,curr_y);
  Draw(str.substr(curr_index,num_symbols_in_line),graphics);
  curr_index+=num_symbols_in_line;
//  curr_y+=Height+Height/2;
  curr_y+=Height;
 } while(curr_index<int(str.size()));

 return true;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор копирования
UAFont& UAFont::operator = (const UAFont &font)
{
 // Имя шрифта
 Name=font.Name;

 // Высота шрифта в пикселях
 Height=font.Height;

 // Межсимвольный интервал в процентах от высоты шрифта
 Interval=font.Interval;

 Scale=font.Scale;

 Size=font.Size;

 return *this;
}
// --------------------------


// ****************************************************************************
// Реализация UBitmapFont
// ****************************************************************************
// --------------------------
// Конструкторы и деструкторы
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
// Методы доступа к параметрам
// --------------------------
// Масштаб в % от оригинала
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
//   UBResizeEdges(I->second.Data,ScaledTable[I->first].Data);
   ++I;
  }
 }
 return true;
}
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Возвращает ширину объекта в пикселах
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
// Методы загрузки и сохранения
// --------------------------
// Загружает данные заданного символа ch из файла
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
//  UBResizeEdges(Table[ch].Data,ScaledTable[ch].Data);
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

// Загружает/сохраняет данные символов с кодами из интервала [ch1,ch2] из файла
// Область символов в файле описывается прямоугольником rect
// размер символа size;
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
//   UBResizeEdges(char_bmp,ScaledTable[k].Data);
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

// Загружает весь шрифт из файла
bool UBitmapFont::LoadFromFile(const string &font_name, const string &font_file_name, int size)
{
 std::size_t dir_sep_pos=font_file_name.find_last_of("\\/");
 std::string font_path;
 if(dir_sep_pos != string::npos)
  font_path=font_file_name.substr(0,dir_sep_pos+1);

 UIniFile<char> ini;
 ini.LoadFromFile(font_file_name);

 std::string image_file_name=font_path;
 image_file_name+=ini("HGEFONT","Bitmap","");

 basic_ifstream<char> file(image_file_name.c_str(),ios::in | ios::binary);
 UBitmap bmp;

 if(!file)
  return false;

 file>>bmp;
 bmp.ReflectionX();

 vector<string> vals;
 vector<string> params;
 ini.GetVariableList("HGEFONT",vals);

 int k=0;
 for(size_t i=0;i<vals.size();i++)
 {
  string value=vals[i];
  if(value == "Char")
  {
   params.clear();
   separatestring(ini("HGEFONT",value,"",k++),params,',');
   if(!params.empty())
   {
    int ch=0;
	if(params[0][0] == '"')
	{
     ch=params[0][1];
	}
	else
	{
     ch=hextoi(params[0]);
	}
	UBitmap &char_bmp=Table[ch].Data;
	int x,y,w,h;
	x=RDK::atoi(params[1]);
	y=RDK::atoi(params[2]);
	w=RDK::atoi(params[3]);
	h=RDK::atoi(params[4]);

	char_bmp.SetRes(w,h, bmp.GetColorModel());
	bmp.GetRect(x, y,char_bmp);
   }

  }
 }

 Height=size;
 Name=font_name;
 Size=size;
 return true;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор копирования
UBitmapFont& UBitmapFont::operator = (const UBitmapFont &font)
{
 static_cast<UAFont * const>(this)->operator = (font);
 Table=font.Table;
 ScaledTable=font.ScaledTable;
 return *this;
};
// --------------------------

// --------------------------
// Скрытые методы управления
// --------------------------
// Отрисовывает заданный символ в текущей позиции канвы
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



// Добавляет шрифт
bool UBitmapFontCollection::AddFont(const string &name, int size, UBitmapFont &font)
{
 Fonts[name][size]=font;
 return true;
}

// Удаляет шрифт
bool UBitmapFontCollection::DelFont(const string &name)
{
 return true;
}

bool UBitmapFontCollection::DelFont(const string &name, int size)
{
 return true;
}

// Возвращает список имен шрифтов
void UBitmapFontCollection::GetFontNames(vector<string> &buffer)
{
 map<string,FontSizeContainerT>::iterator I,J;
 I=Fonts.begin(); J=Fonts.end();
 buffer.clear();
 buffer.reserve(Fonts.size());
 for(;I!=J;++I)
  buffer.push_back(I->first);
}

// Возвращает список имен шрифтов
void UBitmapFontCollection::GetFontSizes(const string &name, vector<int> &buffer)
{
 buffer.clear();

 map<int,UBitmapFont>::iterator I,J;
 map<string,FontSizeContainerT>::iterator fI=Fonts.find(name);

 if(fI == Fonts.end())
  return;

 I=fI->second.begin(); J=fI->second.end();
 buffer.reserve(fI->second.size());
 for(;I!=J;++I)
  buffer.push_back(I->first);
}

// Удаляет все шрифты
void UBitmapFontCollection::DelAllFonts(void)
{
 Fonts.clear();
}

// Возвращает текущий шрифт
UAFont* UBitmapFontCollection::GetFont(const string &name, int size)
{
 return &Fonts[name][size];
}


// Возвращает шрифт по умолчанию
UAFont* UBitmapFontCollection::GetDefaultFont(void)
{
 if(Fonts.begin() != Fonts.end())
 {
  map<string,FontSizeContainerT>::iterator fI=Fonts.begin();
  if(fI->second.begin() != fI->second.end())
  {
   return &fI->second.begin()->second;
  }
 }
 return 0;
}


}

#endif
