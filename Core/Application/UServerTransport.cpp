#include "UServerTransport.h"
#include "../../Deploy/Include/rdk_cpp_initdll.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UServerTransport::UServerTransport(void)
{

}

UServerTransport::~UServerTransport(void)
{

}
// --------------------------

/// Кодирует строку в вектор
void UServerTransport::ConvertStringToVector(const std::string &source, UParamT &dest)
{
 dest.resize(source.size());
 if(source.size()>0)
  memcpy(&dest[0],source.c_str(),source.size());
}

/// Кодирует вектор в строку
void UServerTransport::ConvertVectorToString(const UParamT &source, std::string &dest)
{
 dest.resize(source.size());
 if(source.size()>0)
  memcpy(&dest[0],&source[0],source.size());
}

void UServerTransport::ProcessIncomingData(std::string &bind, std::vector<URpcCommandInternal> &command_list)
{
  vector<unsigned char> client_buffer;
  if(!command_list.empty())
  {
   command_list.clear();
  }
  //Сим вычитываем байты в буфер
  int length = this->ReadIncomingBytes(bind, client_buffer);
  if(length>0)
  {
   //Берем интерпретатор байтов в пакеты
   std::map<std::string, RDK::UTransferReader>::iterator I=PacketReaders.find(bind);
   //Если не нашли - выходим
   if(I == PacketReaders.end())
   {
//  SetEvent(CommandQueueUnlockEvent);
//  SetEvent(ServerReceivingNotInProgress);
	return;
   }
   //Что-тот процессится //TODO: Понять, что там внутри
   I->second.ProcessDataPart2(client_buffer);
   //LogMessage(RDK_EX_DEBUG, (std::string("Number of decoded packets: ")+sntoa(I->second.GetNumPackets())).c_str());
   MLog_LogMessage(RDK_SYS_MESSAGE,RDK_EX_DEBUG, (std::string("Number of decoded packets: ")+sntoa(I->second.GetNumPackets())).c_str());
   while(I->second.GetNumPackets()>0)
   {
	UTransferPacket packet=I->second.GetFirstPacket();
	I->second.DelFirstPacket();
	if(packet.GetNumParams()>0)
	{
//   RDK::URpcCommandInternal cmd;

	 RDK::URpcCommandInternal CurrentProcessedCommand;
	 CurrentProcessedCommand.RecepientId=bind;
	 std::string req;
	 ConvertVectorToString(packet(0),req);
	 CurrentProcessedCommand.Request=req;
	 CurrentProcessedCommand.IsDecoded=false;
	 if(!CurrentProcessedCommand.DecodeBasicData())
	 {
	  // TODO: пишем в лог ошибку декодирования
	  Log_LogMessage(RDK_EX_DEBUG, std::string("Command decode error!").c_str());
	 }
	 else
	 {
	  command_list.push_back(CurrentProcessedCommand);
	 }
	}
   }
  }
}

/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
int UServerTransport::ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes)
{
 return 0;
}

/// Отправить ответ на команду соответствующему получателю
void UServerTransport::SendResponseBuffer(std::vector<unsigned char> buffer, std::string &responce_addr)
{

}

void UServerTransport::ConnectClient(std::string& bind)
{
 PacketReaders[bind].ResetProcessing();
 PacketReaders[bind].ClearPacketList();
}

void UServerTransport::DisconnectClient(std::string& bind)
{
 std::map<std::string, RDK::UTransferReader>::iterator I=PacketReaders.find(bind);
 if(I != PacketReaders.end())
 {
  PacketReaders.erase(I);
 }
}

void UServerTransport::SetServerBinding(std::string &interface_address, int port)
{

}

//Получение адреса интерфейса управления сервером
std::string UServerTransport::GetServerBindingInterfaceAddress(void)
{
 return std::string("");
}

//Получение адреса интерфейса управления сервером
int UServerTransport::GetServerBindingPort(void) const
{
 return 0;
}

///Инициировать остановку сервера, отключить все приемники
void UServerTransport::ServerStop()
{

}

///Инициировать остановку сервера, отключить все приемники
void UServerTransport::ServerStart()
{

}

// --------------------------
// Общие методы управления контроллером
// --------------------------
// Сохраняет параметры интерфейса в xml
/*void UServerTransport::SaveParameters(RDK::USerStorageXML &xml)
{

}

// Загружает параметры интерфейса из xml
void UServerTransport::LoadParameters(RDK::USerStorageXML &xml)
{

}*/
// --------------------------

} //namespace RDK
