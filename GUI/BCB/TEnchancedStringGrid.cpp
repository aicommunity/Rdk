// ---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#define EDIT_COL 2

#include "TEnchancedStringGrid.h"
// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#include "windows.h"
TEnchancedSG *EnchancedSG;

// ---------------------------------------------------------------------------
__fastcall TEnchancedSG::TEnchancedSG(TComponent* Owner) : TFrame(Owner) {
	/*BasicStringGrid->RowCount = 2;
	BasicStringGrid->ColCount = 2;
	BasicStringGrid->Cells[0][0] = "Property";
	BasicStringGrid->Cells[1][0] = "Value";*/
	BasicStringGrid->Align = alClient;
	m_editing = false;
	m_bCellChanged = false;
	m_FlagChanged = false;
}

void TEnchancedSG::Refresh()
{
	/*this->BasicStringGrid->RowCount = m_storage.Size()+1; */
	BasicStringGrid->Refresh();
	ResizeFrame();
	//this->BasicStringGrid->Repaint();
	//this->BasicStringGrid->Update();
	m_editing = false;
}

// ---------------------------------------------------------------------------
// TProperty Members
// ---------------------------------------------------------------------------
TProperty::TProperty() {

}

TProperty& TProperty::operator =(const TProperty &p)
{
	this->m_description = p.m_description;
	this->m_listed = p.m_listed;
	this->m_listItems.clear();
	this->m_listItems.insert(m_listItems.begin(), p.m_listItems.begin(), p.m_listItems.end());
	this->m_lowBorder = p.m_lowBorder;
	this->m_number = p.m_number;
	this->m_PropertyName = p.m_PropertyName;
	this->m_PType = p.m_PType;
	this->m_step = p.m_step;
	this->m_upBorder = p.m_upBorder;
	this->m_value = p.m_value;
	return *this;
}

TProperty::~TProperty() {
	this->m_listItems.clear();
}

TProperty::TProperty(const TProperty &p)
{
	this->m_description = p.m_description;
	this->m_listed = p.m_listed;
	this->m_listItems.clear();
	this->m_listItems.insert(m_listItems.begin(), p.m_listItems.begin(), p.m_listItems.end());
	this->m_lowBorder = p.m_lowBorder;
	this->m_number = p.m_number;
	this->m_PropertyName = p.m_PropertyName;
	this->m_PType = p.m_PType;
	this->m_step = p.m_step;
	this->m_upBorder = p.m_upBorder;
	this->m_value = p.m_value;
}

// ---------------------------------------------------------------------------
TProperty::TProperty(String PropertyName, PropertyType PType, String value) {
	this->m_PropertyName = PropertyName;
	this->m_PType = PType;
	this->m_value = value;
	this->m_listed = false;
}

// ---------------------------------------------------------------------------
void TProperty::SetName(String PropertyName) {
	this->m_PropertyName = PropertyName;
}

// ---------------------------------------------------------------------------
String TProperty::GetName() {
	return this->m_PropertyName;
}

