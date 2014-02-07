//---------------------------------------------------------------------------

#ifndef TEnchancedStringGridH
#define TEnchancedStringGridH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Grids.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------
#include <vector>
//---------------------------------------------------------------------------
enum PropertyType {StrP, BoolP, IntP, DoubleP, BorderedIntP};
class TProperty
{
private:
	String m_PropertyName;
	PropertyType m_PType;
	String m_value;

	bool m_listed;
	std::vector<String> m_listItems;

	int m_lowBorder;
	int m_upBorder;
	int m_step;

	bool StringToBool(String s, bool &err);
	String BoolToString(bool b);

	int m_number;
	String  m_description;
public:
	TProperty();
	~TProperty();
	TProperty(const TProperty &p);
	operator =(const TProperty &p);

	TProperty(String PropertyName, PropertyType PType, String value);
	void SetName(String PropertyName);
	String GetName();
	void SetBool(bool value);
	bool GetBool();
	void SetString(String value);
	String GetString();
	void SetInt(int value);
	int GetInt();
	void SetDouble(double value);
	void GetDouble(double value);
	void SetValue(String value, PropertyType type);

	void SetBorders(int low, int up, int step);
	void GetBorders(int &low, int &up, int &step);

	void AddListItem(String text, int index);
	void SetListed(bool listed);
	bool GetListed();

	void GetListPointer(std::vector<String> **ret);

	void SetNumber(int n);
	int GetNumber();

	void SetDesc(String d);
	String GetDesc();

	PropertyType GetType();
};
//---------------------------------------------------------------------------
class TPropertyStorage
{
private:
	std::vector<TProperty> m_storage;
public:
	TPropertyStorage();
	void AddProperty(TProperty p);
	bool AddPropertyByIndex(int id, TProperty p);
	bool GetPropertyByIndex(int id, TProperty *p);
	bool SetPropertyByIndex(int id, TProperty p);
	bool GetPropertyByName(String name, TProperty &p);
	void Clear();
	int Size();
};
//---------------------------------------------------------------------------
class TEnchancedSG : public TFrame
{
__published:	// IDE-managed Components
	TStringGrid *BasicStringGrid;
	TEdit *txtStringEdit;
	TCheckBox *chbBoolEdit;
	TUpDown *udBorderedInt;
	TEdit *txtDoubleEdit;
	TEdit *txtIntEdit;
	TEdit *txtBorderedIntEdit;
	TComboBox *cmbListEdit;
	void __fastcall BasicStringGridDrawCell(TObject *Sender, int ACol, int ARow, TRect &Rect,
          TGridDrawState State);
	void __fastcall BasicStringGridSelectCell(TObject *Sender, int ACol, int ARow, bool &CanSelect);
	void __fastcall BasicStringGridKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall txtDoubleEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall txtIntEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall txtBorderedIntEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall cmbListEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall txtStringEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall chbBoolEditKeyPress(TObject *Sender, System::WideChar &Key);
	void __fastcall BasicStringGridDblClick(TObject *Sender);
	void __fastcall chbBoolEditClick(TObject *Sender);



private:	// User declarations
	bool m_editing;
	bool m_end_editing;
	int m_editingCol;
	int m_editingRow;
public:		// User declarations
	TPropertyStorage m_storage;
	bool m_cellChanged;
	bool m_bCellChanged;
	void Refresh();
	__fastcall TEnchancedSG(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TEnchancedSG *EnchancedSG;
//---------------------------------------------------------------------------
#endif
