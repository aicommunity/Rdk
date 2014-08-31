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
#include "UPropertyIO.h"

namespace RDK {

/* *************************************************************************** */
// Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UADItem::UADItem(void)
// : OutputSize("OutputSize",this,&UADItem::SetOutputDataSize,&UADItem::GetOutputDataSize),
//   DataOutputSize("DataOutputSize",this,&UADItem::SetDataOutputSize,&UADItem::GetDataOutputSize)
{
 NumInputs=0;
 AddLookupProperty("NumInputs",ptParameter & pgSystem,new UVProperty<int,UADItem>(this,&UADItem::SetNumInputs,&UADItem::GetNumInputs));
 AddLookupProperty("AutoNumInputs",ptParameter & pgSystem,new UVProperty<bool,UADItem>(this,&UADItem::SetAutoNumInputs,&UADItem::GetAutoNumInputs));
 NumOutputs=0;
 AddLookupProperty("NumOutputs",ptParameter & pgSystem,new UVProperty<int,UADItem>(this,&UADItem::SetNumOutputs,&UADItem::GetNumOutputs));
 AddLookupProperty("AutoNumOutputs",ptParameter & pgSystem,new UVProperty<bool,UADItem>(this,&UADItem::SetAutoNumOutputs,&UADItem::GetAutoNumOutputs));

 // Указатель на первый элемент массива указателей на вектора входов
 POutputData=0;

 // Указатель на первый элемент массива указателей на вектора входов
// PInputData=0;

 // Указатель на первый элемент массива размеров векторов входов
// PInputDataSize=0;

 // Суммарное число всех входов
 FullInputDataSize=0;

 // Минимальный размер вектора входов
 MinInputDataSize=0;

 // Максимальный размер вектора входов
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
// Методы управления общедоступными свойствами
// --------------------------
// Возвращает число подключенных элементов item
const int& UADItem::GetNumInputs(void) const
{
 return NumInputs;
}

// Устанавливает число подключенных элементов item
bool UADItem::SetNumInputs(const int &value)
{
 if(NumInputs == value)
  return true;

 for(int i=value;i<NumInputs;i++)
 {
  std::string name=std::string("DataInput")+sntoa(i);
  UEPtr<UIProperty> property=FindProperty(name);
  if(property)
  {
   DisconnectFromIndex(name);
   DelLookupProperty(name);
  }
 }

 Ready=false;
 NumInputs=value;
 InputData.resize(NumInputs);


 for(int i=0;i<NumInputs;i++)
 {
  std::string name=std::string("DataInput")+sntoa(i);
  UEPtr<UIProperty> property=FindProperty(name);
  if(!property)
   AddLookupProperty(name,ptPubInput,new UVPropertyInputData<UItemData,UADItem>(this,&InputData[i],i));
  else
   property->UpdatePData(&InputData[i]);
 }


 return true;
}

// Признак включения/выключения режима автоматического увеличения числа входов
// при подключении нового item.
const bool& UADItem::GetAutoNumInputs(void) const
{
 return AutoNumInputs;
}

bool UADItem::SetAutoNumInputs(const bool &value)
{
 if(AutoNumInputs == value)
  return true;

 AutoNumInputs=value;
 return true;
}

// Возвращает число подключенных элементов item
const int& UADItem::GetNumOutputs(void) const
{
 return NumOutputs;//AssociatedConnectors.GetSize();
}

// Устанавливает число подключенных элементов item
bool UADItem::SetNumOutputs(const int &value)
{
 if(NumOutputs == value)
  return true;

 for(int i=value;i<NumOutputs;i++)
 {
  std::string name=std::string("DataOutput")+sntoa(i);
  UEPtr<UIProperty> property=FindProperty(name);
  if(property)
  {
   DisconnectAll(name);
   DelLookupProperty(name);
  }
 }

 Ready = false;
 NumOutputs=value;
 OutputData.resize(NumOutputs);

 for(int i=0;i<NumOutputs;i++)
 {
  std::string name=std::string("DataOutput")+sntoa(i);
  UEPtr<UIProperty> property=FindProperty(name);
  if(!property)
   AddLookupProperty(name,ptPubOutput,new UVPropertyOutputData<UItemData,UADItem>(this,&OutputData[i],i));
  else
   property->UpdatePData(&OutputData[i]);
 }


 return true;
}

// Признак включения/выключения режима автоматического увеличения числа входов
// при подключении нового item.
const bool& UADItem::GetAutoNumOutputs(void) const
{
 return AutoNumOutputs;
}

bool UADItem::SetAutoNumOutputs(const bool &value)
{
 if(AutoNumOutputs == value)
  return true;

 AutoNumOutputs=value;
 return true;
}

/// Возвращает число выходов к которым кто-то подключен
int UItem::GetNumActiveOutputs(void) const
{
 return int(RelatedConnectors.size());
}

/// Возвращает число коннекторов к которым подключено заданное свойство
int UItem::GetNumActiveOutputs(const NameT &item_property_name) const
{
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(item_property_name);
 if(I == RelatedConnectors.end())
  return 0;

 return int(I->second.size());
}
// --------------------------


// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает вектор выходных данных
/*inline const UItemData& UADItem::GetOutputData(int index) const
{
 return POutputData[index];
} */

// Возвращает указатель на вектор входов InputData по указателю на item
// Возвращает 0 если citem == 0 или не найден в списке подключений
const UEPtr<const UItemData>& UADItem::GetInputData(const UEPtr<UItem> &citem) const
{
 UItemData result;

 if(!citem)
  throw new EInputIndexNotExist(-1);

 UCLink indexes=GetCLink(citem);
 if(indexes.Input < 0)
  throw new EInputIndexNotExist(-1);

 return InputData[indexes.Input];
}

// Возвращает указатель на вектор входов InputData по индексу
// Проверяет индекс на корректность и возвращает 0, если такого входа нет фактически
const UEPtr<const UItemData>& UADItem::GetInputData(size_t index) const
{
 return InputData.at(index);
}

// Возвращает размер вектора входов InputData по индексу
// Проверяет индекс на корректность и возвращает 0, если такого входа нет фактически
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

// Возвращает суммарный размер всех векторов входов
size_t UADItem::GetFullInputDataSize(void) const
{
 return FullInputDataSize;
}
// --------------------------

// ----------------------
// Методы управления выходными данными
// ----------------------
bool UADItem::SetOutputDataSize(int index, int size, bool nobuild)
{
 if(index < 0)
  return false;

 if(OutputData.size() > size_t(index))
 {
  if(OutputData[index].Size == size)
   return true;
 }
 else
  OutputData.resize(index+1);

 OutputData[index].Resize(size);

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   static_pointer_cast<UADItem>(I->second[i])->UpdatePointers();
   static_pointer_cast<UADItem>(I->second[i])->CalcMinMaxInputDataSize();
  }

 if(!nobuild)
  Ready=false;
 return true;
}

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
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   static_pointer_cast<UADItem>(I->second[i])->UpdatePointers();
   static_pointer_cast<UADItem>(I->second[i])->CalcMinMaxInputDataSize();
  }

 Ready=false;
 return true;
}

