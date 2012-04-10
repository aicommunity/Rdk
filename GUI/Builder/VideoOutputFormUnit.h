//---------------------------------------------------------------------------

#ifndef VideoOutputFormUnitH
#define VideoOutputFormUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "VideoOutputFrameUnit.h"
#include <Menus.hpp>
#include <IniFiles.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
class TVideoOutputForm : public TForm
{
__published:	// IDE-managed Components
	TPageControl *PageControl;
	void __fastcall PageControlChange(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TVideoOutputForm(TComponent* Owner);

// Источники видео
std::vector<TVideoOutputFrame*> Sources;

bool UpdateInterfaceFlag;

// Обновляет интерфейс
void UpdateInterface(void);

// Число источников видео
int GetNumSources(void) const;

// Добавляет новый источник видео
void AddSource(void);

// Удаляет источник видео
void DelSource(int index);

// Удаляет все источники видео
void ClearSources(void);

// Выбирает режим заданного источника
void SetSourceType(int index, int mode);

// Возвращает фрейм источника видео
TVideoOutputFrame* GetVideoOutputFrame(int index);

// Сохраняет информацию об источниках данных в заданный ini файл
void SaveToIni(TMemIniFile *ini, const String &section);

// Загружает информацию об источниках данных из заданного ini файла
void LoadFromIni(TMemIniFile *ini, const String &section);
};
//---------------------------------------------------------------------------
extern PACKAGE TVideoOutputForm *VideoOutputForm;
//---------------------------------------------------------------------------
#endif
