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
#include <string>
#include <vector>
//---------------------------------------------------------------------------
class TUComponentsListFrame : public TFrame
{
__published:	// IDE-managed Components
	TPanel *Panel1;
	TRichEdit *ParametersRichEdit;
	THeaderControl *ParametersHeaderControl;
	TPanel *Panel2;
	TStringGrid *StringGrid;
	THeaderControl *HeaderControl;
	TSplitter *Splitter1;
	void __fastcall FrameResize(TObject *Sender);
	void __fastcall StringGridDblClick(TObject *Sender);
	void __fastcall StringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall StringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall StringGridClick(TObject *Sender);
	void __fastcall ParametersHeaderControlSectionClick(THeaderControl *HeaderControl,
          THeaderSection *Section);

protected:		// User declarations
		// Длинное имя выделенного компонента
		std::string SelectedComponentName;

		// Длинный строковой Id выделенного компонента
		std::string SelectedComponentId;

		// Длинное имя текущего компонента
		std::string CurrentComponentName;

		// Длинный строковой id текущего компонента
		std::string CurrentComponentId;

		// Путь до текущего компонента
		std::vector<std::string> CurrentPath;

		// true если запрещено изменение значений дерева реестра
		bool TreeReadOnlyFlag;

		// true если запрещено изменение значений параметров
		bool ParamReadOnlyFlag;

		// true если реестр был изменен
		bool RegistryModified;

		// Включение-выключение отображения параметров в виде xml
		bool ShowXMLComponentParameters;

protected: // Временные переменные
		bool UpdateInterfaceFlag;

		// Элемент который должен быть найден и выделен после обновления списка
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

		// Длинное имя текущего компонента
		const std::string& GetCurrentComponentName(void) const;

		// Длинный строковой id текущего компонента
		const std::string& GetCurrentComponentId(void) const;

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


};
//---------------------------------------------------------------------------
extern PACKAGE TUComponentsListFrame *UComponentsListFrame;
//---------------------------------------------------------------------------
#endif
