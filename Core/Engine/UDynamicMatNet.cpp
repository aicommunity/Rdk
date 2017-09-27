//---------------------------------------------------------------------------

#pragma hdrstop

#include "UDynamicMatNet.h"
//---------------------------------------------------------------------------

namespace RDK {

// --------------------------
// ������ ������� � ���������
// --------------------------
// ���������� ������ �������� ������
// ���������� ������ ������� ������ InputData �� �������
// ��������� ������ �� ������������ � ���������� 0, ���� ������ ����� ��� ����������
MMatrixSize UDynamicMatNet::GetInputDataSize(size_t index) const
{
 if(index >= InputData.size())
  return MMatrixSize(0,0);

 if(!InputData[index])
  return MMatrixSize(0,0);

 return (*InputData[index])->GetMatrixSize();
}

// ���������� ��������� ������ ���� �������� ������
size_t UDynamicMatNet::GetFullInputDataSize(void) const
{
 return FullInputDataSize;
}
// --------------------------

// ----------------------
// ������ ���������� ��������� �������
// ----------------------
bool UDynamicMatNet::SetOutputDataSize(int index, const MMatrixSize &size, bool nobuild)
{
 if(index < 0)
  return false;

 if(OutputData.size() > size_t(index))
 {
  if((*OutputData[index])->GetMatrixSize() == size)
   return true;
 }
 else
  OutputData.resize(index+1);

 (*OutputData[index])->Resize(size);

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

// ��������� �������� �������� ������ �������
void UDynamicMatNet::FillOutputData(int index, const void *data)
{
 if(!Ready && !Build())
  return;

 if(index < 0 || index >= int(OutputData.size()))
  return;

 UItemData &dest=OutputData[index];

 if(!data)
  dest.ToZero();
 else
  dest.Assign(1, dest.GetSize(),(const double*)data);
}

// ��������� ��� �������� ������� ��������� �������
void UDynamicMatNet::FillOutputData(const void *data)
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
   if(dest->GetByteSize() && dest->Void)
	dest->ToZero();
  }
  else
   dest->Assign(1, dest->GetSize(),(const double*)data);
}
// ----------------------

}
#pragma package(smart_init)