// ---------------------------------------------------------------------------
void TProperty::SetBool(bool value) {

	this->m_PType = BoolP;
	this->m_value = BoolToString(value);
}
//---------------------------------------------------------------------------
bool TProperty::GetBool() {
	bool err;
	bool ret = StringToBool(m_value, err);
	return (ret); // что с ошибками творить?!
}
//---------------------------------------------------------------------------
void TProperty::SetString(String value) {
	m_value = value;
}
//---------------------------------------------------------------------------
String TProperty::GetString() {
	return m_value;
}
//---------------------------------------------------------------------------
void TProperty::SetInt(int value) {
	m_value = IntToStr(value);
}
//---------------------------------------------------------------------------
int TProperty::GetInt() {
	return StrToInt(m_value);
}
//---------------------------------------------------------------------------
void TProperty::SetDouble(double value) {
	m_value = FloatToStr(value);
}
//---------------------------------------------------------------------------
void TProperty::SetValue(String value, PropertyType type) {
	this->m_value = value;
	this->m_PType = type;
}
//---------------------------------------------------------------------------
bool TProperty::StringToBool(String s, bool &err) {
	if ((s == "1") || (s == "True") || (s == "true") || (s == "T") ||
		(s == "t")) {
		return true;
	}
	else if ((s == "0") || (s == "False") || (s == "false") || (s == "F") ||
		(s == "f")) {
		return false;
	}
	err = true;
	return false;
}
//---------------------------------------------------------------------------
String TProperty::BoolToString(bool b) {
	if (b) {
		return "1";
	}
	else {
		return "0";
	}
}
//---------------------------------------------------------------------------
PropertyType TProperty::GetType()
{
	return m_PType;
}
//---------------------------------------------------------------------------
void TProperty::SetBorders(int low, int up, int step)
{
	m_lowBorder = low;
	m_upBorder = up;
	m_step = step;
}
//---------------------------------------------------------------------------
void TProperty::GetBorders(int &low, int &up, int &step)
{
	low = m_lowBorder;
	up = m_upBorder;
	step = m_step;

}
//---------------------------------------------------------------------------
void TProperty::AddListItem(String text, int index)
{
	if(m_listed)
	{
		if((index==-1)||(index>= int(m_listItems.size())))
		{
			m_listItems.push_back(text);
		}
		else
		{
			m_listItems.insert(m_listItems.begin()+index, text);
		}
	}
}
//---------------------------------------------------------------------------
void TProperty::SetListed(bool listed)
{
	this->m_listed = listed;
	if(!m_listed)
	{
		m_listItems.clear();
    }
}
//---------------------------------------------------------------------------
bool TProperty::GetListed()
{
	return m_listed;
}
//---------------------------------------------------------------------------
void TProperty::GetListPointer(std::vector<String> **ret)
{
	*ret = &m_listItems;
}
//---------------------------------------------------------------------------
void TProperty::SetNumber(int n)
{
	m_number = n;
}
//---------------------------------------------------------------------------
int TProperty::GetNumber()
{
	return m_number;
}
//---------------------------------------------------------------------------
void TProperty::SetDesc(String d)
{
	m_description = d;
}
//---------------------------------------------------------------------------
String TProperty::GetDesc()
{
	return m_description;
}
//---------------------------------------------------------------------------
//PropertyStorage Members
//---------------------------------------------------------------------------
TPropertyStorage::TPropertyStorage()
{

}
//---------------------------------------------------------------------------
void TPropertyStorage::AddProperty(TProperty p)
{
   this->m_storage.push_back(p);

}
//---------------------------------------------------------------------------
bool TPropertyStorage::AddPropertyByIndex(int id, TProperty p)
{
   if(id<this->Size())
   {
	   m_storage.insert(m_storage.begin()+id, p); //Возможно ошибка
	   return true;
   }
   else
   {
	   return false;
   }
}
//---------------------------------------------------------------------------
bool TPropertyStorage::GetPropertyByIndex(int id, TProperty *p)
{
	if(id<this->Size())
   {
		//TProperty *p1;
		//p1 =
		*p = TProperty(m_storage.at(id));
		   //Возможно ошибка
	   return true;
   }
   else
   {
	   return false;
   }
}
//---------------------------------------------------------------------------
bool TPropertyStorage::SetPropertyByIndex(int id, TProperty p)
{
	if(id<this->Size())
   {
	   m_storage.erase(m_storage.begin()+id);
	   m_storage.insert(m_storage.begin()+id, p);
		   //Возможно ошибка
	   return true;
   }
   else
   {
	   return false;
   }
}
//---------------------------------------------------------------------------
bool TPropertyStorage::GetPropertyByName(String name, TProperty &p)
{
	for(int i=0; i<this->Size();i++)
	{
	   if(name==m_storage.at(i).GetName())
	   {
		  p = m_storage.at(i);
		  return true;
	   }
	}
	return false;
}
//---------------------------------------------------------------------------
int TPropertyStorage::Size()
{
	return m_storage.size();
}
//---------------------------------------------------------------------------
void TPropertyStorage::Clear()
{
    m_storage.clear();
}
//---------------------------------------------------------------------------
void __fastcall TEnchancedSG::BasicStringGridDrawCell(TObject *Sender, int ACol, int ARow,
		  TRect &Rect, TGridDrawState State)
{

	TRect DrawRect=Rect;
	m_cellChanged=false;

	if(this->m_storage.Size()<=0)
		return;
	if(ARow>(this->m_storage.Size()))
		return;

	if(ARow>0)
	{
		TProperty p;
		this->m_storage.GetPropertyByIndex(ARow-1, &p);

	   if(ACol==0)
	   {
			if(BasicStringGrid->Cells[ACol][ARow] != IntToStr(p.GetNumber()))
				BasicStringGrid->Cells[ACol][ARow] = IntToStr(p.GetNumber());
	   }
	   else if(ACol==1)
	   {
			if(BasicStringGrid->Cells[ACol][ARow] != p.GetName())
				BasicStringGrid->Cells[ACol][ARow] = p.GetName();
	   }
	   else if(ACol==2)
	   {
			if(p.GetListed())
			{
				if(State.Contains(gdFocused)&&m_editing&&(ACol==m_editingCol)&&(ARow==m_editingRow))
				{
					cmbListEdit->Left = BasicStringGrid->Left+Rect.Left;
					cmbListEdit->Top = BasicStringGrid->Top+Rect.Top+4;
					cmbListEdit->Width = Rect.Right - Rect.Left;
					cmbListEdit->Height = Rect.Bottom - Rect.Top-2;
					cmbListEdit->Text = p.GetString();
					std::vector<String> * vec;
					p.GetListPointer(&vec);
					cmbListEdit->Items->Clear();
					for(size_t i = 0; i<vec->size();i++)
					{
						cmbListEdit->Items->Add(vec->at(i));
					}
					cmbListEdit->Visible = True;
					cmbListEdit->SetFocus();
					/*m_editingCol = ACol;
					m_editingRow = ARow;*/
					//m_editing = true;
				}
				else
				{
					if(BasicStringGrid->Cells[ACol][ARow] != p.GetString())
					{
						if(BasicStringGrid->Cells[ACol][ARow]!="")
							m_cellChanged=true;
						BasicStringGrid->Cells[ACol][ARow] = p.GetString();
					}
				}
			}
			else
			{
			   switch(p.GetType())
			   {
					case StrP:
						if(State.Contains(gdFocused)&&m_editing&&(ACol==m_editingCol)&&(ARow==m_editingRow))
						{
							txtStringEdit->Left = BasicStringGrid->Left+Rect.Left;
							txtStringEdit->Top = BasicStringGrid->Top+Rect.Top+4;
							txtStringEdit->Width = Rect.Right - Rect.Left;
							txtStringEdit->Height = Rect.Bottom - Rect.Top-2;
							txtStringEdit->Text = p.GetString();
							txtStringEdit->Visible = True;
							txtStringEdit->SetFocus();
							/*m_editingCol = ACol;
							m_editingRow = ARow;*/
							//m_editing = true;
						}
						else
						{
							if(BasicStringGrid->Cells[ACol][ARow] != p.GetString())
							{
								if(BasicStringGrid->Cells[ACol][ARow]!="")
									m_cellChanged=true;
								BasicStringGrid->Cells[ACol][ARow] = p.GetString();
							}
						}
					break;
					case BoolP:
					   /*	if(State.Contains(gdFocused)&&m_editing&&(ACol==m_editingCol)&&(ARow==m_editingRow))
						{
							chbBoolEdit->Left = BasicStringGrid->Left+Rect.Left;
							chbBoolEdit->Top = BasicStringGrid->Top+Rect.Top+4;
							chbBoolEdit->Width = Rect.Right - Rect.Left;
							chbBoolEdit->Height = Rect.Bottom - Rect.Top-2;
							chbBoolEdit->Visible = True;
							chbBoolEdit->Checked = p.GetBool();

							if(chbBoolEdit->Checked)
								chbBoolEdit->Caption = "True";
							else
								chbBoolEdit->Caption = "False";
							chbBoolEdit->SetFocus();
							/*m_editingCol = ACol;
							m_editingRow = ARow;*/
							//m_editing = true;
						/*}
						else
						{*/
						InflateRect(DrawRect,-1,-1);
                        int iDrawState;
						if(p.GetBool())
						{
							iDrawState = DFCS_BUTTONCHECK|DFCS_CHECKED;
						}
						else
						{
						//
							iDrawState = DFCS_BUTTONCHECK;
							//DrawFrameControl(StringGrid1->, DrawRect,
							//   DFC_BUTTON, DrawState);
						}
						DrawFrameControl(BasicStringGrid->Canvas->Handle, &DrawRect, DFC_BUTTON, iDrawState);
						/*if(BasicStringGrid->Cells[ACol][ARow] != Properties[ARow-1].svalue)
							BasicStringGrid->Cells[ACol][ARow] = Properties[ARow-1].svalue;*/
						//}
					break;
					case IntP:
						if(State.Contains(gdFocused)&&m_editing&&(ACol==m_editingCol)&&(ARow==m_editingRow))
						{
							txtIntEdit->Left = BasicStringGrid->Left+Rect.Left;
							txtIntEdit->Top = BasicStringGrid->Top+Rect.Top+4;
							txtIntEdit->Width = Rect.Right - Rect.Left;
							txtIntEdit->Height = Rect.Bottom - Rect.Top-2;
							txtIntEdit->Text = p.GetString();
							txtIntEdit->Visible = True;
							txtIntEdit->SetFocus();
							/*m_editingCol = ACol;
							m_editingRow = ARow;*/
							//m_editing = true;
						}
						else
						{
							if(BasicStringGrid->Cells[ACol][ARow] != p.GetString())
							{
								if(BasicStringGrid->Cells[ACol][ARow]!="")
									m_cellChanged=true;
								BasicStringGrid->Cells[ACol][ARow] = p.GetString();
							}
						}
					break;
					case DoubleP:
						if(State.Contains(gdFocused)&&m_editing&&(ACol==m_editingCol)&&(ARow==m_editingRow))
						{
							txtDoubleEdit->Left = BasicStringGrid->Left+Rect.Left;
							txtDoubleEdit->Top = BasicStringGrid->Top+Rect.Top+4;
							txtDoubleEdit->Width = Rect.Right - Rect.Left;
							txtDoubleEdit->Height = Rect.Bottom - Rect.Top-2;
							txtDoubleEdit->Text = p.GetString();
							txtDoubleEdit->Visible = True;
							txtDoubleEdit->SetFocus();
							/*m_editingCol = ACol;
							m_editingRow = ARow;*/
							//m_editing = true;
						}
						else
						{
							if(BasicStringGrid->Cells[ACol][ARow] != p.GetString())
							{
								if(BasicStringGrid->Cells[ACol][ARow]!="")
									m_cellChanged=true;

								BasicStringGrid->Cells[ACol][ARow] = p.GetString();
							}

						}
					break;
					case BorderedIntP:
						if(State.Contains(gdFocused)&&m_editing&&(ACol==m_editingCol)&&(ARow==m_editingRow))
						{
							txtBorderedIntEdit->Left = BasicStringGrid->Left+Rect.Left;
							txtBorderedIntEdit->Top = BasicStringGrid->Top+Rect.Top+4;
							txtBorderedIntEdit->Width = Rect.Right - Rect.Left - (udBorderedInt->Width + 2);
							txtBorderedIntEdit->Height = Rect.Bottom - Rect.Top-2;
							txtBorderedIntEdit->Text = p.GetString();
							txtBorderedIntEdit->Visible = True;
							udBorderedInt->Associate = txtBorderedIntEdit;
							udBorderedInt->Height = Rect.Bottom - Rect.Top-2;
							udBorderedInt->Left = BasicStringGrid->Left + Rect.Right - udBorderedInt->Width - 1;
							udBorderedInt->Top = BasicStringGrid->Top+Rect.Top+4;
							int u,d,s;
							p.GetBorders(d,u,s);
							udBorderedInt->Max = u;
							udBorderedInt->Min = d;
							udBorderedInt->Increment = s;
							udBorderedInt->Visible = true;
							txtBorderedIntEdit->SetFocus();
							/*m_editingCol = ACol;
							m_editingRow = ARow;*/
							//m_editing = true;
						}
						else
						{
							if(BasicStringGrid->Cells[ACol][ARow] != p.GetString())
							{
								if(BasicStringGrid->Cells[ACol][ARow]!="")
									m_cellChanged=true;

								BasicStringGrid->Cells[ACol][ARow] = p.GetString();
                            }

						}
					break;
			   }
		   }
	   }
	   else if(ACol==3)
	   {
			if(BasicStringGrid->Cells[ACol][ARow] != p.GetDesc())
				BasicStringGrid->Cells[ACol][ARow] = p.GetDesc();
	   }
	}
	if((m_end_editing)&&(ACol==BasicStringGrid->ColCount-1)&&(ARow==BasicStringGrid->RowCount-1))
	 m_end_editing=false;
}
//---------------------------------------------------------------------------