// Заполняет заданный выходной вектор данными
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

// Заполняет все выходные вектора заданными данными
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
// Методы специфического доступа к  выходным данным
// --------------------------
// Инициализирует выход как указатель на объект
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

// Возвращает данные выхода как указателя на объект
void* UADItem::GetOutputDataAsPointer(int index)
{
 if(index<NumOutputs && GetOutputDataSize(index)>0 && GetOutputDataElementSize(index) == sizeof(void*))
 {
  return POutputData[index].PVoid[0];
 }
 return 0;
}

// Возвращает данные входа как указателя на объект
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
// Системные методы управления объектом
// --------------------------
// Создает экземпляр описания класса
UContainerDescription* UADItem::ANewDescription(UComponentDescription* description)
{
 return UComponent::ANewDescription(description);
}
// --------------------------

// ----------------------
// Методы доступа к системным свойствам
// ----------------------
// Размер выходных векторов
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

// Размер единичного данного вектора выходов в байтах
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
// Коммуникационные методы
// ----------------------
// Возвращает подключенный к этому коннектору объект по индексу
const UCItem& UADItem::GetCItem(int c_index) const
{
// if(c_index>=CItemList.GetSize())
//  throw EInputIndexNotExist(c_index);
// return CItemList[c_index];

 UIProperty* i_conn_property=0;

 FindInputProperty(c_index, i_conn_property);

 if(!i_conn_property)
  return DummyItem;

 std::map<std::string, std::vector<UCItem> >::const_iterator I=ConnectedItemList.find(i_conn_property->GetName());
 if(I == ConnectedItemList.end())
  return DummyItem;

 if(I->second.empty())
  return DummyItem;

 return I->second[0];
}

