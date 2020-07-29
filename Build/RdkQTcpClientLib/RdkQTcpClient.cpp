#include "RdkQTcpClient.h"
#include <QTcpSocket>

RdkQTcpThread::RdkQTcpThread()
{

}

RdkQTcpThread::RdkQTcpThread(QTcpSocket *socket, RDK::UTransferReader &packetReader, bool CreateSUspended, QObject *parent):
_socket(socket), QThread(parent), PacketReader(&packetReader)
{
 CalcEnable = UCreateEvent(true);
 CalcStarted = UCreateEvent(true);
 CalculationNotInProgress = UCreateEvent(false);
 CalculationInProgress = UCreateEvent(true);
 SendInProgress = UCreateEvent(true);
 ThreadError = 0;
}

RdkQTcpThread::~RdkQTcpThread()
{
 UDestroyEvent(CalcEnable);
 UDestroyEvent(CalcStarted);
 UDestroyEvent(CalculationNotInProgress);
 UDestroyEvent(CalculationInProgress);
 UDestroyEvent(SendInProgress);
}

// --------------------------
// Управление потоком
// --------------------------
void RdkQTcpThread::BeforeCalculate(void)
{

}

void RdkQTcpThread::AfterCalculate(void)
{

}

void RdkQTcpThread::run()
{
    while(isRunning()) //???
    {
     //if(WaitForSingleObject(CalcStarted,30) == WAIT_TIMEOUT)
     // continue;
     if(CalcStarted->wait(30)==false)
         continue;

   //  if(WaitForSingleObject(CalcEnable,30) == WAIT_TIMEOUT)
   //   continue;

     //if(WaitForSingleObject(CalculationInProgress,30) == WAIT_TIMEOUT)
     // continue;
     if(CalculationInProgress->wait(30)==false)
         continue;
     //ResetEvent(CalculationInProgress);
     CalculationInProgress->reset();

   //  if(WaitForSingleObject(CalculationNotInProgress,30) == WAIT_TIMEOUT)
   //   continue;
     try
     {
      if((_socket->isValid()) && (_socket->state()==QAbstractSocket::SocketState::ConnectedState) && (_socket->bytesAvailable()>0))
      {
       RDK::UParamT ClientBuffer;
       ClientBuffer.resize(1);
       /*
       TStream* dataInputStream=new TMemoryStream();
       int len=IdTCPClient->IOHandler->InputBuffer->Size;
       IdTCPClient->IOHandler->ReadStream(dataInputStream, len, false);
       ClientBuffer.resize(len/sizeof(ClientBuffer[0]));
       dataInputStream->Position=0;
       dataInputStream->ReadBuffer(&ClientBuffer[0], len);
       delete dataInputStream;
       dataInputStream=0;
       int prev_packet_size=0;
       int new_packet_size=0;
       */

       QDataStream in;
       in.setDevice(_socket);
       in.setVersion(QDataStream::Qt_5_0);
       in.startTransaction();
       //int len = _socket->bytesAvailable();
       char *buffer;
       uint length=0;
       in.readBytes(buffer, length);
       ClientBuffer.resize(length/sizeof(ClientBuffer[0]));
       memcpy(&ClientBuffer[0], buffer, length);

       int prev_packet_size=0;
       int new_packet_size=0;
       //if(WaitForSingleObject(PacketReaderUnlockEvent, INFINITE) != WAIT_TIMEOUT)
       if(PacketReaderUnlockEvent->exclusive_lock())
       {
   //	 ResetEvent(PacketReaderUnlockEvent);
        ThreadError=0;
        prev_packet_size=PacketReader->GetNumPackets();
        PacketReader->ProcessDataPart2(ClientBuffer);
        new_packet_size=PacketReader->GetNumPackets();
        //ReleaseMutex(PacketReaderUnlockEvent);
        //PacketReaderUnlockEvent->reset();
        PacketReaderUnlockEvent->exclusive_unlock();
        if(new_packet_size>prev_packet_size)
        {
         std::string str;
         str.resize(ClientBuffer.size());
         memcpy(&str[0],&ClientBuffer[0],str.size());

         //SetEvent(PacketReceivedEvent);
         PacketReceivedEvent->set();
        }

       }
       if(new_packet_size>prev_packet_size)
           CalcEnable->reset();
        //ResetEvent(CalcEnable);
      }
     }
     catch(QException &exception)
     {
      //ResetEvent(CalcEnable);
      CalcEnable->reset();
      //SetEvent(PacketReceivedEvent);
      PacketReceivedEvent->set();
      ThreadError=RDK_RPC_RECEIVE_TRANSPORT_EXCEPTION;
      this->msleep(10);
     }
     catch(...)
     {
      //ResetEvent(CalcEnable);
      CalcEnable->reset();
      //SetEvent(PacketReceivedEvent);
      PacketReceivedEvent->set();
      ThreadError=RDK_RPC_RECEIVE_TRANSPORT_EXCEPTION;
      this->msleep(10);
     }
   //  BeforeCalculate();
   //  AfterCalculate();
    // ReleaseMutex(CalculationNotInProgress);
     //SetEvent(CalculationInProgress);
     CalculationInProgress->set();
     //Sleep(1);
   //  Sleep(1);
     this->msleep(1);
    }
}