void __fastcall TEnchancedSG::BasicStringGridSelectCell(TObject *Sender, int ACol,
		  int ARow, bool &CanSelect)
{
	if(!m_editing)
		return;
	
	if((m_editingCol != ACol)||(m_editingRow != ARow))
	{
		TProperty p;
		m_storage.GetPropertyByIndex(m_editingRow-1, &p);
		if(p.GetListed())
		{
			if(cmbListEdit->Items->IndexOf(cmbListEdit->Text)<0)
			{
				ShowMessage("Error, choose one of list values!");
				return;
			}
			else
			{
				p.SetValue(cmbListEdit->Text, p.GetType());
				cmbListEdit->Visible = false;
				m_editing = false;
				m_end_editing = true;
			}
		}
		else
		{
			switch(p.GetType())
			{
				case StrP:
					p.SetValue(txtStringEdit->Text, p.GetType());
					txtStringEdit->Visible = false;
					//txtStringEdit->Repaint();
					m_editing = false;
					m_end_editing = true;
				break;
				case BoolP:
					p.SetBool(chbBoolEdit->Checked);
					chbBoolEdit->Visible = false;
					m_editing = false;
					m_end_editing = true;
				break;
				case DoubleP:
					p.SetValue(txtDoubleEdit->Text, p.GetType());
					txtDoubleEdit->Visible = false;
					//txtStringEdit->Repaint();
					m_editing = false;
					m_end_editing = true;
				break;
				case IntP:
					p.SetValue(txtIntEdit->Text, p.GetType());
					txtIntEdit->Visible = false;
					//txtStringEdit->Repaint();
					m_editing = false;
					m_end_editing = true;
				break;
				case BorderedIntP:
					int val;
					val = StrToInt(txtBorderedIntEdit->Text);
					if(val>= udBorderedInt->Max)
						 val = udBorderedInt->Max;
					if(val<= udBorderedInt->Min)
						 val = udBorderedInt->Min;
					txtBorderedIntEdit->Text = IntToStr(val);

					p.SetValue(txtBorderedIntEdit->Text, p.GetType());
					txtBorderedIntEdit->Visible = false;
					udBorderedInt->Visible = false;
					//txtStringEdit->Repaint();
				   	m_editing = false;
					/*m_end_editing = true; */
				break;
			}

		}
		cmbListEdit->Hint=p.GetDesc();
		BasicStringGrid->SetFocus();
		m_storage.SetPropertyByIndex(m_editingRow-1, p);
		Refresh();
	}
}
//---------------------------------------------------------------------------