/// Ищет свойство-вход по заданному индексу
void UADItem::FindInputProperty(int index, UIProperty* &property) const
{
 // Ищем указатель на входные данные
 property=0;

 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
 J=PropertiesLookupTable.end();
 for(;I != J;++I)
 {
  if(I->second.Type & ptInput)
  {
   property=I->second.Property.Get();
   if(!property || !property->CheckRange(index))
   {
	property=0;
	continue;
   }

   break;
  }
 }
}

void UADItem::FindInputProperty(const NameT &connector_property_name, UIProperty* &property) const
{
 UItem::FindInputProperty(connector_property_name, property);
}

/// Ищет свойство-выход по заданному индексу
void UADItem::FindOutputProperty(int index, UIProperty* &property) const
{
 // Ищем указатель на выходные данные
 property=0;
 VariableMapCIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
 for(;I != J;++I)
 {
  if(I->second.Type & ptOutput)
  {
   property=I->second.Property.Get();
   if(!property || !property->CheckRange(index))
   {
    property=0;
	continue;
   }

   break;
  }
 }
}

/// Ищет свойство-выход по заданному индексу
void UADItem::FindOutputProperty(const NameT &item_property_name, UIProperty* &property) const
{
 return UItem::FindOutputProperty(item_property_name, property);
}



   /*
// Устанавливает связь с элементом сети 'na' со входом по индексу index.
// Переназначает связь если na уже подключен.
bool UItem::ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index)
{
 if(!UConnector::ConnectToItem(na, i_index, c_index))
  return false;

 // Ищем указатель на выходные данные
 UIProperty* output_property=0;
 na->FindOutputProperty(i_index, output_property);

// if(output_property)
//  CItemList[c_index].Name=output_property->GetName();

 // Ищем указатель на входные данные
 UIProperty* input_property=0;
 FindInputProperty(c_index, input_property);
 if(input_property)
 {
  if(input_property->GetIoType() & ipData)
  {
   if(input_property->GetIoType() & ipSingle)
   {
	if(output_property && output_property->CompareLanguageType(*input_property))
	 input_property->SetPointer(c_index,const_cast<void*>(output_property->GetPointer(i_index)));
   }
   else
   if(input_property->GetIoType() & ipRange)
   {
	if(output_property)
	 input_property->SetPointer(c_index,const_cast<void*>(output_property->GetPointer(i_index)));
   }
  }
  else
  if(input_property->GetIoType() & ipComp)
   input_property->SetPointer(c_index,na);
 }

 return true;
}
*/
// Устанавливает связь с элементом сети 'na' со входом по индексу index.
// Переназначает связь если na уже подключен.
bool UADItem::ConnectToItem(UEPtr<UItem> na, int i_index, int &c_index)
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
 InputData[c_index]=&nad->POutputData[i_index];


 UpdatePointers();
 CalcMinMaxInputDataSize();

 UIProperty* i_item_property=0;
 UIProperty* i_conn_property=0;

 if(!na)
  return false;

 nad->FindOutputProperty(i_index,i_item_property);
 FindInputProperty(c_index,i_conn_property);

 if(!i_item_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item by this index not found: ")+sntoa(i_index));
  return false;
 }

 NameT conn_prop_name;
 if(i_conn_property)
  conn_prop_name=i_conn_property->GetName();

 return ConnectToItem(na, i_item_property->GetName(), conn_prop_name, c_index);
}