//=====================================================================================================


RdkQTcpClient::RdkQTcpClient()
{
    //TODO: Проверить, действительно ли так должно быть?
    TcpSocket = new QTcpSocket();
    Thread = new RdkQTcpThread(TcpSocket, PacketReader,false, this);
    Thread->PacketReceivedEvent = UCreateEvent(true);
    Thread->PacketReaderUnlockEvent = UCreateMutex();
    EraseTimer = new QTimer(this);
    connect(EraseTimer, SIGNAL(timeout()), this, SLOT(EraseTimerTick()));
    EraseTimer->start();
}

RdkQTcpClient::~RdkQTcpClient()
{
    EraseTimer->stop();
    if(Thread!=NULL)
    {
        Thread->terminate();
        Thread->CalculationInProgress->wait(-1);
        UDestroyEvent(Thread->PacketReceivedEvent);
        UDestroyMutex(Thread->PacketReaderUnlockEvent);
        Thread->CalculationNotInProgress->reset();
        delete Thread;
    }
    if(EraseTimer!=NULL)
     delete EraseTimer;

    if(TcpSocket!=NULL)
     delete TcpSocket;
}

void RdkQTcpClient::EraseTimerTick()
{

}


/// Добавляет в список пакет ожидающий удаление
void RdkQTcpClient::AddIdCmdsRemovablePacket(int value)
{
 //if(WaitForSingleObject(Thread->PacketReaderUnlockEvent, INFINITE) != WAIT_TIMEOUT)*/
 if(Thread->PacketReaderUnlockEvent->exclusive_lock())
 {
  //ResetEvent(Thread->PacketReaderUnlockEvent);
  IdCmdsRemovablePackets.push_back(value);
  //ReleaseMutex(Thread->PacketReaderUnlockEvent);
  Thread->PacketReaderUnlockEvent->exclusive_unlock();
 }
}

/// Очищает список пакетов ожидающих удаление
void RdkQTcpClient::ClearIdCmdsRemovablePackets(void)
{
 //if(WaitForSingleObject(Thread->PacketReaderUnlockEvent, INFINITE) != WAIT_TIMEOUT)
 if(Thread->PacketReaderUnlockEvent->exclusive_lock())
 {
//  ResetEvent(Thread->PacketReaderUnlockEvent);
  IdCmdsRemovablePackets.clear();
  //ReleaseMutex(Thread->PacketReaderUnlockEvent);
  Thread->PacketReaderUnlockEvent->exclusive_unlock();
 }
}