void __fastcall TEnchancedSG::BasicStringGridKeyPress(TObject *Sender, System::WideChar &Key)

{
	m_bCellChanged = false;
	if(Key==VK_TAB)
		return;
	if(Key==VK_SPACE)
	{
		if((BasicStringGrid->Col==(EDIT_COL))&&(BasicStringGrid->Row>0))
		{
			TProperty p;
			m_storage.GetPropertyByIndex(BasicStringGrid->Row-1, &p);
			if(p.GetType()==BoolP)
			{
				p.SetBool(!p.GetBool());
				m_storage.SetPropertyByIndex(BasicStringGrid->Row-1, p);
				Refresh();
				m_bCellChanged = true;
				return;
			}
		}
    }

	if(Key==VK_RETURN)
	{
		 if(BasicStringGrid->Col==(EDIT_COL))
		 {
			m_editing=true;
			m_editingCol = BasicStringGrid->Col;
			m_editingRow = BasicStringGrid->Row;
			Refresh();
		 }
    }
  /*if (m_editing)
  {
	chbBoolEdit->SetFocus();
	SendMessage(chbBoolEdit->Handle, WM_CHAR, (WORD)Key, 0);
  }*/
  Key=0;
}
//---------------------------------------------------------------------------


void __fastcall TEnchancedSG::txtDoubleEditKeyPress(TObject *Sender, System::WideChar &Key)