/// Разрывает связь с элементом сети 'na', подключенную от i_index
void UADItem::DisconnectFromItem(UEPtr<UItem> na, int i_index)
{
 if(!na)
  return;

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.begin();
 for(;I != ConnectedItemList.end();++I)
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i].Item == na && I->second[i].Index == i_index)
	DisconnectFromIndex(I->first, I->second[i].Name,i);
   else
	++i;
  }
 }
}

/// Разрывает связь с элементом сети 'na', подключенную от i_index к c_index
void UADItem::DisconnectFromItem(UEPtr<UItem> na, int i_index, int c_index)
{
 UEPtr<UADItem> nad=dynamic_pointer_cast<UADItem>(na);
 if(!nad)
  return;

 UIProperty* i_item_property=0;
 UIProperty* i_conn_property=0;

 nad->FindOutputProperty(i_index,i_item_property);
 FindInputProperty(c_index,i_conn_property);

 if(!i_item_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Item by this index not found: ")+sntoa(i_index));
  return;
 }

 if(!i_conn_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector by this index not found: ")+sntoa(c_index));
  return;
 }

 std::map<std::string, std::vector<UCItem> >::iterator I=ConnectedItemList.find(i_conn_property->GetName());
 if(I != ConnectedItemList.end())
 {
  int i=0;
  while(i<int(I->second.size()))
  {
   if(I->second[i].Item == na && I->second[i].Name == i_item_property->GetName())
	DisconnectFromIndex(I->first, I->second[i].Name,i);
   else
    ++i;
  }
 }
}

// Разрывает все связи с элементом сети 'na'
void UADItem::DisconnectFromItem(UEPtr<UItem> na)
{
 UItem::DisconnectFromItem(na);
}

/// Разрывает связь с элементом сети 'na', подключенную от i_index
void UADItem::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name)
{
 UItem::DisconnectFromItem(na, item_property_name);
}

/// Разрывает связь с элементом сети 'na', подключенную от i_index к c_index
void UADItem::DisconnectFromItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name)
{
 UItem::DisconnectFromItem(na, item_property_name, connector_property_name);
}


 /*
// Выполняет действия после физически установленой связи
bool UADItem::AConnectToItem(UEPtr<UItem> na, int i_index, int c_index)
{
 return true;
}

// Выполняет действия после физически разорваной связи
void UADItem::ADisconnectFromItem(UEPtr<UItem> na, int i_index, int c_index)
{
}    */

// Проверяет, допустимо ли подключение заданного item к этому коннектору
bool UADItem::CheckItem(UEPtr<UItem> item, int item_index, int conn_index)
{
/* if(item->GetOutputType(item_index) == GetInputType(conn_index))
  return true;
 return false;*/
 return true;
}

// Проверяет, допустимо ли подключение заданного item к этому коннектору
bool UADItem::CheckItem(UEPtr<UItem> item, const NameT &item_property_name, const NameT &connector_property_name)
{
 return UItem::CheckItem(item, item_property_name, connector_property_name);
}

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UADItem::CheckLink(const UEPtr<UItem> &item, int item_index) const
{
 UCLink link=GetCLink(item);
 if(link.Input >=0)
 {
  if(link.Output == item_index || item_index <0)
   return true;
 }
 return false;
}

