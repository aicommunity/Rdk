//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UStringListInputFormUnit.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TUStringListInputForm *UStringListInputForm;


TUParameterDescription::TUParameterDescription(void)
{

}

TUParameterDescription::TUParameterDescription(const TUParameterDescription &copy)
: Name(copy.Name), Value(copy.Value)
{

}

bool TUParameterDescription::operator == (const TUParameterDescription &copy)
{
 return Name == copy.Name;
}

bool TUParameterDescription::operator != (const TUParameterDescription &copy)
{
 return !((*this) == copy);
}

//---------------------------------------------------------------------------
__fastcall TUStringListInputForm::TUStringListInputForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
}

/// Число параметров
int TUStringListInputForm::GetNumParameters(void)
{
 return int(ParametersList.size());
}

bool TUStringListInputForm::SetNumParameters(int value)
{
 if(value <0)
  return false;
 if(value == int(ParametersList.size()))
  return true;

 ParametersList.resize(value);
 ResultParametersList.resize(value);
 return true;
}

/// Имя параметра с заданным индексом
const std::string& TUStringListInputForm::GetParameterName(int index)
{
 return ParametersList[index].Name;
}

bool TUStringListInputForm::SetParameterName(int index, const std::string &value)
{
 if(index <0 || index >=int(ParametersList.size()))
  return false;

 ParametersList[index].Name=ResultParametersList[index].Name=value;
 return true;
}

/// Значение параметра с заданным индексом
const std::string& TUStringListInputForm::GetParameterValue(int index)
{
 if(index <0 || index >=int(ParametersList.size()))
  return false;

 return ResultParametersList[index].Value;
}

bool TUStringListInputForm::SetParameterValue(int index, const std::string &value, bool as_default)
{
 if(index <0 || index >=int(ParametersList.size()))
  return false;

 ResultParametersList[index].Value=value;
 if(as_default)
  ParametersList[index].Value=value;

 return true;
}

/// Заголовок окна
std::string TUStringListInputForm::GetCaption(void)
{
 return AnsiString(Caption).c_str();
}

bool TUStringListInputForm::SetCaption(const std::string &value)
{
 Caption=value.c_str();
 return true;
}

/// Обновляет интерфейс
void TUStringListInputForm::AUpdateInterface(void)
{
 StringGrid->RowCount=ParametersList.size()+1;
 StringGrid->ColCount=2;

 int index=StringGrid->Row;

 if(!ParametersList.empty())
  StringGrid->FixedRows=1;
 StringGrid->Cells[0][0] = "Name";
 StringGrid->Cells[0][1] = "Value";

 for(size_t i=0;i<ResultParametersList.size();i++)
 {
  StringGrid->Cells[0][i+1] = ResultParametersList[i].Name.c_str();
  StringGrid->Cells[1][i+1] = ResultParametersList[i].Value.c_str();
 }

 if(index>=0 && index< StringGrid->RowCount)
  StringGrid->Row=index;

 if(StringGrid->Row>0)
  Edit->Text=ResultParametersList[StringGrid->Row-1].Value.c_str();
}

//---------------------------------------------------------------------------
void __fastcall TUStringListInputForm::OkButtonClick(TObject *Sender)
{
 ModalResult=mrOk;
}
//---------------------------------------------------------------------------
void __fastcall TUStringListInputForm::CancelButtonClick(TObject *Sender)
{
 ModalResult=mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TUStringListInputForm::RestoreButtonClick(TObject *Sender)
{
 ResultParametersList=ParametersList;
 UpdateInterface(true);
}
//---------------------------------------------------------------------------
void __fastcall TUStringListInputForm::EditKeyPress(TObject *Sender, System::WideChar &Key)

{
 if(UpdateInterfaceFlag)
  return;
 if(Key == VK_RETURN)
 {
  if(StringGrid->Row>0)
   ResultParametersList[StringGrid->Row-1].Value=AnsiString(Edit->Text).c_str();
  UpdateInterface(true);
 }
}
//---------------------------------------------------------------------------
void __fastcall TUStringListInputForm::StringGridClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;
 UpdateInterface(true);
}
//---------------------------------------------------------------------------
void __fastcall TUStringListInputForm::StringGridDblClick(TObject *Sender)
{
 if(UpdateInterfaceFlag)
  return;
 UpdateInterface(true);
}
//---------------------------------------------------------------------------
void __fastcall TUStringListInputForm::StringGridSelectCell(TObject *Sender, int ACol,
		  int ARow, bool &CanSelect)
{
 if(UpdateInterfaceFlag)
  return;
 UpdateInterface(true);
}
//---------------------------------------------------------------------------
void __fastcall TUStringListInputForm::FormShow(TObject *Sender)
{
 UpdateInterface(true);
}
//---------------------------------------------------------------------------
