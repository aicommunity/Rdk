#ifndef UTRANSFERPACKET_CPP
#define UTRANSFERPACKET_CPP

#include "UTransferPacket.h"
#include <string.h>
//#include <mem.h>

namespace RDK {

unsigned char UPacketPrefix[16]={0x55,0x1a,0x78,0x1b,0x76,0x44,0x11,0x28,0x2a,0x85,0xcc,0x7d,0x89,0x76,0x8f,0x15};

// Методы
// -------------------------
// Конструторы и деструкторы
// -------------------------
UTransferPacket::UTransferPacket(void)
{
 CmdId=0;
 NumParams=0;
 Checksum=0;
 PacketSize=sizeof(int)*8;
}

UTransferPacket::UTransferPacket(const UTransferPacket &packet)
{
(*this)=packet;
}

UTransferPacket::UTransferPacket(const UParamT &buffer)
{
 if(!Load(buffer))
  {
   CmdId=0;
   NumParams=0;
   Checksum=0;
   PacketSize=sizeof(int)*8;
  }
}

UTransferPacket::~UTransferPacket(void)
{
}
// -------------------------

// -------------------------
// Методы управления данными
// -------------------------
// Устанавливает номер команды
bool UTransferPacket::SetCmdId(int cmdid)
{
 if(CmdId == cmdid)
  return true;

 CmdId=cmdid;
 return true;
}

// Устанавливает число параметров команды
bool UTransferPacket::SetNumParams(int numparams)
{
 if(numparams < 0 || numparams > MAX_NUM_PARAMS)
  return false;

 if(NumParams == numparams)
  return true;

 if(!numparams)
  {
   Params.resize(0);
  }
 else
  {
   Params.resize(numparams);
  }

 NumParams=numparams;
 return true;
}

// Устанавливает размер буфера параметра
bool UTransferPacket::SetParamSize(int i, int size)
{
 if(i<0 || i>=NumParams || size < 0 || size > MAX_PARAM_SIZE)
  return false;

 Params[i].resize(size);
 return true;
}

// Устанавливает значение i-го параметра
// Меняет текущий размер буфера параметра на требуемое значение
bool UTransferPacket::SetParam(int i, const UParamT &value, int istart, int istop)
{
 if(i<0 || i >= NumParams || istart < 0)
  return false;

 if(istop == -1)
 {
  if(value.size()-istart>MAX_PARAM_SIZE)
   return false;
  Params[i].assign(value.begin()+istart,value.end());
 }
 else
 {
  if(value.size()-istart-istop>MAX_PARAM_SIZE)
   return false;
  Params[i].assign(value.begin()+istart,value.begin()+istop);
 }
 return true;
}

// Устанавливает значение i-го параметра
// Меняет текущий размер буфера параметра на требуемое значение
bool UTransferPacket::SetParam(int i, const std::string &value)
{
 if(i<0 || i >= NumParams)
  return false;

 Params[i].resize(value.size());
 if(!value.empty())
  memcpy(&Params[i][0],&value[0],value.size()*sizeof(char));

 return true;
}

bool UTransferPacket::SetParam(int i, const std::vector<char> &value)
{
 if(i<0 || i >= NumParams)
  return false;

 Params[i].resize(value.size());
 if(!value.empty())
  memcpy(&Params[i][0],&value[0],value.size()*sizeof(char));

 return true;
}



// Устанавливает значение i-го параметра
// декодируя буфер 'value' и возвращает индекс на следующий
// за последним байтом параметра байт буфера
// или возвращает -1 в случае ошибки
int UTransferPacket::DecodeParam(int i, const UParamT &value, int istart)
{
 int size;

 if(i<0 || i >= NumParams || istart < 0)
  return -1;

 size=IntCompose(value, istart);

 if(size > MAX_PARAM_SIZE)
  return -1;

 if(!SetParamSize(i,size))
  return -1;

 for(int j=0;j<size;j++)
  Params[i][j]=value[istart+j+4];

 return istart+size+sizeof(int);
}

// Пересчитывает контрольную сумму и возвращает ее
unsigned int UTransferPacket::CalcChecksum(void)
{
 Checksum=CmdId;
 for(int i=0;i<NumParams;i++)
  for(unsigned int j=0;j<Params[i].size();j++)
   Checksum+=Params[i][j];

 return Checksum;
}

// Вычисляет значение int по участку вектора
unsigned int UTransferPacket::IntCompose(const UParamT &value, int istart)
{ 
 memcpy((void*)&Temp,&(value[istart]),sizeof(int));
             
 return Temp; 
}

const UParamT& UTransferPacket::IntDivide(unsigned int value, UParamT &buffer, int istart) const
{ 
 memcpy(&(buffer[istart]),&value,sizeof(int));
 return buffer;
}


// -------------------------

// -------------------------
// Методы загрузки пакета
// -------------------------
// Анализирует полученный буфер и возвращает
// + число байт недоcтающих для декодирования пакета
// - число байт лишних в буфере
// 0 - если декодирование возможно
int UTransferPacket::CheckBuffer(const UParamT &buffer, int buffersize)
{
 if(buffersize < sizeof(UPacketPrefix)+sizeof(int)*2)
  return sizeof(UPacketPrefix)+sizeof(int)*2-buffersize;

 if(memcmp(UPacketPrefix,&buffer[0],sizeof(UPacketPrefix)))
  return 0;

 if(IntCompose(buffer,20) > MAX_PACKET_SIZE)
  return 0;

 return IntCompose(buffer,20)-buffersize;
}

int UTransferPacket::CheckBuffer(const UParamT &buffer, int buffersize, int start_index)
{
 if((buffersize-start_index) < sizeof(UPacketPrefix)+sizeof(int)*2)
  return sizeof(UPacketPrefix)+sizeof(int)*2-(buffersize-start_index);

 if(memcmp(UPacketPrefix,&buffer[start_index],sizeof(UPacketPrefix)))
  return 0;

 if(IntCompose(buffer,start_index+20) > MAX_PACKET_SIZE)
  return 0;

 return IntCompose(buffer,start_index+20)-buffersize;
}

// Ищет в буфере начало пакета и возвращает индекс начала
// или -1 если пакет не найден
int UTransferPacket::FindPacketInBuffer(const UParamT &buffer, int start_index)
{
 if(start_index<0 || start_index>=int(buffer.size()))
  return -1;

 if(int(buffer.size())-start_index<sizeof(UPacketPrefix))
  return -1;

 for(size_t i=start_index;i<buffer.size()-sizeof(UPacketPrefix);i++)
  if(!memcmp(UPacketPrefix,&buffer[i],sizeof(UPacketPrefix)))
   return i;

 return -1;
}

// Загружает все данные пакета из массива 'buffer'
bool UTransferPacket::Load(const UParamT &buffer, int start_index)
{
 int pbuf=sizeof(UPacketPrefix)+sizeof(int)*3;
 int tmpPacketSize=sizeof(UPacketPrefix)+sizeof(int)*3;


// if(buffer.size() < sizeof(UPacketPrefix)+sizeof(int)*4)
//  return false;
 if(int(buffer.size())-start_index < sizeof(UPacketPrefix)+sizeof(int)*2)
  return false;


 if(memcmp(UPacketPrefix,&buffer[start_index],sizeof(UPacketPrefix)))
  return false;

 if(!SetCmdId(IntCompose(buffer,start_index+sizeof(UPacketPrefix))))
  return false;

 PacketSize=IntCompose(buffer,start_index+sizeof(UPacketPrefix)+4);
 if(PacketSize > MAX_PACKET_SIZE)
  return false;

 if(PacketSize > (int)buffer.size()-start_index)
  return false;

 if(IntCompose(buffer,start_index+sizeof(UPacketPrefix)+8) > MAX_NUM_PARAMS)
  return false;

 if(!SetNumParams(IntCompose(buffer,start_index+sizeof(UPacketPrefix)+8)))
  return false;

 for(int i=0;i<NumParams;i++)
  {
   pbuf=DecodeParam(i,buffer,start_index+pbuf);
   if(pbuf == -1)
    return false;
   tmpPacketSize+=GetParamSize(i)+sizeof(int);
  }

 unsigned int checksum=IntCompose(buffer,start_index+pbuf);
 tmpPacketSize+=sizeof(int);
 if(PacketSize != tmpPacketSize)
  return false;
 if(checksum != CalcChecksum())
  return false;

 return true;
}

// Сохраняет все данные пакета в массив 'buffer'
// Память должна быть выделена
bool UTransferPacket::Save(UParamT &buffer)
{
 int pbuf=0;
 
 int psize=GetPacketSize();
 if(psize > MAX_PACKET_SIZE || psize < 0)
  return false;

 buffer.resize(psize);

 memcpy(&buffer[0],UPacketPrefix,sizeof(UPacketPrefix));
 pbuf+=sizeof(UPacketPrefix);
 IntDivide(CmdId,buffer,pbuf); pbuf+=sizeof(CmdId);
 PacketSize=GetPacketSize();
 IntDivide(PacketSize,buffer,pbuf); pbuf+=sizeof(PacketSize);
 IntDivide(NumParams,buffer,pbuf); pbuf+=sizeof(NumParams);
 for(int i=0;i<Params.size();i++)
  {
   (*this)(i,buffer,pbuf);
   pbuf+=Params[i].size()+sizeof(int);
  }

 CalcChecksum();
 IntDivide(Checksum,buffer,pbuf); pbuf+=sizeof(Checksum);

 return true;
}

// Возвращает длину пакета в байтах
int UTransferPacket::GetPacketSize(void) const
{
 int size=0;
 
 int psize=0;
 
 psize=Params.size();

 for(int i=0;i<Params.size();i++)
  size+=Params[i].size();

 return size+sizeof(UPacketPrefix)+NumParams*sizeof(int)+sizeof(int)*4;
}
// -------------------------

// -------------------------
// Перегруженные операторы
// -------------------------
// Оператор присваивания
UTransferPacket& UTransferPacket::operator = (const UTransferPacket &packet)
{
 CmdId=packet.CmdId;
 Checksum=packet.Checksum;
 Params=packet.Params;
 NumParams=packet.NumParams;

 return *this;
}

// Возвращает данные i-го параметра в виде массива 
UParamT& UTransferPacket::operator () (int i, UParamT &buffer, int istart)
{
 if(i<0 || i>= NumParams || istart < 0)
  return buffer;

 int size=Params[i].size();

 IntDivide(size,buffer,istart);
 for(int j=0;j<size;j++)
  buffer[istart+j+4]=Params[i][j];

 return buffer;
}
// -------------------------


// Класс, обеспечивающий считывание последовательности пакетов из потока
// class UTransferReader
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UTransferReader::UTransferReader(void)
{
 LastSize=0;
 ReadIndex=0;
 PacketInProgress=false;
}

UTransferReader::~UTransferReader(void)
{
}
// --------------------------

// --------------------------
// Методы управления списком команд
// --------------------------
/// Очищает список
void UTransferReader::ClearPacketList(void)
{
 PacketList.clear();
}

/// Возвращает самый старый пакет и удаляет его из списка
const UTransferPacket& UTransferReader::GetFirstPacket(void) const
{
 if(PacketList.empty())
  throw 1; // Заглушка

 return *PacketList.begin();
}

/// Удаляет самый старый пакет
void UTransferReader::DelFirstPacket(void)
{
 PacketList.erase(PacketList.begin());
}

/// Возвращает самый новый пакет и удаляет его из списка
const UTransferPacket& UTransferReader::GetLastPacket(void) const
{
 if(PacketList.empty())
  throw 1; // Заглушка

 std::list<UTransferPacket>::const_iterator I=PacketList.end();
 --I;

 return *I;
}

/// Удаляет самый новый пакет
void UTransferReader::DelLastPacket(void)
{
 std::list<UTransferPacket>::iterator I=PacketList.end();
 --I;

 PacketList.erase(I);
}

/// Возвращает ссылку на список пакетов
std::list<UTransferPacket>& UTransferReader::GetPacketList(void)
{
 return PacketList;
}

/// Возвращает число пакетов в очереди
int UTransferReader::GetNumPackets(void) const
{
 return int(PacketList.size());
}
// --------------------------

// --------------------------
// Методы обработки потока
// --------------------------
// Обрабатывает очередную порцию данных
int UTransferReader::ProcessDataPart(const UParamT &buffer)
{
 ClientBuffer.insert(ClientBuffer.end(),buffer.begin(), buffer.end());
 if(!PacketInProgress)
 {
  int start_search=0;
  do
  {

/*   if(start_search>int(buffer.size()))
   {
	ResetProcessing();
	return 1;
   } */
   start_search=Packet.FindPacketInBuffer(ClientBuffer, start_search);
   if(start_search<0)
   {
	return 1;
   }


   if(start_search>0)
   {
	ClientBuffer.erase(ClientBuffer.begin(),ClientBuffer.begin()+start_search);
	start_search=0;
   }
   LastSize=Packet.CheckBuffer(ClientBuffer,ClientBuffer.size(),start_search);
   if(LastSize<=int(ClientBuffer.size())-start_search+24)
   {
	if(!Packet.Load(ClientBuffer,start_search))
	{
	 ResetProcessing();
	 start_search+=24;
	 continue;
//	 ClientError=2;
	}
	else
	{
	 PacketList.push_back(Packet);
	 LastSize=0;
	 start_search+=Packet.GetPacketSize();
	 if(start_search>0)
	  ClientBuffer.erase(ClientBuffer.begin(),ClientBuffer.begin()+start_search);
	 PacketInProgress=false;
	}
//	int j=Packet.FindPacketInBuffer(buffer, start_search);
//	if(j<0)
//	 return 1;

//   ClientBuffer.assign(buffer.begin()+j, buffer.end());
//    LastSize=Packet.CheckBuffer(ClientBuffer,ClientBuffer.size());
   }
   else
   {
//	LastSize-=i-24;
	PacketInProgress=true;
	return 1;
   }
  } while(!PacketInProgress);
 }
 else
 {
//  ClientBuffer.insert(ClientBuffer.end(),buffer.begin(),buffer.end());
  if(LastSize<0)
  {
   ResetProcessing();
   return 1;
  }
  if(LastSize<=ClientBuffer.size()+24)
  {
   PacketInProgress=false;
   int start_search=0;
	if(!Packet.Load(ClientBuffer))
	{
	 ResetProcessing();
	 ClientBuffer.erase(ClientBuffer.begin(),ClientBuffer.begin()+24);
//	 start_search+=int(buffer.size())-LastSize;
	 return 1;
	}
	else
	{
	 PacketList.push_back(Packet);
	 LastSize=0;
	 PacketInProgress=false;
	 start_search+=Packet.GetPacketSize();
	 if(start_search>0)
	  ClientBuffer.erase(ClientBuffer.begin(),ClientBuffer.begin()+start_search);
	}

/*   do
   {
	if(!Packet.Load(ClientBuffer))
	{
	 ResetProcessing();
	 return 1;
//	 ClientError=2;
	 start_search+=int(buffer.size())-LastSize;
	}
	else
	{
	 PacketList.push_back(Packet);
	 LastSize=0;
	 PacketInProgress=false;
	 start_search+=Packet.GetPacketSize();
	}

	if(start_search>int(buffer.size()))
	{
	 ResetProcessing();
	 return 1;
	}
	int i=Packet.FindPacketInBuffer(buffer, start_search);
	ClientBuffer.assign(buffer.begin()+i, buffer.end());
	LastSize=Packet.CheckBuffer(ClientBuffer,ClientBuffer.size());
	if(LastSize<=ClientBuffer.size()+24)
	{
	 start_search+=int(buffer.size())-LastSize;
	}
	else
	{
	 LastSize-=i-24;
	 PacketInProgress=true;
	}
   } while(!PacketInProgress);  */
  }
  else
  {
//   LastSize-=int(ClientBuffer.size())-24;
  }
 }
 return 0;
}

/// Прерывает текущю обработку если она была
void UTransferReader::ResetProcessing(void)
{
 LastSize=0;
 PacketInProgress=false;
}
// --------------------------


}

#endif