bool UADItem::CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name) const
{
 return UItem::CheckLink(connector, item_property_name);
}

// Проверяет, существует ли связь с заданным коннектором и конкретным входом
bool UADItem::CheckLink(const UEPtr<UItem> &item, int item_index, int conn_index) const
{
 UCLink link=GetCLink(item);
 if(link.Output>=0)
 {
  if(link.Input == conn_index || conn_index<0)
   return true;
 }

 return false;
}

bool UADItem::CheckLink(const UEPtr<UConnector> &connector, const NameT &item_property_name, const NameT &connector_property_name) const
{
 return UItem::CheckLink(connector, item_property_name, connector_property_name);
}


/// Ищет первый свободный вход
NameT UADItem::FindFreeInputName(void)
{
 Build();
 for(size_t i=0;i<InputData.size();i++)
 {
  if(!InputData[i])
   return std::string("DataInput")+sntoa(i);
 }
 return std::string("");
}

bool UADItem::ConnectToItem(UEPtr<UItem> na, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 if(!UItem::ConnectToItem(na, item_property_name, connector_property_name, c_index))
  return false;
 return true;
}

// Разрывает связь с элементом сети подключенным ко входу 'index'
void UADItem::DisconnectFromIndex(int c_index)
{
 if(c_index<0 || c_index >= int(InputData.size()))
  return;

 UIProperty* i_conn_property=0;
 FindInputProperty(c_index,i_conn_property);

 if(!i_conn_property)
 {
  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("Connector by this index not found: ")+sntoa(c_index));
  return;
 }

 DisconnectFromIndex(i_conn_property->GetName(), "", -1);

// InputDataSize[c_index]=0;
 InputData[c_index]=0;

 UpdatePointers();
 CalcMinMaxInputDataSize();
}

void UADItem::DisconnectFromIndex(const NameT &connector_property_name, const NameT &item_property_name, int index)
{
 UItem::DisconnectFromIndex(connector_property_name, item_property_name, index);

 // TODO тут чтото другое вместо этого:
// InputData[c_index]=0;

 UpdatePointers();
 CalcMinMaxInputDataSize();
}

void UADItem::DisconnectFromIndex(const NameT &connector_property_name)
{
 UItem::DisconnectFromIndex(connector_property_name);

 // TODO тут чтото другое вместо этого:
// InputData[c_index]=0;

 UpdatePointers();
 CalcMinMaxInputDataSize();
}


// Устанавливает связь с коннектором 'c'.
bool UADItem::Connect(UEPtr<UConnector> c, int i_index, int c_index)
{
 UEPtr<UADItem> ad_c=dynamic_pointer_cast<UADItem>(c);

 if(!ad_c)
  return false;

 if(i_index>=NumOutputs)
  SetNumOutputs(i_index+1);

 if(!Build())
  return false;

// UIProperty *i_item_property=0;
// UIProperty *i_conn_property=0;
// FindOutputProperty(i_index,i_item_property);
// c->FindInputProperty(c_index,i_conn_property);

 std::string item_property_name=std::string("DataOutput")+sntoa(i_index),
	conn_property_name;
/*
 if(i_item_property)
  item_property_name=i_item_property->GetName();
 else
  item_property_name=std::string("DataOutput")+sntoa(i_index);
  */

  if(!FindProperty(item_property_name))
   return false;

  if(c_index>=0)
  {
   if(ad_c->GetNumInputs()<=c_index)
   {
	if(ad_c->GetAutoNumInputs())
	 ad_c->SetNumInputs(c_index+1);
	else
	 LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("DataInput index out of range and AutoNumInputs == false: ")+sntoa(c_index));
   }
   conn_property_name=std::string("DataInput")+sntoa(c_index);
  }
  else
  {
   conn_property_name=c->FindFreeInputName();
   if(conn_property_name.empty())
   {
	c_index=ad_c->GetNumInputs();
	ad_c->SetNumInputs(ad_c->GetNumInputs()+1);
	conn_property_name=std::string("DataInput")+sntoa(c_index);
   }
  }


