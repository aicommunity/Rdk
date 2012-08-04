//---------------------------------------------------------------------------

#ifndef UComponentsListFrameUnitH
#define UComponentsListFrameUnitH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <ToolWin.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.ValEdit.hpp>
#include "TUVisualControllerFrameUnit.h"
#include <string>
#include <vector>
#include "TUVisualController.h"
//---------------------------------------------------------------------------
class TUComponentsListFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TRichEdit *ParametersRichEdit;
	THeaderControl *ParametersHeaderControl;
	TPanel *Panel2;
	TStringGrid *StringGrid;
	THeaderControl *HeaderControl;
	TSplitter *Splitter1;
	TPageControl *PageControl1;
	TTabSheet *TabSheet1;
	TTabSheet *TabSheet2;
	TTabSheet *TabSheet3;
	THeaderControl *StateHeaderControl;
	TRichEdit *StateRichEdit;
	THeaderControl *IOHeaderControl;
	TGroupBox *GroupBox;
	TGroupBox *GroupBox2;
	TSplitter *Splitter2;
	TStringGrid *OutputsStringGrid;
	TStringGrid *InputsStringGrid;
	TTabSheet *TabSheet4;
	TTabSheet *TabSheet5;
	THeaderControl *HeaderControl1;
	THeaderControl *HeaderControl2;
	TStringGrid *ParametersListStringGrid;
	TStringGrid *StatesListStringGrid;
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall StringGridDblClick(TObject *Sender);
	void __fastcall StringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall StringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall StringGridClick(TObject *Sender);
	void __fastcall ParametersHeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section);
	void __fastcall ParametersRichEditChange(TObject *Sender);
	void __fastcall StateRichEditChange(TObject *Sender);
	void __fastcall StateHeaderControlSectionClick(THeaderControl *HeaderControl, THeaderSection *Section);
	void __fastcall OutputsStringGridClick(TObject *Sender);
	void __fastcall OutputsStringGridDblClick(TObject *Sender);
	void __fastcall OutputsStringGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall InputsStringGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall InputsStringGridDblClick(TObject *Sender);
	void __fastcall InputsStringGridClick(TObject *Sender);
	void __fastcall PageControl1Change(TObject *Sender);
	void __fastcall ParametersListStringGridClick(TObject *Sender);
	void __fastcall ParametersListStringGridDblClick(TObject *Sender);
	void __fastcall ParametersListStringGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect);
	void __fastcall StatesListStringGridClick(TObject *Sender);
	void __fastcall StatesListStringGridDblClick(TObject *Sender);
	void __fastcall StatesListStringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);



protected:		// User declarations
		// Длинное имя выделенного компонента
		std::string SelectedComponentName;

		// Длинный строковой Id выделенного компонента
		std::string SelectedComponentId;

		// Длинное имя текущего компонента
		std::string CurrentComponentName;

		// Длинный строковой id текущего компонента
		std::string CurrentComponentId;

		// Имя выделенного параметра выделенного компонента
		std::string SelectedComponentParameterName;

		// Имя выделенной переменной состояния выделенного компонента
		std::string SelectedComponentStateName;

		// Путь до текущего компонента
		std::vector<std::string> CurrentPath;

		// Выбранный выход объекта
		int SelectedComponentOutput;

		// Выбранный вход объекта
		int SelectedComponentInput;

		// true если запрещено изменение значений дерева реестра
		bool TreeReadOnlyFlag;

		// true если запрещено изменение значений параметров
		bool ParamReadOnlyFlag;

		// true если реестр был изменен
		bool RegistryModified;

		// Включение-выключение отображения параметров в виде xml
		bool ShowXMLComponentParameters;

protected: // Временные переменные		// Элемент который должен быть найден и выделен после обновления списка
		int SelectedId;

public:
	__fastcall TUComponentsListFrame(TComponent* Owner);

		// -----------------------
        // Методы доступа к физическим данным
		// -----------------------
		// Возвращает id выбранного компонента
		const std::string GetSelectedComponentId(void) const;

		// Возвращает имя выбранного компонента
		const std::string GetSelectedComponentName(void) const;

		// Длинное имя выделенного компонента
		const std::string& GetSelectedComponentLongName(void) const;

		// Длинный строковой Id выделенного компонента
		const std::string& GetSelectedComponentLongId(void) const;

		// Имя выделенного параметра выделенного компонента
		const std::string& GetSelectedComponentParameterName(void) const;

		// Имя выделенной переменной состояния выделенного компонента
		const std::string& GetSelectedComponentStateName(void) const;

		// Длинное имя текущего компонента
		const std::string& GetCurrentComponentName(void) const;

		// Длинный строковой id текущего компонента
		const std::string& GetCurrentComponentId(void) const;

		// Выбранный выход объекта
		int GetSelectedComponentOutput(void) const;

		// Выбранный вход объекта
		int GetSelectedComponentInput(void) const;

		// Включение-выключение отображения параметров в виде xml
		bool GetShowXMLComponentParameters(void) const;

		// Возвращает флаг запрета редактирования дерева узлов
		bool GetTreeReadOnlyFlag(void);

		// Возвращает флаг запрета редактирования параметров
		bool GetParamReadOnlyFlag(void);

		// Возвращает признак наличия изменений в данных
		bool GetRegistryModified(void);
		// -----------------------

		// -----------------------
		// Методы управления физическими данными
		// -----------------------
		// Очищает список изменений реестра
		void __fastcall ClearDiffRegistry(void);

		// Делает текущим узел с полным именем 'fullname'
		bool __fastcall SetCurrentSubTree(std::string &fullname);

		// Включение-выключение отображения параметров в виде xml
		void SetShowXMLComponentParameters(bool flag);

		// Разрешает/запрещает редактирование дерева узлов
		void __fastcall SetTreeReadOnlyFlag(bool flag);

		// Разрешает/запрещает редактирование параметров
		void __fastcall SetParamReadOnlyFlag(bool flag);

		// Устанавливает признак наличия изменений в данных
		void __fastcall SetRegistryModified(bool flag);
        // -----------------------


// Перерисовывает путь до текущего узла
void __fastcall UpdatePath(void);

// Отрисовка фрейма
void UpdateInterface(void);

// Обновляет длинные имена выбранных компонент
void UpdateSelectedComponentInfo(void);

// Обновляет параметры компонента
void UpdateParameters(void);

// Обновляет состояние компонента
void UpdateState(void);

// Обновляет данные ввода-вывода
void UpdateIO(void);

// Обновляет данные списка параметров
void UpdateParametersList(void);

// Обновляет данные списка переменных состояния
void UpdateStatesList(void);
};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsListFrame *UComponentsListFrame;
//---------------------------------------------------------------------------
#endif