/// Очистка списка пришедших пакетов от ответов сервера на асинхронные команды и команды с вышедшем таймаутом ожидания ответа
bool RdkQTcpClient::EraseUnresponcedPackets(void)
{
 //if(WaitForSingleObject(Thread->PacketReaderUnlockEvent, INFINITE) != WAIT_TIMEOUT)
 if(Thread->PacketReaderUnlockEvent->exclusive_lock())
 {
//  ResetEvent(Thread->PacketReaderUnlockEvent);


  if(IdCmdsRemovablePackets.size()==0)
  {
   //ReleaseMutex(Thread->PacketReaderUnlockEvent);
   Thread->PacketReaderUnlockEvent->exclusive_unlock();
   return true;
  }

  RDK::USerStorageXML xml;
  std::string PacketXml;
  std::list<RDK::UTransferPacket> &packetList=PacketReader.GetPacketList();
  std::list<RDK::UTransferPacket>::iterator it,kt;
  it = packetList.begin();
  while(it != packetList.end())
   {

    PacketXml.resize(it->GetParamSize(0));
    memcpy(&PacketXml[0],&(it->operator ()((0),0)), it->GetParamSize(0));
    xml.Load(PacketXml, "RpcResponse");
    int id=xml.ReadInteger("Id", 0);

    bool erased=false;
    for(size_t i=0;i<IdCmdsRemovablePackets.size();i++)
    {
     if(id  == IdCmdsRemovablePackets[i])
     {
      IdCmdsRemovablePackets.erase(IdCmdsRemovablePackets.begin()+i);
      erased=true;
      break;
     }
    }

    if(erased)
    {
     kt=it;
     ++kt;
     packetList.erase(it);
     it=kt;
    }
    else
     ++it;
   }
  //ReleaseMutex(Thread->PacketReaderUnlockEvent);
  Thread->PacketReaderUnlockEvent->exclusive_unlock();
 }
 else
  return false;

 return true;
}

// Подготовка запроса
int RdkQTcpClient::PrepareCommandXml(RDK::USerStorageXML &xml, const char* cmd, int channel)
{
 xml.Create("RpcRequest");
 xml.WriteString("Cmd",cmd);
 int id=LastPacketId;
 LastPacketId=LastPacketId+1;
 if(LastPacketId<0)
  LastPacketId=0;
 xml.WriteInteger("Id",id);
 xml.WriteInteger("Channel",channel);
 return id;
}

// Подготовка команды для камеры
int RdkQTcpClient::PreparePtzControlXml(RDK::USerStorageXML &xml, const std::string &cmd, const std::string &camera, int channel)
{
 xml.Create("RpcRequest");
 xml.WriteString("Cmd",cmd);
 int id=LastPacketId;
 LastPacketId=LastPacketId+1;
 if(LastPacketId<0)
  LastPacketId=0;
 xml.WriteInteger("Id",id);
 xml.WriteInteger("Channel",channel);
 xml.WriteString("Component",camera);
 return id;
}

// Отсылка запроса
int RdkQTcpClient::SendControlCommand(RDK::USerStorageXML &xml)
{
 //if(WaitForSingleObject(Thread->SendInProgress,10000) == WAIT_TIMEOUT)
 if(Thread->SendInProgress->wait(10000)==false)
  return RDK_RPC_SEND_UNLOCK_WAIT_TIMEOUT;
 //ResetEvent(Thread->SendInProgress);
 Thread->SendInProgress->reset();
 try
 {
  std::string request;
  xml.Save(request);
  RDK::UTransferPacket Packet;
  Packet.SetNumParams(1);
  Packet.SetParamSize(0,request.size());
  Packet.SetParam(0,request);
  RDK::UParamT buffer;
  Packet.Save(buffer);

  if(TcpSocket->isOpen() && TcpSocket->isValid() && !buffer.empty())
  {
   QDataStream out;
   out.setDevice(TcpSocket);
   out.setVersion(QDataStream::Qt_5_0);
   out.startTransaction();

   char* buf;
   buf = new char[buffer.size()];
   memcpy(buf,&buffer[0],buffer.size());

   out.writeBytes(buf, buffer.size());
   //TStream* dataOutputStream=new TMemoryStream();
   //dataOutputStream->WriteBuffer(&buffer[0], buffer.size()*sizeof(buffer[0]));
   //dataOutputStream->Position=0;
   //IdTCPClient->IOHandler->Write(dataOutputStream, dataOutputStream->Size);
   //dataOutputStream->Free();
   //SetEvent(Thread->CalcEnable);
   Thread->CalcEnable->set();
  }
 }
 catch(QException &exeption)
 {
  //SetEvent(Thread->SendInProgress);
  Thread->SendInProgress->set();
  //ReleaseMutex(Thread->CalculationNotInProgress);
  return RDK_RPC_SEND_TRANSPORT_EXCEPTION;
 }
 catch(...)
 {
  //SetEvent(Thread->SendInProgress);
  Thread->SendInProgress->set();
  //ReleaseMutex(Thread->CalculationNotInProgress);
  return RDK_RPC_SEND_TRANSPORT_UNHANDLED_EXCEPTION;
 }
 //SetEvent(Thread->SendInProgress);
 Thread->SendInProgress->set();
 //ReleaseMutex(Thread->CalculationNotInProgress);
 return 0;
}