// if(!i_item_property || !i_conn_property)
//  return true;

 int cc_index=-1;
 return Connect(c, item_property_name, conn_property_name, cc_index);
}

bool UADItem::Connect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name, int &c_index)
{
 std::string conn_property_name;
 UEPtr<UADItem> ad_c=dynamic_pointer_cast<UADItem>(c);
  if(!connector_property_name.empty())
  {
   std::string::size_type i=connector_property_name.find("DataInput");
   if(i != std::string::npos)
   {
	c_index=RDK::atoi(connector_property_name.substr(9));
	if(ad_c->GetNumInputs()<=c_index)
	{
	 if(ad_c->GetAutoNumInputs())
	  ad_c->SetNumInputs(c_index+1);
	 else
	  LogMessageEx(RDK_EX_DEBUG, __FUNCTION__, std::string("DataInput index out of range and AutoNumInputs == false: ")+sntoa(c_index));
	}
    conn_property_name=std::string("DataInput")+sntoa(c_index);
   }
  }
  else
  {
   conn_property_name=c->FindFreeInputName();
   c_index=ad_c->GetNumInputs();
   ad_c->SetNumInputs(ad_c->GetNumInputs()+1);
   conn_property_name=std::string("DataInput")+sntoa(c_index);
  }


 return UItem::Connect(c, item_property_name, conn_property_name, c_index);
}

// Разрывает связь выхода этого объекта с коннектором 'c' по индексу
void UADItem::Disconnect(UEPtr<UConnector> c, int i_index, int c_index)
{
 UIProperty *i_item_property=0;
 UIProperty *i_conn_property=0;
 FindOutputProperty(i_index,i_item_property);

 UEPtr<UADItem> ad_c=dynamic_pointer_cast<UADItem>(c);

 if(!ad_c)
  return;

 ad_c->FindInputProperty(c_index,i_conn_property);

 if(!i_item_property || !i_conn_property)
  return;

 return Disconnect(ad_c, i_item_property->GetName(), i_conn_property->GetName());
}

/// Разрывает все связи выхода этого объекта с коннектором 'c'.
void UADItem::Disconnect(UEPtr<UConnector> c)
{
 UItem::Disconnect(c);
}

// Разрывает связь выхода этого объекта с коннектором 'c' по индексу
void UADItem::Disconnect(UEPtr<UConnector> c, const NameT &item_property_name, const NameT &connector_property_name)
{
 UItem::Disconnect(c, item_property_name, connector_property_name);
}

// Разрывает связь выхода этого объекта с коннектором по Id 'id'.
bool UADItem::Disconnect(const UId &id)
{
 return UItem::Disconnect(id);
}

// Возвращает текущее число соединений.
int UADItem::GetNumAConnectors(int index) const
{
 UIProperty *i_item_property=0;
 FindOutputProperty(index,i_item_property);

 if(!i_item_property)
  return 0;

 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.find(i_item_property->GetName());

 if(I==RelatedConnectors.end())
  return 0;

 return int(I->second.size());
// return AssociatedConnectors[index].GetSize();//NumAConnectors[index];
}

int UADItem::GetNumAConnectors(const NameT &item_property_name) const
{
 return UItem::GetNumAConnectors(item_property_name);
}

// Возвращает  коннектор из списка подключений.
UEPtr<UConnector> UADItem::GetAConnectorByIndex(int output, int index) const
{
 UIProperty *i_item_property=0;
 FindOutputProperty(output,i_item_property);

 if(!i_item_property)
  return 0;

 return GetAConnectorByIndex(i_item_property->GetName(), index);
}

UEPtr<UConnector> UADItem::GetAConnectorByIndex(const NameT &item_property_name, int index) const
{
 return UItem::GetAConnectorByIndex(item_property_name, index);
}
// --------------------------

