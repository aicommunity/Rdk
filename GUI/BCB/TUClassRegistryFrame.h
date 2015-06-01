//---------------------------------------------------------------------------

#ifndef TUClassRegistryFrameH
#define TUClassRegistryFrameH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <Grids.hpp>
#include <ValEdit.hpp>
#include <ToolWin.hpp>
#include "../../Deploy/Include/rdk_cpp_initdll.h"
//---------------------------------------------------------------------------
class TUClassRegistryFrame : public TFrame
{
__published:    // IDE-managed Components
    TPanel *Panel;
    TLabeledEdit *LabeledEdit;
    TBitBtn *SetBitBtn;
    TBitBtn *RestoreBitBtn;
    TListView *ListView;
    TSplitter *Splitter;
    THeaderControl *HeaderControl;
    TListView *ComponentsListView;
    void __fastcall RestoreBitBtnClick(TObject *Sender);
    void __fastcall SetBitBtnClick(TObject *Sender);
    void __fastcall LabeledEditKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall ListViewSelectItem(TObject *Sender, TListItem *Item, bool Selected);
    void __fastcall ComponentsListViewResize(TObject *Sender);
    void __fastcall ComponentsListViewDblClick(TObject *Sender);
    void __fastcall ComponentsListViewColumnClick(TObject *Sender, TListColumn *Column);
    void __fastcall ComponentsListViewSelectItem(TObject *Sender, TListItem *Item, bool Selected);
    void __fastcall ComponentsListViewKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall HeaderControlSectionClick(THeaderControl *HeaderControl, THeaderSection *Section);

private:    // User declarations
public:        // User declarations
    __fastcall TUClassRegistryFrame(TComponent* Owner);
protected:    // User declarations
        // Указатель на реестр
        RDK::UClassRegistry* Registry;

        // Внутренний реестр, хранящий все обновления с момента последней установки
        // SetRegistry
        RDK::UClassRegistry DiffRegistry;

        // Указатель на текущий отображаемый корень
        RDK::UClassRegistry* CurrentRoot;

        // Указатель на текущий выделенный узел
        RDK::UClassRegistry* CurrentSubTree;

        // Имя текущего отображемого корня
        std::string CurrentRootName;

        // Имя текущего класса
        std::string SelectedClassName;

        // Длинное имя текущего компонента
        std::string CurrentComponentName;

        // Имя текущего параметра
        std::string CurrentParameterName;

        // Путь до текущего компонента
        std::vector<std::string> CurrentPath;

        // true если запрещено изменение значений дерева реестра
        bool TreeReadOnlyFlag;

        // true если запрещено изменение значений параметров
        bool ParamReadOnlyFlag;

        // true если реестр был изменен
        bool RegistryModified;

protected: // Временные переменные

        bool UpdateInterfaceFlag;

public:
        // -----------------------
        // Методы доступа к физическим данным
        // -----------------------
        // Возвращает указатель на рабочий реестр
        RDK::UClassRegistry* GetRegistry(void);

        // Возвращает указатель на изменения реестра
        RDK::UClassRegistry* GetDiffRegistry(void);

        // Возвращает указатель на выбранный узел
        RDK::UClassRegistry* GetCurrentSubTree(void);

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
        // Устанавливает новый рабочий реестр
        void __fastcall SetRegistry(RDK::UClassRegistry *reg);

        // Очищает список изменений реестра
        void __fastcall ClearDiffRegistry(void);

        // Делает текущим узел с полным именем 'fullname'
        bool __fastcall SetCurrentSubTree(std::string &fullname);

        // Разрешает/запрещает редактирование дерева узлов
        void __fastcall SetTreeReadOnlyFlag(bool flag);

        // Разрешает/запрещает редактирование параметров
        void __fastcall SetParamReadOnlyFlag(bool flag);

        // Устанавливает признак наличия изменений в данных
        void __fastcall SetRegistryModified(bool flag);
        // -----------------------

        // -----------------------
        // Методы доступа к виртуальным данным
        // -----------------------
        // Возвращает указатель на список параметров выделенного узла
        // или 0, если не выделено ничего
        RDK::UClassRegData* __fastcall GetSelectedData(void);
        // -----------------------

        // -----------------------
        // Методы управления
        // -----------------------
        // Обновляет весь интерфейс
        void __fastcall UpdateInterface(void);

        // Перерисовывает список компонент
        void __fastcall UpdateComponentList(void);

        // Перерисовывает список параметров
        void __fastcall UpdateParams(void);

        // Перерисовывает путь до текущего узла
        void __fastcall UpdatePath(void);
        // -----------------------

        // -----------------------
        // Скрытые методы управления
        // -----------------------
protected:
        // Возвращает полное имя узла
        String __fastcall GetFullName(TTreeNode *node);

        // Перерисовывает список компонент
        void __fastcall UpdateComponentList(RDK::UClassRegistry &root);

        // Перерисовывает список параметров
        void __fastcall UpdateParams(RDK::UClassRegData &data);
        // -----------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassRegistryFrame *UClassRegistryFrame;
//---------------------------------------------------------------------------
#endif
