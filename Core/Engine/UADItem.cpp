/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UADITEM_CPP
#define UADITEM_CPP

#include <algorithm>
#include "UADItem.h"

namespace RDK {

// ------------------------
// ������������ � �����������
// ------------------------
UIDataInfo::UIDataInfo(void)
{
}

UIDataInfo::~UIDataInfo(void)
{
}
// ------------------------

/* *************************************************************************** */
// ������
// --------------------------
// ������������ � �����������
// --------------------------
UADItem::UADItem(void)
// : OutputSize("OutputSize",this,&UADItem::SetOutputDataSize,&UADItem::GetOutputDataSize),
//   DataOutputSize("DataOutputSize",this,&UADItem::SetDataOutputSize,&UADItem::GetDataOutputSize)
{
 // ��������� �� ������ ������� ������� ���������� �� ������� ������
 POutputData=0;

 // ��������� �� ������ ������� ������� ���������� �� ������� ������
// PInputData=0;

 // ��������� �� ������ ������� ������� �������� �������� ������
// PInputDataSize=0;

 // ��������� ����� ���� ������
 FullInputDataSize=0;

 // ����������� ������ ������� ������
 MinInputDataSize=0;

 // ������������ ������ ������� ������
 MaxInputDataSize=0;
}

UADItem::~UADItem(void)
{
/* for(size_t i=0;i<InputDataInfo.size();i++)
  if(InputDataInfo[i])
   delete InputDataInfo[i].Get();

 for(size_t i=0;i<OutputDataInfo.size();i++)
  if(OutputDataInfo[i])
   delete OutputDataInfo[i].Get();
   */
}                       
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� ������ �������� ������
/*inline const UItemData& UADItem::GetOutputData(int index) const
{
 return POutputData[index];
} */

// ���������� ��������� �� ������ ������ InputData �� ��������� �� item
// ���������� 0 ���� citem == 0 ��� �� ������ � ������ �����������
const UEPtr<const UItemData>& UADItem::GetInputData(const UEPtr<UAItem> &citem) const
{
 UItemData result;

 if(!citem)
  throw new EInputIndexNotExist;

 UCLink indexes=GetCLink(citem);
 if(indexes.Input < 0)
  throw new EInputIndexNotExist;

 return InputData[indexes.Input];
}

// ���������� ��������� �� ������ ������ InputData �� �������
// ��������� ������ �� ������������ � ���������� 0, ���� ������ ����� ��� ����������
const UEPtr<const UItemData>& UADItem::GetInputData(size_t index) const
{
 return InputData.at(index);
}

// ���������� ������ ������� ������ InputData �� �������
// ��������� ������ �� ������������ � ���������� 0, ���� ������ ����� ��� ����������
size_t UADItem::GetInputDataSize(size_t index) const
{
 if(index >= InputData.size())
  return 0;
 
 if(!InputData[index])
  return 0;

 return InputData[index]->GetSize();
/*
 try
 {
  return InputData[index]->GetSize();
 }
 catch (UEPtr<const RDK::UItemData>::EUsingZeroPtr *exception)
 {
  return 0;
 }*/
}

// ���������� ��������� ������ ���� �������� ������
size_t UADItem::GetFullInputDataSize(void) const
{
 return FullInputDataSize;
}
// --------------------------

// --------------------------
// ������ ������� � �������� ������ � �������
// --------------------------
// �������� �������� ������
const USharedPtr<UIDataInfo>& UADItem::GetOutputDataInfo(int index) const
{
 return OutputDataInfo[index];
}

bool UADItem::SetOutputDataInfo(int index, const USharedPtr<UIDataInfo> value)
{
// if(!Build())
//  return false;
 if(int(OutputDataInfo.size())<=index)
  return false;

 if(OutputDataInfo[index] == value)
  return true;

// if(OutputDataInfo[index])
//  delete OutputDataInfo[index];
 OutputDataInfo[index]=value;
 return true;
}

// �������� ������� ������
const USharedPtr<UIDataInfo>& UADItem::GetInputDataInfo(int index) const
{
 return InputDataInfo[index];
}

bool UADItem::SetInputDataInfo(int index, const USharedPtr<UIDataInfo> value)
{
// if(!Build())
//  return false;
 if(int(InputDataInfo.size())<=index)
  return false;

 if(InputDataInfo[index] == value)
  return true;

// if(InputDataInfo[index])
//  delete InputDataInfo[index];
 InputDataInfo[index]=value;
 return true;
}

// ����� �������
const NameT& UADItem::GetOutputName(int index) const
{
 return OutputNames[index];
}

bool UADItem::SetOutputName(int index, const NameT& name)
{
 if(int(OutputNames.size())<=index)
  return false;

 if(OutputNames[index] == name)
  return true;

 OutputNames[index]=name;
 return true;
}

// ����� ������
const NameT& UADItem::GetInputName(int index) const
{
 return InputNames[index];
}

bool UADItem::SetInputName(int index, const NameT& name)
{
 if(int(InputNames.size())<=index)
  return false;

 if(InputNames[index] == name)
  return true;

 InputNames[index]=name;
 return true;
}

// �������� �������� ������� � �������� ������ � item
bool UADItem::CopyDataInfo(UEPtr<UADItem> item) const
{
 if(!item)
  return false;

 for(size_t i=0;i<InputDataInfo.size();i++)
  item->SetInputDataInfo(i,InputDataInfo[i]);

 for(size_t i=0;i<OutputDataInfo.size();i++)
  item->SetOutputDataInfo(i,OutputDataInfo[i]);

 return true;
}

// �������� ����� ������ � ������� � item
bool UADItem::CopyIONames(UEPtr<UADItem> item) const
{
 if(!item)
  return false;

 for(size_t i=0;i<InputNames.size();i++)
  item->SetInputName(i,InputNames[i]);

 for(size_t i=0;i<OutputNames.size();i++)
  item->SetOutputName(i,OutputNames[i]);

 return true;
}
// --------------------------

// ----------------------
// ������ ���������� ��������� �������
// ----------------------
// ������������� ������ ������� �������� ������
/*size_t UADItem::GetOutputDataSize(int index) const
{
// if(index < 0 || index >= int(OutputData.size()))
//  return 0;

 return OutputData[index].Size;
} */

bool UADItem::SetOutputDataSize(int index, int size, bool nobuild)
{
// if(!Build())
//  return false;

 if(index < 0)// || index >= int(OutputData.size()))
  return false;

 if(OutputData.size() > size_t(index))
 {
  if(OutputData[index].Size == size)
   return true;
 }
 else
  OutputData.resize(index+1);

 OutputData[index].Resize(size);

 if(index<AssociatedConnectors.GetSize())
  for(int j=0;j<AssociatedConnectors[index].GetSize();j++)
  {
   static_pointer_cast<UADItem>(AssociatedConnectors[index][j])->UpdatePointers();
   static_pointer_cast<UADItem>(AssociatedConnectors[index][j])->CalcMinMaxInputDataSize();
  }

 if(!nobuild)
  Ready=false;
 return true;
}

// ������������� ������ ������� �������� ������ � ������
// ��������� �����, ��������� ����� ��������� ����������
/*size_t UADItem::GetByteOutputDataSize(int index) const
{
// if(index>=int(OutputData.size()))
//  return 0;

 return OutputData[index].ByteSize;
}    */

// ������������� ������ ���������� ������� ������� �������� ������ � ������
/*size_t UADItem::GetOutputDataElementSize(int index) const
{
// if(index>=int(OutputData.size()))
//  return 0;

 return OutputData[index].DataSize;
} */

bool UADItem::SetOutputDataElementSize(int index, int size)
{
// if(!Build())
//  return false;

 if(index < 0)// || index >= int(OutputData.size()))
  return false;

 if(OutputData.size() > size_t(index))
 {
  if(OutputData[index].DataSize == size)
   return true;
 }
 else
  OutputData.resize(index+1);

 OutputData[index].SetDataSize(size);
 if(index<AssociatedConnectors.GetSize())
  for(int j=0;j<AssociatedConnectors[index].GetSize();j++)
  {
   static_pointer_cast<UADItem>(AssociatedConnectors[index][j])->UpdatePointers();
   static_pointer_cast<UADItem>(AssociatedConnectors[index][j])->CalcMinMaxInputDataSize();
  }


 Ready=false;
 return true;
}

// ��������� �������� �������� ������ �������
void UADItem::FillOutputData(int index, const void *data)
{
 if(!Ready && !Build())
  return;

 if(index < 0 || index >= int(OutputData.size()))
  return;

 UItemData &dest=OutputData[index];

 if(!data)
  memset(dest.Void,0,dest.ByteSize);
 else
  dest.Assign(dest.Size,data);
}

// ��������� ��� �������� ������� ��������� �������
void UADItem::FillOutputData(const void *data)
{
 if(!Ready && !Build())
  return;

 size_t size=OutputData.size();
 if(!size)
  return;

 UItemData* dest=&OutputData[0];
 for(size_t i=0;i<size;++i,++dest)
  if(!data)
  {
   if(dest->ByteSize && dest->Void)
    memset(dest->Void,0,dest->ByteSize);
  }
  else
   dest->Assign(dest->Size,data);
}
// ----------------------

// --------------------------
// ������ �������������� ������� �  �������� ������
// --------------------------
// �������������� ����� ��� ��������� �� ������
bool UADItem::SetOutputDataAsPointer(int index, void *pointer)
{
 if(index<NumOutputs)
 {
  SetOutputDataElementSize(index,sizeof(void*));
  SetOutputDataSize(index,1);
  OutputData[index].PVoid[0]=pointer;
  return true;
 }
 return false;
}

// ���������� ������ ������ ��� ��������� �� ������
void* UADItem::GetOutputDataAsPointer(int index)
{
 if(index<NumOutputs && GetOutputDataSize(index)>0 && GetOutputDataElementSize(index) == sizeof(void*))
 {
  return POutputData[index].PVoid[0];
 }
 return 0;
}

// ���������� ������ ����� ��� ��������� �� ������
void* UADItem::GetInputDataAsPointer(int index)
{
 if(index<NumInputs && InputData[index] && InputData[index]->GetSize()>0 && InputData[index]->GetDataSize() == int(sizeof(void*)))
 {
  void *pointer=InputData[index]->PVoid[0];
  return pointer;
 }
 return 0;
}
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// ������� ��������� �������� ������
UContainerDescription* UADItem::ANewDescription(UComponentDescription* description)
{
 return 0; // ��������!!
}
// --------------------------

// ----------------------
// ������ ������� � ��������� ���������
// ----------------------
// ������ �������� ��������
vector<size_t> UADItem::GetOutputDataSize(void) const
{
 vector<size_t> result;

 size_t size=OutputData.size();
 result.resize(size);
 for(size_t i=0;i<size;i++)
  result[i]=OutputData[i].Size;

 return result;
}

bool UADItem::SetOutputDataSize(const vector<size_t> &value)
{
// if(!Build())
//  return false;

// if(OutputData.size() != value.size())
//  return false;

 size_t size=value.size();
 if(OutputData.size()<size)
  OutputData.resize(size);

 for(size_t i=0;i<size;i++)
  if(!SetOutputDataSize(i,value[i]))
   return false;

 return true;
}

// ������ ���������� ������� ������� ������� � ������
vector<size_t> UADItem::GetOutputDataElementSize(void) const
{
 vector<size_t> result;

 size_t size=OutputData.size();
 result.resize(size);
 for(size_t i=0;i<size;i++)
  result[i]=OutputData[i].DataSize;

 return result;
}

bool UADItem::SetOutputDataElementSize(const vector<size_t> &value)
{
// if(OutputData.size() != value.size())
//  return false;

 size_t size=value.size();
 if(OutputData.size()<size)
  OutputData.resize(size);

 for(size_t i=0;i<size;i++)
  if(!SetOutputDataElementSize(i,value[i]))
   return false;

 return true;
}
// ----------------------

// ----------------------
// ���������������� ������
// ----------------------
// ������������� ����� � ��������� ���� 'na' �� ������ �� ������� index.
// ������������� ����� ���� na ��� ���������.
bool UADItem::ConnectToItem(UEPtr<UAItem> na, int i_index, int &c_index)
{
 UEPtr<UADItem> nad=dynamic_pointer_cast<UADItem>(na);
 if(!nad)
  return false;

 if(c_index>=NumInputs)
 {
  if(AutoNumInputs)
  {
//   if(c_index<0)
//	c_index=NumInputs;

   if(!SetNumInputs(c_index+1))
	return false;
   if(!Build())
	return false;
  }
  else
   return false;

//  if(c_index<0)
//   c_index=NumInputs-1;
 }
 else
 if(c_index<0)
 {
  for(int i=NumInputs-1;i>=0;i--)
  {
   if(!GetCItem(i).Item)
   {
	c_index=i;
	break;
   }
  }
  if(c_index<0)
  {
   c_index=NumInputs;

   if(!SetNumInputs(c_index+1))
	return false;
   if(!Build())
	return false;
  }
 }

 USharedPtr<UIDataInfo> iteminfo, conninfo;
 iteminfo=nad->OutputDataInfo[i_index];
 conninfo=InputDataInfo[c_index];
 if(conninfo && !iteminfo)
  return false;

 if(iteminfo && conninfo && !iteminfo->Compare(conninfo))
  return false;

 InputData[c_index]=&nad->POutputData[i_index];


 UpdatePointers();
 CalcMinMaxInputDataSize();

 if(!UAConnector::ConnectToItem(static_pointer_cast<UAItem>(nad), i_index, c_index))
  return false;

 // ��������� �������� �� ������������� ����������
 VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
 for(;I != J;++I)
 {
  if(I->second.Type & ptInput)
  {
   UIPropertyIO* input_property=dynamic_cast<UIPropertyIO*>(I->second.Property.Get());
   if(!input_property || !input_property->CheckRange(c_index))
	continue;

   if(input_property->GetType() & ipData)
	input_property->SetPointer(nad->GetOutputDataAsPointer(i_index));
   else
   if(input_property->GetType() & ipComp)
	input_property->SetPointer(nad);
  }
 }

 return true;
}

// ��������� ����� � ��������� ���� ������������ �� ����� 'index'
void UADItem::DisconnectFromIndex(int c_index)
{
 if(c_index<0 || c_index >= int(InputData.size()))
  return;

 // ��������� �������� �� ��������������� ����������
 VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
 for(;I != J;++I)
 {
  if(I->second.Type & ptInput)
  {
   UIPropertyIO* input_property=dynamic_cast<UIPropertyIO*>(I->second.Property.Get());
   if(!input_property || !input_property->CheckRange(c_index))
	continue;

   input_property->SetPointer(0);
  }
 }

 UAConnector::DisconnectFromIndex(c_index);

// InputDataSize[c_index]=0;
 InputData[c_index]=0;

 UpdatePointers();
 CalcMinMaxInputDataSize();
}
// --------------------------

// ----------------------
// ������ ������� � �����������
// ----------------------
// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
bool UADItem::Copy(UEPtr<UAContainer> target, UEPtr<UAContainerStorage> stor, bool copystate) const
{
 if(!UAItem::Copy(target,stor,copystate))
  return false;

 UEPtr<UADItem>item=dynamic_pointer_cast<UADItem>(target);

 if(!CopyDataInfo(item))
  return false;

 if(!CopyIONames(item))
  return false;

 return true;
}
// ----------------------

// --------------------------
// ������ ���������� ������
// --------------------------
// �������������� �������� �� ��������� � ����� �������� �����
bool UADItem::Default(void)
{
// SetOutputDataElementSize(0,sizeof(void*));
 SetOutputDataSize(0,1);

 return UAItem::Default();
}

// ������������ ������ ���������� ��������� �������
// ����� ��������� ����������
// ������������� �������� ����� Reset() � ���������� Ready � true
// � ������ �������� ������
bool UADItem::Build(void)
{
 if(IsReady())
  return true;

 OutputData.resize(NumOutputs);
 InputData.resize(NumInputs);
// InputDataSize.resize(NumInputs);

 size_t size=OutputDataInfo.size();
// for(size_t i=NumOutputs;i<size;i++)
//  if(OutputDataInfo[i])
//   delete OutputDataInfo[i];
 OutputDataInfo.resize(NumOutputs);
 for(int i=size;i<NumOutputs;i++)
  OutputDataInfo[i]=0;

 size=InputDataInfo.size();
// for(size_t i=NumInputs;i<size;i++)
//  if(InputDataInfo[i])
//   delete InputDataInfo[i];
 InputDataInfo.resize(NumInputs);
 for(int i=size;i<NumInputs;i++)
  InputDataInfo[i]=0;

 // ����� �������
 OutputNames.resize(NumOutputs);

 // ����� ������
 InputNames.resize(NumInputs);

 UpdatePointers();

 if(!UAItem::Build())
  return false;

 return true;
}

// ����� �������� ����� ��� ������ ��������
bool UADItem::Reset(void)
{
 if(!UAContainer::Reset())
  return false;
// FillOutputData();

 return true;
}
// --------------------------

// ----------------------
// ��������������� ������
// ----------------------
// ��������� ������� ������ �� ������ ������������� �� ����� ����������
/*void UADItem::UpdateInputData(int index)
{
 if(index>=NumInputs)
  return;

 UEPtr<UADItem> nad=GetCItem(index);
 InputDataSize[index]=nad->POutputData[i_index].Size;
 InputData[index]=&nad->POutputData[i_index];
} */

// ��������� ��������� �� ������� ������ � �������
void UADItem::UpdatePointers(void)
{
 if(NumOutputs>0)
 {
  POutputData=&OutputData[0];
 }
 else
 {
  POutputData=0;
 }

 if(NumInputs>0)
 {
  // ��������� �� ������ ������� ������� ���������� �� ������� ������
//  PInputData=&InputData[0];

  // ��������� �� ������ ������� ������� �������� �������� ������
//  PInputDataSize=&InputDataSize[0];
 }
 else
 {
  // ��������� �� ������ ������� ������� ���������� �� ������� ������
//  PInputData=0;

  // ��������� �� ������ ������� ������� �������� �������� ������
//  PInputDataSize=0;
 }

 // ��������� ����� ���� ������
 FullInputDataSize=0;
 for(int i=0;i<NumInputs;i++)
//  FullInputDataSize+=PInputDataSize[i];
  if(InputData[i])
   FullInputDataSize+=InputData[i]->GetSize();
}


// ��������� ����������� � ������������ ������ �������� ������
void UADItem::CalcMinMaxInputDataSize(void)
{
/*
 vector<size_t>::const_iterator I;

 I=min_element(InputDataSize.begin(),InputDataSize.end());

 if(I != InputDataSize.end())
  MinInputDataSize=*I;
 else
  MinInputDataSize=0;

 I=max_element(InputDataSize.begin(),InputDataSize.end());

 if(I != InputDataSize.end())
  MaxInputDataSize=*I;
 else
  MaxInputDataSize=0;
  */
 if(!InputData.size())
 {
  MinInputDataSize=0;
  MaxInputDataSize=0;
 }
 else
 {
  if(InputData[0])
   MaxInputDataSize=MinInputDataSize=InputData[0]->GetSize();
  else
   MaxInputDataSize=MinInputDataSize=0;
  for(int i=1;i<NumInputs;i++)
  {
   if(!InputData[i])
    continue;
   if(int(MinInputDataSize)>InputData[i]->GetSize())
	MinInputDataSize=InputData[i]->GetSize();
   if(int(MaxInputDataSize)<InputData[i]->GetSize())
	MaxInputDataSize=InputData[i]->GetSize();
  }
 }
}
// ----------------------
/* *************************************************************************** */

}
#endif


