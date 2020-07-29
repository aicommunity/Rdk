#ifndef RDKQTCPCLIENT_H
#define RDKQTCPCLIENT_H

#include <QtCore>
#include <QtNetwork>
#include <QThread>

#include "RdkQTcpClientLib.h"

#include "../../Core/Serialize/USerStorageXML.h"
#include "../../../Rdk/Core/System/UGenericMutex.h"
#include "../../../Rdk/Core/Engine/UELockVar.h"
#include "../../../Rdk/Core/Utilities/UTransferPacket.h"
#include "../../Deploy/Include/myrdk.h"

class RdkQTcpThread: public QThread
{
Q_OBJECT
public: //Конструкторы и деструкторы
    RdkQTcpThread();
    RdkQTcpThread(QTcpSocket *socket, RDK::UTransferReader &packetReader, bool CreateSUspended, QObject *parent);
    ~RdkQTcpThread();

public: //События
    UGenericEvent *CalcEnable;

    UGenericEvent *CalcStarted;

    UGenericEvent *CalculationNotInProgress;

    UGenericEvent *PacketReceivedEvent;

    UGenericMutex *PacketReaderUnlockEvent;

    UGenericEvent *CalculationInProgress;

    UGenericEvent *SendInProgress;

    /// Выставляется только в треде получения данных
    RDK::UELockVar<int> ThreadError;

    RDK::UTransferReader *PacketReader;

public://Методы
    void run() override;

    // --------------------------
    // Управление потоком
    // --------------------------
    void BeforeCalculate(void);

    void AfterCalculate(void);

    void SetSocket(QTcpSocket *socket) {_socket=socket;};



protected:
    bool Terminated;
    //int _socketDescriptor;

    QTcpSocket *_socket;
};

class RdkQTcpClient: public QObject
{
Q_OBJECT
public:
    RdkQTcpClient();
    ~RdkQTcpClient();

    /// Добавляет в список пакет ожидающий удаление
    void AddIdCmdsRemovablePacket(int value);

    /// Очищает список пакетов ожидающих удаление
    void ClearIdCmdsRemovablePackets(void);

    /// Очистка списка пришедших пакетов от ответов сервера на асинхронные команды и команды с вышедшем таймаутом ожидания ответа
    bool EraseUnresponcedPackets(void);

    // Подготовка запроса
    int PrepareCommandXml(RDK::USerStorageXML &xml, const char* cmd, int channel);

    // Подготовка команды для камеры
    int PreparePtzControlXml(RDK::USerStorageXML &xml, const std::string &cmd, const std::string &camera, int channel);

    // Отсылка запроса
    // Возвращает 0 в случае успеха
    // RDK_RPC_SEND_TRASPORT_EXCEPTION - в случае возникновения исключения при отправке
    int SendControlCommand(RDK::USerStorageXML &xml);

    // Поиск пакета по id комманды
    // Возвращает true в случае успеха
    int FindPacketById(int cmdId, RDK::USerStorageXML &xml, bool erase_found, std::list<RDK::UTransferPacket>::iterator &it);
    int FindPacketById(int cmdId, RDK::USerStorageXML &xml, std::vector<RDK::UParamT> &binary_data, bool erase_found, std::list<RDK::UTransferPacket>::iterator &it);

    // Удаляет пакет по его итератору
    void ErasePacket(std::list<RDK::UTransferPacket>::iterator &it);

    // Ожидание ответа сервера
    int WaitServerResponse(int cmdId, RDK::USerStorageXML &response, int timeout);
    int WaitServerResponse(int cmdId, RDK::USerStorageXML &response, std::vector<RDK::UParamT> &binary_data, int timeout);

    // Разбиение строки по символу sep
    int SeparateString(const std::string &str, std::vector<std::string> &output, char sep, int num=0, int *lastpos=0);

    //-----------------
    //Добавления в интерфейс класса
    bool SocketIsConnected();

    int Connect(const char* server_addr, int server_port);

    int Disconnect();

    std::string GetHost();
    int GetPort();
public:
    /// Последний переданный id пакета
    RDK::UELockVar<int> LastPacketId;

    /// Поток обработки ответов
    RdkQTcpThread* Thread;

    RDK::UTransferReader PacketReader;

private slots:
    void EraseTimerTick();


private:
    QTcpSocket* TcpSocket;
    QTimer * EraseTimer;
    std::vector<int> IdCmdsRemovablePackets;
};

#endif // RdkQTcpClient_H
