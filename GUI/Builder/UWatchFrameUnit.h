//---------------------------------------------------------------------------


#ifndef UWatchFrameUnitH
#define UWatchFrameUnitH
#include <Classes.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ExtDlgs.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <VCLTee.Series.hpp>

#include <ComCtrls.hpp>
#include <ToolWin.hpp>
#include <ImgList.hpp>

#include <IniFiles.hpp>
#include <jpeg.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
#include <Menus.hpp>


#include <string>
#include <map>
#include <vector>
#include "myrdk.h"

using namespace std;

//typedef float real;
//---------------------------------------------------------------------------
/*
   Вспомогательный класс - контейнер данных, характеризующих наблюдаемую точку
съёма.
*/
//---------------------------------------------------------------------------
class TUWatchInfo
{
public: // Связи
// Индекс графика в массиве серий
int SeriesIndex;

// Указатель на источник данных
void* DataSource;

// Альтернативный указатель на имя компонента-источника данных
string DataSourceName;

// Индекс выхода
int OutputIndex;

// Индекс элемента выхода
int OutputElementIndex;

public: // Данные графика
const double *X;
const double *Y;
int XYSize;

// Кеш максимума и минимума за заданный интервал времени
double XCurrentMax,YCurrentMax;
double XCurrentMin,YCurrentMin;

// Тип наблюдения
// true - в массивах X,Y хранится набор всех значений графика
// и вся серия обновляется целиком на каждом шаге
// false -
// на каждом шаге новый элемент добавляется в конец
bool FullUpdate;

// Интервал наблюдения, старые данные автоматически удаляются
// игнорируется если <0, или если FullUpdate == true
double WatchInterval;

// Флаг видимости
// Если true - серия отображается, иначе - нет
bool Visible;

public: // Оформление графика
// Смещение графиков серий друг относительно друга по Y
double YShift;

// Цвет графика
TColor Color;

// Стиль линии графика
TPenStyle Style;

// Подпись графика
string Legend;

// Толщина линии
int LineWidth;

public: // Методы
TUWatchInfo(void);
~TUWatchInfo(void);
TUWatchInfo(const TUWatchInfo &wd);
TUWatchInfo& operator = (const TUWatchInfo& wd);
};
        
//---------------------------------------------------------------------------
class TUWatchFrame : public TFrame
{
__published:	// IDE-managed Components
        TChart *Chart1;
	TSavePictureDialog *SavePictureDialog;
	TPopupMenu *PopupMenu;
	TMenuItem *N1;
	TMenuItem *bmp1;
	TMenuItem *AddWatch1;
	TMenuItem *N2;
	void __fastcall N1Click(TObject *Sender);
	void __fastcall bmp1Click(TObject *Sender);
	void __fastcall AddWatch1Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TUWatchFrame(TComponent* Owner);
        virtual __fastcall ~TUWatchFrame(void);

protected:	// Параметры
// Размер кеша отображаемых данных
int CacheSize;

protected:	// Данные
        // -----------------------------------------
        // Данные об наблюдаемых точках съёма данных
        // -----------------------------------------
        // Список имён наблюдаемых точек съёма, и смещений
        // графиков серий друг относительно друга по оси Y.
		vector<TUWatchInfo> NameList;

		// Текущий индекс накопления в кеше
		int CacheIndex;
        // -----------------------------------------

        // -----------------------------------------
		// Данные о состоянии
        // -----------------------------------------
        // Содержит 'true' если данные серий были изменены
        // или были добавлены/удалены новые серии
        bool ModifyState;
        // -----------------------------------------

public:	// Методы
		// ------------------------------
		// Методы настройки отображения
		// ------------------------------
		// Включает/отключает автомасштабирование по оси X
		void __fastcall SetXAutoScale(bool value);

		// Возвращает состояние автомасштабирования по оси X
		bool __fastcall GetXAutoScale(void);

		// Включает/отключает автомасштабирование по оси Y
		void __fastcall SetYAutoScale(bool value);

		// Возвращает состояние автомасштабирования по оси Y
		bool __fastcall GetYAutoScale(void);

		// Устанавливает границы изменения по оси X
		void __fastcall SetXMin(double value);
		void __fastcall SetXMax(double value);

		// Устанавливает границы изменения по оси Y
		void __fastcall SetYMin(double value);
		void __fastcall SetYMax(double value);

		// Возвращает границы изменения по оси X
		double __fastcall GetXMin(void);
		double __fastcall GetXMax(void);

		// Возвращает границы изменения по оси Y
		double __fastcall GetYMin(void);
		double __fastcall GetYMax(void);

		// Устанавливает заголовок графика
		void __fastcall SetGraphTitle(AnsiString title);

		// Устанавливает видимость подписей по осям
		void __fastcall SetXLabelVisible(bool value);
		void __fastcall SetYLabelVisible(bool value);

		// Возвращает видимость подписей по осям
		bool __fastcall GetXLabelVisible(void);
		bool __fastcall GetYLabelVisible(void);

		// Устанавливает подписи по осям
		void __fastcall SetXLabelTitle(AnsiString value);
		void __fastcall SetYLabelTitle(AnsiString value);

		// Возвращает подписи по осям
		AnsiString __fastcall GetXLabelTitle(void);
		AnsiString __fastcall GetYLabelTitle(void);

