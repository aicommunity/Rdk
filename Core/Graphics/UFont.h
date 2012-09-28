/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UFontH
#define UFontH

#include <vector>
#include <map>
#include <string>
#include "../Utilities/UIniFile.h"
#include "UBitmap.h"

namespace RDK {

class UAGraphics;

// Описание линии
class UFontLine
{
public:
int x1,y1,x2,y2;
};


// Описание растрового символа
class UBitmapFontSymbol
{
public:
// Изображение символа
UBitmap Data;
};

// Описание векторного символа
class UVectorFontSymbol
{
public:
// Набор линий, описывающий символ
// от начала координат, расположенным в правом верхнем углу символа
std::vector<UFontLine> Lines;
};

class UAFont
{
protected:

protected: // Параметры
// Имя шрифта
string Name;

// Высота шрифта в пикселях
int Height;

// Межсимвольный интервал в процентах от высоты шрифта
float Interval;

// Масштаб в % от оригинала
float Scale;

protected: // Данные

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UAFont(void);
UAFont(const UAFont &font);
~UAFont(void);
// --------------------------

// --------------------------
// Методы доступа к параметрам
// --------------------------
// Имя шрифта
const string& GetName(void) const;
virtual bool SetName(const string& name);

// Высота шрифта в пикселях
int GetHeight(void) const;

// Межсимвольный интервал в процентах от высоты шрифта
float GetInterval(void) const;
virtual bool SetInterval(float value);

// Масштаб в % от оригинала
float GetScale(void) const;
virtual bool SetScale(float value);
// --------------------------


// --------------------------
// Методы управления
// --------------------------
// Возвращает ширину объекта в пикселах
virtual int CalcWidth(wchar_t ch)=0;
virtual int CalcWidth(char ch)=0;
int CalcWidth(const wstring &str);
int CalcWidth(const string &str);

// Отрисовывает заданный символ в текущей позиции канвы
bool Draw(wchar_t ch, UAGraphics *graphics);
bool Draw(char ch, UAGraphics *graphics);
bool Draw(const wstring &str, UAGraphics *graphics);
bool Draw(const string &str, UAGraphics *graphics);
// --------------------------

// --------------------------
// Методы загрузки и сохранения
// Загружает данные заданного символа ch из файла
virtual bool Load(wchar_t ch, const string &filename)=0;
virtual bool Save(wchar_t ch, const string &filename)=0;
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор копирования
UAFont& operator = (const UAFont &font);
// --------------------------

// --------------------------
// Скрытые методы управления
// --------------------------
protected:
// Отрисовывает заданный символ в текущей позиции канвы
virtual void DrawSymbol(wchar_t ch, UAGraphics *graphics)=0;
virtual void DrawSymbol(char ch, UAGraphics *graphics)=0;
// --------------------------
};


class UBitmapFont: public UAFont
{
protected: // Параметры


protected: // Данные
// Таблица описаний символов
std::map<wchar_t,UBitmapFontSymbol> Table;

// Таблица масштабированных описаний символов
std::map<wchar_t,UBitmapFontSymbol> ScaledTable;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UBitmapFont(void);
UBitmapFont(const UBitmapFont &font);
~UBitmapFont(void);
// --------------------------

// --------------------------
// Методы доступа к параметрам
// --------------------------
// Масштаб в % от оригинала
bool SetScale(float value);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Возвращает ширину объекта в пикселах
int CalcWidth(wchar_t ch);
int CalcWidth(char ch);
// --------------------------

// --------------------------
// Методы загрузки и сохранения
// --------------------------
// Загружает/сохраняет данные заданного символа ch из файла
bool Load(wchar_t ch, const string &filename);
bool Save(wchar_t ch, const string &filename);

// Загружает/сохраняет данные символов с кодами из интервала [ch1,ch2] из файла
// Область символов в файле описывается прямоугольником rect
// размер символа size;
bool Load(const string &filename, const UBPoint &size=UBPoint(16,16), const UBRect &rect=UBRect(-1,-1,-1,-1), wchar_t ch1=0, wchar_t ch2=255);
bool Save(const string &filename, const UBPoint &size=UBPoint(16,16), const UBRect &rect=UBRect(-1,-1,-1,-1), wchar_t ch1=0, wchar_t ch2=255);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор копирования
UBitmapFont& operator = (const UBitmapFont &font);
// --------------------------

// --------------------------
// Скрытые методы управления
// --------------------------
protected:
// Отрисовывает заданный символ в текущей позиции канвы
void DrawSymbol(wchar_t ch, UAGraphics *graphics);
void DrawSymbol(char ch, UAGraphics *graphics);
// --------------------------
};

class UVectorFont: public UAFont
{
protected: // Параметры
// Толщина линий шрифта
int PenSize;

protected: // Данные
// Таблица описаний символов
std::map<wchar_t,UVectorFontSymbol> Table;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UVectorFont(void);
UVectorFont(const UVectorFont &font);
~UVectorFont(void);
// --------------------------

// --------------------------
// Методы управления
// --------------------------
// Возвращает ширину объекта в пикселах
int CalcWidth(wchar_t ch);
int CalcWidth(char ch);
// --------------------------

// --------------------------
// Методы загрузки и сохранения
// --------------------------
// Загружает данные заданного символа ch из файла
bool Load(wchar_t ch, const string &filename);
bool Save(wchar_t ch, const string &filename);
// --------------------------

// --------------------------
// Операторы
// --------------------------
// Оператор копирования
UVectorFont& operator = (const UVectorFont &font);
// --------------------------

// --------------------------
// Скрытые методы управления
// --------------------------
protected:
// Отрисовывает заданный символ в текущей позиции канвы
void DrawSymbol(wchar_t ch, UAGraphics *graphics);
void DrawSymbol(char ch, UAGraphics *graphics);
// --------------------------
};

}

#endif