// Поиск пакета по id комманды
int RdkQTcpClient::FindPacketById(int cmdId, RDK::USerStorageXML &xml, bool erase_found, std::list<RDK::UTransferPacket>::iterator &it)
{
 //if(WaitForSingleObject(Thread->PacketReaderUnlockEvent,INFINITE) != WAIT_TIMEOUT)
 if(Thread->PacketReaderUnlockEvent->exclusive_lock())
 {
//  ResetEvent(Thread->PacketReaderUnlockEvent);
  std::list<RDK::UTransferPacket> &packetList=PacketReader.GetPacketList();
  std::list<RDK::UTransferPacket>::iterator end;

  std::string PacketXml;
  for (it = packetList.begin(), end = packetList.end(); it != end; ++it)
  {
   PacketXml.resize(it->GetParamSize(0));
   memcpy(&PacketXml[0],&(it->operator ()((0),0)), it->GetParamSize(0));
   xml.Load(PacketXml, "RpcResponse");

   int id=xml.ReadInteger("Id", 0);
   if(id  == cmdId)
   {
    if(erase_found)
     packetList.erase(it);
    //ReleaseMutex(Thread->PacketReaderUnlockEvent);
    Thread->PacketReaderUnlockEvent->exclusive_lock();
    return 1;
   }
  }
  //ReleaseMutex(Thread->PacketReaderUnlockEvent);
  Thread->PacketReaderUnlockEvent->exclusive_unlock();
 }
 else
 {
  return RDK_RPC_PACKET_READER_ACCESS_TIMEOUT;
 }

 // пакет не найден
 return 0;
}


int RdkQTcpClient::FindPacketById(int cmdId, RDK::USerStorageXML &xml, std::vector<RDK::UParamT> &binary_data, bool erase_found, std::list<RDK::UTransferPacket>::iterator &it)
{
 //if(WaitForSingleObject(Thread->PacketReaderUnlockEvent,INFINITE) != WAIT_TIMEOUT)
 if(Thread->PacketReaderUnlockEvent->exclusive_lock())
 {
//  ResetEvent(Thread->PacketReaderUnlockEvent);
  std::list<RDK::UTransferPacket> &packetList=PacketReader.GetPacketList();
  std::list<RDK::UTransferPacket>::iterator end;

  std::string PacketXml;
  for (it = packetList.begin(), end = packetList.end(); it != end; ++it)
  {
   PacketXml.resize(it->GetParamSize(0));
   memcpy(&PacketXml[0],&(it->operator ()((0),0)), it->GetParamSize(0));
   xml.Load(PacketXml, "RpcResponse");

   if( xml.ReadInteger("Id", 0) == cmdId)
   {
    binary_data.resize(it->GetNumParams()-1);
    for(int i=0;i<binary_data.size();i++)
    {
     binary_data[i].resize(it->GetParamSize(i+1));
     binary_data[i]=(*it)(i+1);
    }

    if(erase_found)
     packetList.erase(it);
    //ReleaseMutex(Thread->PacketReaderUnlockEvent);
    Thread->PacketReaderUnlockEvent->exclusive_lock();
    return 1;
   }
  }
  //ReleaseMutex(Thread->PacketReaderUnlockEvent);
  Thread->PacketReaderUnlockEvent->exclusive_unlock();
 }
 else
 {
  return RDK_RPC_PACKET_READER_ACCESS_TIMEOUT;
 }

 // пакет не найден
 return 0;
}