{
	m_bCellChanged = false;
	if(Key==VK_RETURN)
	{
		TProperty p;
		m_storage.GetPropertyByIndex(m_editingRow-1, &p);
		p.SetValue(txtDoubleEdit->Text, p.GetType());
		txtDoubleEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		m_storage.SetPropertyByIndex(m_editingRow-1, p);
		Refresh();
		m_bCellChanged = true;
		return;
	}
	if(Key==VK_ESCAPE)
	{
		txtDoubleEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		Refresh();
	}
	if(Key==VK_BACK)
	{
		return;
	}
	else if(Key=='-')
	{
		if(txtDoubleEdit->Text!="")
		{
			if(txtDoubleEdit->SelStart!=0)
			{
				Key=0;
				return;
			}
		}
	}
	else if(Key=='.')
	{
		if((txtDoubleEdit->Text=="")||(txtDoubleEdit->Text=="-")||(txtDoubleEdit->Text.Pos(".")!=0))
		{
			Key=0;
			return;
		}
	}
	else if((Key<'0')||(Key>'9'))
	{
		Key=0;
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TEnchancedSG::txtIntEditKeyPress(TObject *Sender, System::WideChar &Key)

{
	m_bCellChanged = false;
	if(Key==VK_RETURN)
	{
		TProperty p;
		m_storage.GetPropertyByIndex(m_editingRow-1, &p);
		p.SetValue(txtIntEdit->Text, p.GetType());
		txtIntEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		m_storage.SetPropertyByIndex(m_editingRow-1, p);
		Refresh();
		m_bCellChanged = true;
		return;
	}
	if(Key==VK_ESCAPE)
	{
		txtIntEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		Refresh();
	}
	if(Key==VK_BACK)
	{
		return;
	}
	else if(Key=='-')
	{
		if(txtIntEdit->Text!="")
		{
			if(txtIntEdit->SelStart!=0)
			{
				Key=0;
				return;
			}
		}
	}
	else if((Key<'0')||(Key>'9'))
	{
		Key=0;
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TEnchancedSG::txtBorderedIntEditKeyPress(TObject *Sender, System::WideChar &Key)

{
	m_bCellChanged = false;
   if(Key==VK_RETURN)
	{
		TProperty p;
		m_storage.GetPropertyByIndex(m_editingRow-1, &p);
		p.SetValue(txtBorderedIntEdit->Text, p.GetType());
		txtBorderedIntEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		m_storage.SetPropertyByIndex(m_editingRow-1, p);
		Refresh();
		m_bCellChanged = true;
		return;
	}
	if(Key==VK_ESCAPE)
	{
		txtBorderedIntEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		Refresh();
	}
   if(Key==VK_BACK)
	{
		return;
	}
	else if(Key=='-')
	{
		if(txtBorderedIntEdit->Text!="")
		{
			if(txtBorderedIntEdit->SelStart!=0)
			{
				Key=0;
				return;
			}
		}
	}
	else if((Key<'0')||(Key>'9'))
	{
		Key=0;
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TEnchancedSG::cmbListEditKeyPress(TObject *Sender, System::WideChar &Key)

{
	m_bCellChanged = false;
	if(Key==VK_ESCAPE)
	{
		cmbListEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		Refresh();
	}
	if(Key==VK_RETURN)
	{
		TProperty p;
		m_storage.GetPropertyByIndex(m_editingRow-1, &p);
		if(cmbListEdit->Items->IndexOf(cmbListEdit->Text)<0)
		{
			ShowMessage("Error, choose one of list values!");
		}
		else
		{
			p.SetValue(cmbListEdit->Text, p.GetType());
			cmbListEdit->Visible = false;
			m_editing = false;
			m_end_editing = true;
		}
		BasicStringGrid->SetFocus();
		m_storage.SetPropertyByIndex(m_editingRow-1, p);
		Refresh();
		m_bCellChanged = true;
	}
	Key=0;
}
//---------------------------------------------------------------------------


void __fastcall TEnchancedSG::txtStringEditKeyPress(TObject *Sender, System::WideChar &Key)
{
	m_bCellChanged = false;
	if(Key==VK_RETURN)
	{
		TProperty p;
		m_storage.GetPropertyByIndex(m_editingRow-1, &p);
		p.SetValue(txtStringEdit->Text, p.GetType());
		txtStringEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		m_storage.SetPropertyByIndex(m_editingRow-1, p);
		Refresh();
		m_bCellChanged = true;
	}
	if(Key==VK_ESCAPE)
	{
		txtStringEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		Refresh();
	}
}
//---------------------------------------------------------------------------

void __fastcall TEnchancedSG::chbBoolEditKeyPress(TObject *Sender, System::WideChar &Key)

{
	if(Key==VK_SPACE)
		return;
	if(Key==VK_RETURN)
	{
		TProperty p;
		m_storage.GetPropertyByIndex(m_editingRow-1, &p);
		p.SetBool(chbBoolEdit->Checked);
		chbBoolEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		m_storage.SetPropertyByIndex(m_editingRow-1, p);
		Refresh();
		return;
	}
	if(Key==VK_ESCAPE)
	{
		chbBoolEdit->Visible = false;
		m_editing = false;
		m_end_editing = true;
		BasicStringGrid->SetFocus();
		Refresh();
		return;
	}
	Key=0;
}
//---------------------------------------------------------------------------

void __fastcall TEnchancedSG::BasicStringGridDblClick(TObject *Sender)
{
	m_bCellChanged = false;
	if((BasicStringGrid->Col==2)&&(!m_editing))
	{
		if(BasicStringGrid->Row>0)
		{
			TProperty p;
			m_storage.GetPropertyByIndex(BasicStringGrid->Row-1, &p);
			if(p.GetType()==BoolP)
			{
				p.SetBool(!p.GetBool());
				m_storage.SetPropertyByIndex(BasicStringGrid->Row-1, p);
				Refresh();
				m_bCellChanged = true;
				m_FlagChanged = true;
				return;
			}
			m_editingCol = BasicStringGrid->Col;
			m_editingRow = BasicStringGrid->Row;

			m_editing=true;
		}
    }
}
//---------------------------------------------------------------------------

void __fastcall TEnchancedSG::chbBoolEditClick(TObject *Sender)
{
	if(chbBoolEdit->Checked)
		chbBoolEdit->Caption = "True";
	else
		chbBoolEdit->Caption = "False";

	if(!m_editing)
		return;
}
//---------------------------------------------------------------------------
void TEnchancedSG::ResizeFrame()
{
	if(BasicStringGrid->ColCount<3)
		return;

	BasicStringGrid->ColWidths[0] = BasicStringGrid->Canvas->TextWidth(BasicStringGrid->Cells[0][0])+10;
	int NumWidth = BasicStringGrid->Canvas->TextWidth(BasicStringGrid->Cells[0][0])+10;
	int DataMaxWidth = BasicStringGrid->Width/2;
	int NameMaxWidth = DataMaxWidth-BasicStringGrid->ColWidths[0];

	int NameNeededWidth = 0;


  for(int i=1; i<BasicStringGrid->RowCount;i++)
  {
	if((BasicStringGrid->Canvas->TextWidth(BasicStringGrid->Cells[1][i])+10)>NameNeededWidth)
	{
		NameNeededWidth = BasicStringGrid->Canvas->TextWidth(BasicStringGrid->Cells[1][i])+10;
	}
 }
 int NameWidth = 0;
 if(NameNeededWidth>NameMaxWidth)
	NameWidth = NameMaxWidth;
 else
	NameWidth = NameNeededWidth;

 int DataWidth = BasicStringGrid->Width - NameWidth - NumWidth - 28;

 BasicStringGrid->ColWidths[1] = NameWidth;
 BasicStringGrid->ColWidths[2] = DataWidth;
}

void __fastcall TEnchancedSG::FrameResize(TObject *Sender)
{
	ResizeFrame();
}
//---------------------------------------------------------------------------

void __fastcall TEnchancedSG::BasicStringGridMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y)
{
 int C,R;
 BasicStringGrid->MouseToCell(X, Y, C, R);

 if(C >= BasicStringGrid->ColCount || R >= BasicStringGrid->RowCount || C <0 || R<1)
  return;

 if ((BasicStringGrid->Row != R) || (BasicStringGrid->Col != C))
 {
//  BasicStringGrid->Row = R;
//  BasicStringGrid->Col = C;
  Application->CancelHint();
  TProperty p;
  m_storage.GetPropertyByIndex(R-1, &p);
  BasicStringGrid->Hint =p.GetDesc();
 }

//  cmbListEdit->Hint=p.GetDesc();

}
//---------------------------------------------------------------------------