// ----------------------
// Методы доступа к компонентам
// ----------------------
// Копирует этот объект в 'target' с сохранением всех компонент
// и значений параметров
bool UADItem::Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor, bool copystate) const
{
 if(!UItem::Copy(target,stor,copystate))
  return false;

 UEPtr<UADItem> item=dynamic_pointer_cast<UADItem>(target);
 return true;
}
// ----------------------

// --------------------------
// Методы управления счетом
// --------------------------
// Восстановление настроек по умолчанию и сброс процесса счета
bool UADItem::Default(void)
{
 SetNumInputs(1);
 SetAutoNumInputs(true);
 SetNumOutputs(1);
 SetAutoNumOutputs(true);
// SetOutputDataElementSize(0,sizeof(void*));
 SetOutputDataSize(0,1);

 return UItem::Default();
}

// Обеспечивает сборку внутренней структуры объекта
// после настройки параметров
// Автоматически вызывает метод Reset() и выставляет Ready в true
// в случае успешной сборки
bool UADItem::Build(void)
{
 if(IsReady())
  return true;

 // Выполняем подсчет минимального числа входов и выходов
 int min_num_inputs=0;
 int min_num_outputs=0;
 VariableMapIteratorT I=PropertiesLookupTable.begin(),
					  J=PropertiesLookupTable.end();
 for(;I != J;++I)
 {
  if(I->second.Type & ptInput)
  {
   UIProperty* property=dynamic_cast<UIProperty*>(I->second.Property.Get());
   if(!property)
	continue;

   if(property->GetMinRange()+1>min_num_inputs)
	min_num_inputs=property->GetMinRange()+1;

   if(property->GetMaxRange()+1>min_num_inputs)
	min_num_inputs=property->GetMaxRange()+1;
  }

  if(I->second.Type & ptOutput)
  {
   UIProperty* property=dynamic_cast<UIProperty*>(I->second.Property.Get());
   if(!property)
	continue;

   if(property->GetMinRange()+1>min_num_outputs)
	min_num_outputs=property->GetMinRange()+1;

   if(property->GetMaxRange()+1>min_num_outputs)
	min_num_outputs=property->GetMaxRange()+1;
  }
 }

 if(NumInputs<min_num_inputs)
  SetNumInputs(min_num_inputs);

 if(NumOutputs<min_num_outputs)
  SetNumOutputs(min_num_outputs);

 UpdatePointers();

 if(!UItem::Build())
  return false;

 return true;
}

// Сброс процесса счета без потери настроек
bool UADItem::Reset(void)
{
 if(!UContainer::Reset())
  return false;
// FillOutputData();

 return true;
}
// --------------------------

// ----------------------
// Вспомогательные методы
// ----------------------
// Обновляет входной массив по данным подключенного ко входу компонента
/*void UADItem::UpdateInputData(int index)
{
 if(index>=NumInputs)
  return;

 UEPtr<UADItem> nad=GetCItem(index);
 InputDataSize[index]=nad->POutputData[i_index].Size;
 InputData[index]=&nad->POutputData[i_index];
} */

// Обновляет указатели на массивы входов и выходов
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
	   /*
 if(NumInputs>0)
 {
  // Указатель на первый элемент массива указателей на вектора входов
//  PInputData=&InputData[0];

  // Указатель на первый элемент массива размеров векторов входов
//  PInputDataSize=&InputDataSize[0];
 }
 else
 {
  // Указатель на первый элемент массива указателей на вектора входов
//  PInputData=0;

  // Указатель на первый элемент массива размеров векторов входов
//  PInputDataSize=0;
 }
         */
 // Суммарное число всех входов
 FullInputDataSize=0;
 for(int i=0;i<NumInputs;i++)
//  FullInputDataSize+=PInputDataSize[i];
  if(InputData[i])
   FullInputDataSize+=InputData[i]->GetSize();
}


// Вычисляет минимальный и максимальный размер векторов входов
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