		// Устанавливает видимость легенды
		void __fastcall SetLegendVisible(bool value);

		// Возвращает видимость легенды
		bool __fastcall GetLegendVisible(void);

		// Устанавливает положение легенды
		// 0 - Слева
		// 1 - Слева (на графике)
		// 2 - Справа
		// 3 - Справа (на графике)
		// 4 - Сверху
		// 5 - Снизу
		void __fastcall SetLegendPosition(int value);

		// Возвращает положение легенды
		int __fastcall GetLegendPosition(void);

		// Размер кеша отображаемых данных
		int GetCacheSize(void) const;
		bool SetCacheSize(int value);
		// ------------------------------

		// ------------------------------
        // Методы управления состоянием
        // ------------------------------
        // Возвращает 'true', если данные в сериях были изменены,
        // или если серии были добавлены/удалены
		bool __fastcall GetModifyState(void);

		// Сохранение и загрузка описания графика в файл
		bool SaveToIni(TMemIniFile *ini, const String &section);
		bool LoadFromIni(TMemIniFile *ini, const String &section);

		// Собирает информацию об открытых сериях в файл 'watchname'
        // Если 'collectstate' == 'true', то сохраняет также накопленную информацию
        bool __fastcall CollectInfo(string watchname, bool collectstate);

        // Восстанавливает серии по информации из файла 'watchname'
        // Если 'collectstate' == 'true', то восстанавливает также накопленную
        // информацию
        // (Все открытые наблюдения будут предварительно закрыты)
        bool __fastcall RestoreInfo(string watchname, bool collectstate);
        // ------------------------------

        // ------------------------------
        // Методы управления наблюдениями
		// ------------------------------
		// Возвращает данные наблюдения
		TUWatchInfo* __fastcall Get(int seriesindex);
		TUWatchInfo* __fastcall Get(void *datasource);

		// Возвращает общее число данных наблюдения
		int __fastcall GetNumWatches(void);

		// Добавление нового наблюдения
		// Возвращает индекс серии
		int __fastcall Add(TUWatchInfo& wd);

		// Добавление нового наблюдения по имени компонента и индексу выхода
		// Возвращает индекс серии
		int __fastcall Add(const string &name, int output=0, int outindex=0, double yshift=0, TPenStyle style=psSolid, TColor color=TColor(0));

        // Удаление наблюдения
        void __fastcall Del(int seriesindex);

        // Удаляет все наблюдения
		void __fastcall Clear(void);

		// Отключает отображение серии
		// используется при обновлении данных
		// если seriesindex < 0 то отключает все серии
		void __fastcall SeriesDisable(int seriesindex=-1);

		// Включает отображение серии
		// используется при обновлении данных
		// если seriesindex < 0 то включает все серии
		void __fastcall SeriesEnable(int seriesindex=-1);

		// Обновление данных серии
		// Добавление одной точки в серию
 //		void __fastcall SeriesUpdate(int seriesindex, double x, double y);
 //		void __fastcall SeriesUpdate(void* datasource, double x, double y);

		// Обновление данных серии
		// Добавление массива точек в серию
 //		void __fastcall SeriesUpdate(int seriesindex, double* x, double* y, int size);
 //		void __fastcall SeriesUpdate(void* datasource, double* x, double* y, int size);

		// Обновление информации за 'stepcount' прошедших шагов интегрирования
		void __fastcall StepUpdate(bool speedup=true);

        // Удаляет данные всех наблюдений
        void __fastcall Reset(void);

        // Возвращает копию списка всех наблюдаемых серий
		void __fastcall GetWatchList(map<int, TUWatchInfo> &buffer);

		// Возвращает копию списка всех видимых серий
		void __fastcall GetVisibleList(map<int, TUWatchInfo> &buffer);

		// Возвращает копию списка всех невидимых серий
		void __fastcall GetInvisibleList(map<int, TUWatchInfo> &buffer);

		// Заменяет подпись под выбранной серией
		void __fastcall ChangeLegend(int seriesindex, string legend);

        // Заменяет цвет выбранной серии
		void __fastcall ChangeColor(int seriesindex, TColor color);

		// Заменяет тип линии выбранной серии
		void __fastcall ChangeLineStyle(int seriesindex, TPenStyle style);

		// Изменяет информацио о видимости серии
		void __fastcall ChangeVisible(int seriesindex, bool visible);

		// Изменяет смещение по оси Y
		void __fastcall ChangeYShift(int seriesindex, double yshift);

		// Изменяет толщину линии
		void __fastcall ChangeLineWidth(int seriesindex, int width);
		// ------------------------------

		// -----------------------------
		// Методы визуального управления
		// -----------------------------
		// Изменение данных серий
		void __fastcall TBSeriesModify(TObject *Sender);

		// Сохранение графика в файл
		// Сохраняет изображение графика в bmp
		bool SaveToBitmap(const AnsiString &filename);

		// Сохраняет изображение в метафайл
		bool SaveToMetafile(const AnsiString &filename);

		// Сохраняет изображение графика в jpeg
		bool SaveToJpeg(const AnsiString &filename);

		// Сохраняет изображение с выбором типа из диалога
		bool Save(void);
		// -----------------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUWatchFrame *UWatchFrame;
//---------------------------------------------------------------------------
#endif
