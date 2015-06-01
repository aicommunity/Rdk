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
        // ��������� �� ������
        RDK::UClassRegistry* Registry;

        // ���������� ������, �������� ��� ���������� � ������� ��������� ���������
        // SetRegistry
        RDK::UClassRegistry DiffRegistry;

        // ��������� �� ������� ������������ ������
        RDK::UClassRegistry* CurrentRoot;

        // ��������� �� ������� ���������� ����
        RDK::UClassRegistry* CurrentSubTree;

        // ��� �������� ������������ �����
        std::string CurrentRootName;

        // ��� �������� ������
        std::string SelectedClassName;

        // ������� ��� �������� ����������
        std::string CurrentComponentName;

        // ��� �������� ���������
        std::string CurrentParameterName;

        // ���� �� �������� ����������
        std::vector<std::string> CurrentPath;

        // true ���� ��������� ��������� �������� ������ �������
        bool TreeReadOnlyFlag;

        // true ���� ��������� ��������� �������� ����������
        bool ParamReadOnlyFlag;

        // true ���� ������ ��� �������
        bool RegistryModified;

protected: // ��������� ����������

        bool UpdateInterfaceFlag;

public:
        // -----------------------
        // ������ ������� � ���������� ������
        // -----------------------
        // ���������� ��������� �� ������� ������
        RDK::UClassRegistry* GetRegistry(void);

        // ���������� ��������� �� ��������� �������
        RDK::UClassRegistry* GetDiffRegistry(void);

        // ���������� ��������� �� ��������� ����
        RDK::UClassRegistry* GetCurrentSubTree(void);

        // ���������� ���� ������� �������������� ������ �����
        bool GetTreeReadOnlyFlag(void);

        // ���������� ���� ������� �������������� ����������
        bool GetParamReadOnlyFlag(void);

        // ���������� ������� ������� ��������� � ������
        bool GetRegistryModified(void);
        // -----------------------

        // -----------------------
        // ������ ���������� ����������� �������
        // -----------------------
        // ������������� ����� ������� ������
        void __fastcall SetRegistry(RDK::UClassRegistry *reg);

        // ������� ������ ��������� �������
        void __fastcall ClearDiffRegistry(void);

        // ������ ������� ���� � ������ ������ 'fullname'
        bool __fastcall SetCurrentSubTree(std::string &fullname);

        // ���������/��������� �������������� ������ �����
        void __fastcall SetTreeReadOnlyFlag(bool flag);

        // ���������/��������� �������������� ����������
        void __fastcall SetParamReadOnlyFlag(bool flag);

        // ������������� ������� ������� ��������� � ������
        void __fastcall SetRegistryModified(bool flag);
        // -----------------------

        // -----------------------
        // ������ ������� � ����������� ������
        // -----------------------
        // ���������� ��������� �� ������ ���������� ����������� ����
        // ��� 0, ���� �� �������� ������
        RDK::UClassRegData* __fastcall GetSelectedData(void);
        // -----------------------

        // -----------------------
        // ������ ����������
        // -----------------------
        // ��������� ���� ���������
        void __fastcall UpdateInterface(void);

        // �������������� ������ ���������
        void __fastcall UpdateComponentList(void);

        // �������������� ������ ����������
        void __fastcall UpdateParams(void);

        // �������������� ���� �� �������� ����
        void __fastcall UpdatePath(void);
        // -----------------------

        // -----------------------
        // ������� ������ ����������
        // -----------------------
protected:
        // ���������� ������ ��� ����
        String __fastcall GetFullName(TTreeNode *node);

        // �������������� ������ ���������
        void __fastcall UpdateComponentList(RDK::UClassRegistry &root);

        // �������������� ������ ����������
        void __fastcall UpdateParams(RDK::UClassRegData &data);
        // -----------------------
};
//---------------------------------------------------------------------------
extern PACKAGE TUClassRegistryFrame *UClassRegistryFrame;
//---------------------------------------------------------------------------
#endif
