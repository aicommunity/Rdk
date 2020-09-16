//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TMatrixFormUnit.h"
#include "rdk.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMatrixForm *MatrixForm;
//---------------------------------------------------------------------------
__fastcall TMatrixForm::TMatrixForm(TComponent* Owner)
	: TUVisualControllerForm(Owner)
{
 UpdateInterval=-1;
 CheckModelFlag=true;
 SelectedRow=SelectedCol=-1;
 PropertyType=0;
}

// Если force_update == true, то интерфейс обновляется
// вне зависимости от UpdateInterval
void TMatrixForm::AUpdateInterface(void)
{
 if(ComponentMatrixName.empty())
  return;

 if(PropertyMatrixName.empty())
  return;

 if(PropertyType <1 || PropertyType>4)
  return;

 ComponentNamePanel->Caption=(ComponentMatrixName+std::string(":")+PropertyMatrixName).c_str();

 switch(PropertyType)
 {
 case 1:
 {
  RDK::MDMatrix<double> m;
  {
   RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();
   m=engine->Model_GetComponentPropertyData<RDK::MDMatrix<double> >(ComponentMatrixName.c_str(), PropertyMatrixName.c_str());
  }
  //ReadParameterValue(ComponentMatrixName, PropertyMatrixName, m);

  StringGrid->ColCount=m.GetCols()+1;
  StringGrid->RowCount=m.GetRows()+1;
  for(int i=0;i<m.GetRows();i++)
  {
   StringGrid->Cells[0][i+1]=IntToStr(i);
   for(int j=0;j<m.GetCols();j++)
   {
	StringGrid->Cells[j+1][0]=IntToStr(j);
	StringGrid->Cells[j+1][i+1]=FloatToStr(m(i,j));
   }
  }
 }
 break;

 case 2:
 {
  RDK::MDMatrix<int> m;
  {
   RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();
   m=engine->Model_GetComponentPropertyData<RDK::MDMatrix<int> >(ComponentMatrixName.c_str(), PropertyMatrixName.c_str());
  }
  //ReadParameterValue(ComponentMatrixName, PropertyMatrixName, m);

  StringGrid->ColCount=m.GetCols()+1;
  StringGrid->RowCount=m.GetRows()+1;
  for(int i=0;i<m.GetRows();i++)
  {
   StringGrid->Cells[0][i+1]=IntToStr(i);
   for(int j=0;j<m.GetCols();j++)
   {
	StringGrid->Cells[j+1][0]=IntToStr(j);
	StringGrid->Cells[j+1][i+1]=IntToStr(m(i,j));
   }
  }
 }
 break;

 case 3:
 {
  RDK::MDVector<double> m;
  {
   RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();
   m=engine->Model_GetComponentPropertyData<RDK::MDVector<double> >(ComponentMatrixName.c_str(), PropertyMatrixName.c_str());
  }
  //ReadParameterValue(ComponentMatrixName, PropertyMatrixName, m);

  StringGrid->ColCount=m.GetCols()+1;
  StringGrid->RowCount=2;
  StringGrid->Cells[0][1]="0";
  for(int j=0;j<m.GetCols();j++)
  {
   StringGrid->Cells[j+1][0]=IntToStr(j);
   StringGrid->Cells[j+1][1]=FloatToStr(m(j));
  }
 }
 break;

 case 4:
 {
  RDK::MDVector<int> m;
  {
   RDK::UELockPtr<RDK::UEngine> engine=RDK::GetEngineLock();
   m=engine->Model_GetComponentPropertyData<RDK::MDVector<int> >(ComponentMatrixName.c_str(), PropertyMatrixName.c_str());
  }
  //ReadParameterValue(ComponentMatrixName, PropertyMatrixName, m);

  StringGrid->ColCount=m.GetCols()+1;
  StringGrid->RowCount=2;
  StringGrid->Cells[0][1]="0";
  for(int j=0;j<m.GetCols();j++)
  {
   StringGrid->Cells[j+1][0]=IntToStr(j);
   StringGrid->Cells[j+1][1]=FloatToStr(m(j));
  }
 }
 break;
 }
}

// Возврат интерфейса в исходное состояние
void TMatrixForm::AClearInterface(void)
{
 ComponentMatrixName="";
 PropertyMatrixName="";
 SelectedRow=SelectedCol=-1;
 PropertyType=0;
}

/// Выбирает матрицу для наблюдения
bool TMatrixForm::SelectMatrix(const std::string &comp_name, const std::string &prop_name)
{
 RDK::UELockPtr<RDK::UNet> model=RDK::GetModelLock<RDK::UNet>();
 if(!model)
  return false;

 RDK::UEPtr<RDK::UNet> comp;
 if(comp_name.empty())
 {
  comp=RDK::GetModel<RDK::UNet>();
 }
 else
 {
  comp=model->GetComponentL<RDK::UNet>(comp_name,true);
  if(!comp)
   return false;
 }

 RDK::UIProperty* prop=comp->FindProperty(prop_name);

 if(!prop)
  return false;

 if(prop->GetLanguageType() == typeid(RDK::MDMatrix<double>))
 {
  PropertyType=1;
 }
 else
 if(prop->GetLanguageType() == typeid(RDK::MDMatrix<int>))
 {
  PropertyType=2;
 }
 else
 if(prop->GetLanguageType() == typeid(RDK::MDVector<double>))
 {
  PropertyType=3;
 }
 else
 if(prop->GetLanguageType() == typeid(RDK::MDVector<int>))
 {
  PropertyType=4;
 }
 else
 {
  return false;
 }

 ComponentMatrixName=comp_name;
 PropertyMatrixName=prop_name;
 SelectedRow=SelectedCol=0;

 UpdateInterface(true);
}

//---------------------------------------------------------------------------
void __fastcall TMatrixForm::FormShow(TObject *Sender)
{
 UpdateInterface();

 if(SelectedCol<0 && StringGrid->ColCount>1)
  SelectedCol=0;

 if(SelectedRow<0 && StringGrid->RowCount>1)
  SelectedRow=0;
}
//---------------------------------------------------------------------------
void __fastcall TMatrixForm::StringGridSelectCell(TObject *Sender, int ACol, int ARow,
          bool &CanSelect)
{
 SelectedRow=ARow-1;
 SelectedCol=ACol-1;
}
//---------------------------------------------------------------------------