// Удаляет пакет по его итератору
void RdkQTcpClient::ErasePacket(std::list<RDK::UTransferPacket>::iterator &it)
{
 //if(WaitForSingleObject(Thread->PacketReaderUnlockEvent,INFINITE) != WAIT_TIMEOUT)
 if(Thread->PacketReaderUnlockEvent->exclusive_lock())
 {
  //ResetEvent(Thread->PacketReaderUnlockEvent);
  std::list<RDK::UTransferPacket> &packetList=PacketReader.GetPacketList();
  packetList.erase(it);
  //ReleaseMutex(Thread->PacketReaderUnlockEvent);
  Thread->PacketReaderUnlockEvent->exclusive_unlock();
 }
}


//---------------------------------------------------------------------------
// Ожидание ответа сервера
int RdkQTcpClient::WaitServerResponse(int cmdId, RDK::USerStorageXML &response, int timeout)
{
 std::list<RDK::UTransferPacket>::iterator it;
 QElapsedTimer e_timer;
 e_timer.start();
 while(e_timer.elapsed()<timeout)
 {
  //DWORD curr_ticks=GetTickCount();
  //if(timeout<(curr_ticks-start_waiting))
//  {
//   break;
//  }
  if(timeout<e_timer.elapsed())
      break;

  int diff=timeout-e_timer.elapsed();
  //if(WaitForSingleObject(Thread->PacketReceivedEvent, diff) != WAIT_TIMEOUT)
  if(Thread->PacketReceivedEvent->wait(diff)!=false)
  {
   //ResetEvent(Thread->PacketReceivedEvent);
   Thread->PacketReceivedEvent->reset();
   if(Thread->ThreadError != 0)
    return Thread->ThreadError;
   int res=FindPacketById(cmdId, response, true, it);
   if(res == 0)
   {
    continue;
   }
   return res;
  }
 }
 return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
/*
 std::list<RDK::UTransferPacket>::iterator it;
 if(WaitForSingleObject(Thread->PacketReceivedEvent, timeout) != WAIT_TIMEOUT)
 {
  ResetEvent(Thread->PacketReceivedEvent);
  if(Thread->ThreadError != 0)
   return Thread->ThreadError;
  int res=FindPacketById(cmdId, response, true, it);
  return res;
 }
 else
 {
  return RDK_RPC_RESPONSE_RECEIVE_TIMEOUT;
 }
        */
}

//---------------------------------------------------------------------------
// Разбиение строки по символу sep
int RdkQTcpClient::SeparateString(const std::string &str, std::vector<std::string> &output, char sep, int num, int *lastpos)
{
 std::string::size_type i=0,j=0;
 int size=0;
 int nnum=(num>0)?num-1:0;

 if(lastpos) *lastpos=0;
 output.resize(0);
 if(str.empty())
  return 0;

 while(i != std::string::npos && (nnum>=0) )
 {
  i=str.find_first_of(sep,j);
  if(i == j)
  {
   j++;
   continue;
  }

  ++size;
  output.resize(size);
  if(num)
   nnum--;
  if(i == std::string::npos)
   output[size-1]=str.substr(j);
  else
   output[size-1]=str.substr(j,i-j);
  j=i+1;
  if(j >= str.size())
   break;
 }

 if(lastpos) *lastpos=i;

 return size;
}

//-----------------
//Добавления в интерфейс класса
bool RdkQTcpClient::SocketIsConnected()
{
 if(TcpSocket==nullptr)
 {
     return false;
 }
 bool val = TcpSocket->isValid();
 bool op = TcpSocket->isOpen();
 return TcpSocket->isValid()&&TcpSocket->isOpen();
}


int RdkQTcpClient::Connect(const char* server_addr, int server_port)
{
    TcpSocket->connectToHost(QHostAddress(QString(server_addr)), server_port);
    return 0;
}

int RdkQTcpClient::Disconnect()
{
    TcpSocket->close();
    return 0;
}


std::string RdkQTcpClient::GetHost()
{
    if(TcpSocket==nullptr)
        return std::string("");
    QHostAddress addr = TcpSocket->peerAddress();
    if(addr!=QHostAddress::Null)
    {
        QString host = addr.toString();
        return std::string(host.toUtf8().constData());
    }
    return std::string("");
}
int RdkQTcpClient::GetPort()
{
    return TcpSocket->peerPort();
}
