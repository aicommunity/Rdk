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
class RDK_LIB_TYPE UFontLine
{
public:
int x1,y1,x2,y2;
};


// Описание растрового символа
class RDK_LIB_TYPE UBitmapFontSymbol
{
public:
// Изображение символа
UBitmap Data;
};

// Описание векторного символа
class RDK_LIB_TYPE UVectorFontSymbol
{
public:
// Набор линий, описывающий символ
// от начала координат, расположенным в правом верхнем углу символа
std::vector<UFontLine> Lines;
};

class RDK_LIB_TYPE UAFont
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

// Метрический размер шрифта
int Size;

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

// Метрический размер шрифта
int GetSize(void) const;
virtual bool SetSize(int size);
// --------------------------


// --------------------------
// Методы управления
// --------------------------
// Возвращает ширину объекта в пикселах
virtual int CalcWidth(wchar_t ch)=0;
virtual int CalcWidth(char ch)=0;

// Вычисление длины и высоты строки текста
void CalcTextSize(const wstring &str, int &width, int &height);
void CalcTextSize(const string &str, int &width, int &height);

// Вычисление, сколько символов строки, начиная с символа index, войдет по
// ширине в заданное число пикселей
virtual int CalcTextLength(const string &str, int index, int width);
virtual int CalcTextLength(const wstring &str, int index, int width);

// Отрисовывает заданный символ в текущей позиции канвы
bool Draw(wchar_t ch, UAGraphics *graphics);
bool Draw(char ch, UAGraphics *graphics);
bool Draw(const wstring &str, UAGraphics *graphics);
bool Draw(const string &str, UAGraphics *graphics);

bool DrawRect(const wstring &str, const UBRect &rect, int align, UAGraphics *graphics);
bool DrawRect(const string &str, const UBRect &rect, int align, UAGraphics *graphics);
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


class RDK_LIB_TYPE UBitmapFont: public UAFont
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

// Загружает весь шрифт из файла
bool LoadFromFile(const string &font_name, const string &font_file_name, int size);
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
			   /*
class RDK_LIB_TYPE UVectorFont: public UAFont
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
                  */
class RDK_LIB_TYPE UFontCollection
{
public:
// --------------------------
// Методы доступа к данным
// --------------------------
// Добавляет шрифт
//bool AddFont(const string &name, UBitmapFont &font);

// Удаляет шрифт
virtual bool DelFont(const string &name)=0;
virtual bool DelFont(const string &name, int size)=0;

// Возвращает список имен шрифтов
virtual void GetFontNames(vector<string> &buffer)=0;

// Возвращает список имен шрифтов
virtual void GetFontSizes(const string &name, vector<int> &buffer)=0;

// Удаляет все шрифты
virtual void DelAllFonts(void)=0;

// Возвращает шрифт
virtual UAFont* GetFont(const string &name, int size)=0;
// --------------------------

};

class RDK_LIB_TYPE UBitmapFontCollection
{
public:
typedef map<int,UBitmapFont> FontSizeContainerT;
typedef map<string,FontSizeContainerT> FontContainerT;

protected:
FontContainerT Fonts;

public:
// --------------------------
// Методы доступа к данным
// --------------------------
// Добавляет шрифт
bool AddFont(const string &name, int size, UBitmapFont &font);

// Удаляет шрифт
bool DelFont(const string &name);
bool DelFont(const string &name, int size);

// Возвращает список имен шрифтов
void GetFontNames(vector<string> &buffer);

// Возвращает список имен шрифтов
void GetFontSizes(const string &name, vector<int> &buffer);

// Удаляет все шрифты
void DelAllFonts(void);

// Возвращает шрифт
UAFont* GetFont(const string &name, int size);

// Возвращает шрифт по умолчанию
UAFont* GetDefaultFont(void);
// --------------------------

};

}

#endif

