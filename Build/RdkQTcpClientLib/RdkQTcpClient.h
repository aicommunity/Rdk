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
public: //������������ � �����������
    RdkQTcpThread();
    RdkQTcpThread(QTcpSocket *socket, RDK::UTransferReader &packetReader, bool CreateSUspended, QObject *parent);
    ~RdkQTcpThread();

public: //�������
    UGenericEvent *CalcEnable;

    UGenericEvent *CalcStarted;

    UGenericEvent *CalculationNotInProgress;

    UGenericEvent *PacketReceivedEvent;

    UGenericMutex *PacketReaderUnlockEvent;

    UGenericEvent *CalculationInProgress;

    UGenericEvent *SendInProgress;

    /// ������������ ������ � ����� ��������� ������
    RDK::UELockVar<int> ThreadError;

    RDK::UTransferReader *PacketReader;

public://������
    void run() override;

    // --------------------------
    // ���������� �������
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

    /// ��������� � ������ ����� ��������� ��������
    void AddIdCmdsRemovablePacket(int value);

    /// ������� ������ ������� ��������� ��������
    void ClearIdCmdsRemovablePackets(void);

    /// ������� ������ ��������� ������� �� ������� ������� �� ����������� ������� � ������� � �������� ��������� �������� ������
    bool EraseUnresponcedPackets(void);

    // ���������� �������
    int PrepareCommandXml(RDK::USerStorageXML &xml, const char* cmd, int channel);

    // ���������� ������� ��� ������
    int PreparePtzControlXml(RDK::USerStorageXML &xml, const std::string &cmd, const std::string &camera, int channel);

    // ������� �������
    // ���������� 0 � ������ ������
    // RDK_RPC_SEND_TRASPORT_EXCEPTION - � ������ ������������� ���������� ��� ��������
    int SendControlCommand(RDK::USerStorageXML &xml);

    // ����� ������ �� id ��������
    // ���������� true � ������ ������
    int FindPacketById(int cmdId, RDK::USerStorageXML &xml, bool erase_found, std::list<RDK::UTransferPacket>::iterator &it);
    int FindPacketById(int cmdId, RDK::USerStorageXML &xml, std::vector<RDK::UParamT> &binary_data, bool erase_found, std::list<RDK::UTransferPacket>::iterator &it);

    // ������� ����� �� ��� ���������
    void ErasePacket(std::list<RDK::UTransferPacket>::iterator &it);

    // �������� ������ �������
    int WaitServerResponse(int cmdId, RDK::USerStorageXML &response, int timeout);
    int WaitServerResponse(int cmdId, RDK::USerStorageXML &response, std::vector<RDK::UParamT> &binary_data, int timeout);

    // ��������� ������ �� ������� sep
    int SeparateString(const std::string &str, std::vector<std::string> &output, char sep, int num=0, int *lastpos=0);

    //-----------------
    //���������� � ��������� ������
    bool SocketIsConnected();

    int Connect(const char* server_addr, int server_port);

    int Disconnect();

    std::string GetHost();
    int GetPort();
public:
    /// ��������� ���������� id ������
    RDK::UELockVar<int> LastPacketId;

    /// ����� ��������� �������
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
