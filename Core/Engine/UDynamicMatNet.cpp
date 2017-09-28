//---------------------------------------------------------------------------

#pragma hdrstop

#include "UDynamicMatNet.h"
//---------------------------------------------------------------------------

namespace RDK {

// --------------------------
// Методы доступа к свойствам
// --------------------------
// Возвращает вектор выходных данных
// Возвращает размер вектора входов InputData по индексу
// Проверяет индекс на корректность и возвращает 0, если такого входа нет фактически
MMatrixSize UDynamicMatNet::GetInputDataSize(size_t index) const
{
 if(index >= InputData.size())
  return MMatrixSize(0,0);

 if(!InputData[index])
  return MMatrixSize(0,0);

 return (*InputData[index])->GetMatrixSize();
}

// Возвращает суммарный размер всех векторов входов
size_t UDynamicMatNet::GetFullInputDataSize(void) const
{
 throw 1; // todo: //return FullInputDataSize;
}
// --------------------------

// ----------------------
// Методы управления выходными данными
// ----------------------
bool UDynamicMatNet::SetOutputDataSize(int index, const MMatrixSize &size)
{
 if(index < 0)
  return false;

 if(OutputData.size() > size_t(index))
 {
  if((*OutputData[index])->GetMatrixSize() == size)
   return true;
 }
 else
 {
  SetNumOutputs(index+1);
  Build();
 }

 OutputData[index]->Resize(size);
/*
 std::map<std::string, std::vector<PUAConnector> >::const_iterator I=RelatedConnectors.begin();
 for(;I != RelatedConnectors.end();++I)
  for(size_t i=0;i<I->second.size();i++)
  {
   static_pointer_cast<UADItem>(I->second[i])->UpdatePointers();
   static_pointer_cast<UADItem>(I->second[i])->CalcMinMaxInputDataSize();
  }
  */
 return true;
}

// Заполняет заданный выходной вектор данными
void UDynamicMatNet::FillOutputData(int index, const void *data)
{
 if(!Ready && !Build())
  return;

 if(index < 0 || index >= int(OutputData.size()))
  return;

 if(!data)
  OutputData[index]->ToZero();
 else
  OutputData[index]->Assign(1, (*OutputData[index])->GetSize(),(const double*)data);
}

// Заполняет все выходные вектора заданными данными
void UDynamicMatNet::FillOutputData(const void *data)
{
 if(!Ready && !Build())
  return;

 size_t size=OutputData.size();
 if(!size)
  return;

 for(size_t i=0;i<size;++i)
  if(!data)
  {
   if((*OutputData[i])->GetByteSize() && (*OutputData[i])->Void)
	OutputData[i]->ToZero();
  }
  else
   OutputData[i]->Assign(1, (*OutputData[i])->GetSize(),(const double*)data);
}
// ----------------------

}
#pragma package(smart_init)
