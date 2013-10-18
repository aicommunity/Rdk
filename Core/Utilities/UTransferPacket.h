#ifndef UTRANSFERPACKET_H
#define UTRANSFERPACKET_H

#include <vector>

#define MAX_PACKET_SIZE 1524000
#define MAX_NUM_PARAMS 20
#define MAX_PARAM_SIZE 1024000

namespace RDK {

// ������������������ ���������������� ������ ������
extern unsigned char UPacketPrefix[16];

typedef std::vector<unsigned char> UParamT;
typedef std::vector<unsigned char>::iterator ParamIterator;
typedef std::vector<unsigned char>::const_iterator ParamCIterator;

class UTransferPacket
       {
       protected: // ������
        // ����� ������
        int PacketSize;

        // ���������� ����� �������
        int CmdId;

        // ����� ���������� �������
        int NumParams;

        // ������ ����������
        std::vector<UParamT> Params;

        // ����������� �����
        unsigned int Checksum;
        
        int Temp;

       public: // ������
        // -------------------------
        // ����������� � �����������
        // -------------------------
        UTransferPacket(void);
        UTransferPacket(const UTransferPacket &packet);
        UTransferPacket(const UParamT &buffer);
        virtual ~UTransferPacket(void);
        // -------------------------

        // -------------------------
        // ������ ������� � ������
        // -------------------------
        // ���������� ����� �������
        inline int GetCmdId(void) const
         { return CmdId; };

        // ���������� ����� ���������� �������
        inline int GetNumParams(void) const
         { return NumParams; };

        // ���������� ������ ������ ���������
        inline int GetParamSize(int i) const
         { return Params[i].size(); };

        // ���������� ����������� �����
        inline unsigned int GetChecksum(void) const
         { return Checksum; };
        // -------------------------

        // -------------------------
        // ������ ���������� �������
        // -------------------------
        // ������������� ����� �������
        bool SetCmdId(int cmdid);

        // ������������� ����� ���������� �������
        bool SetNumParams(int numparams);

        // ������������� ������ ������ ���������
        bool SetParamSize(int i, int size);

        // ������������� �������� i-�� ���������
        // ������ ������� ������ ������ ��������� �� ��������� ��������
        bool SetParam(int i, const UParamT &value, int istart=0, int istop=-1);

		// ������������� �������� i-�� ���������
		// ������ ������� ������ ������ ��������� �� ��������� ��������
		bool SetParam(int i, const std::string &value);
		bool SetParam(int i, const std::vector<char> &value);

		// ������������� �������� i-�� ���������
        // ��������� ����� 'value' � ���������� ������ �� ��������� 
        // �� ��������� ������ ��������� ���� ������
        // ��� ���������� -1 � ������ ������
        int DecodeParam(int i, const UParamT &value, int istart=0);

        // ������������� ����������� ����� � ���������� ��
        unsigned int CalcChecksum(void);

        // ��������� �������� int �� ������� �������
        unsigned int IntCompose(const UParamT &value, int istart);

        // ��������� ������� ������� �� �������� int
        // � ���������� 
        const UParamT& IntDivide(unsigned int value, UParamT &buffer, int istart) const;
        // -------------------------

        // -------------------------
        // ������ �������� ������
        // -------------------------
        // ����������� ���������� ����� � ����������
        // + ����� ���� ���������� ��� ������������� ������
        // - ����� ���� ������ � ������
        // 0 - ���� ������������� ��������
        int CheckBuffer(const UParamT &buffer, int buffersize);
		int CheckBuffer(const UParamT &buffer, int buffersize, int start_index);

		// ���� � ������ ������ ������ � ���������� ������ ������
		// ��� -1 ���� ����� �� ������
		int FindPacketInBuffer(const UParamT &buffer, int start_index);

        // ��������� ��� ������ ������ �� ������� 'buffer'
        bool Load(const UParamT &buffer, int start_index=0);

        // ��������� ��� ������ ������ � ������ 'buffer'
        // ������ ������ ���� ��������
        bool Save(UParamT &buffer);

        // ���������� ����� ������ � ������
        int GetPacketSize(void) const;
        // -------------------------

        // -------------------------
        // ������������� ���������
        // -------------------------
        // �������� ������������
        UTransferPacket& operator = (const UTransferPacket &packet);

        // ���������� ������ i-�� ���������
        inline UParamT& operator () (int i)
         { return Params[i]; };

        // ���������� j-� ���� ������ i-�� ���������
        inline unsigned char& operator () (int i, int j)
         { return Params[i][j]; };

        // ���������� ������ i-�� ��������� � ���� ������� 
        UParamT& operator () (int i, UParamT &buffer, int istart);
        // -------------------------
	   };

/// �����, �������������� ���������� ������������������ ������� �� ������
class UTransferReader
{
protected: // ������
/// ������ ������
std::list<UTransferPacket> PacketList;

protected: // ��������� ����������
/// ������� ����������� �����
UTransferPacket Packet;

/// ����, ������������ ����� ���� ���������� ���������� ������
bool PacketInProgress;

int LastSize;
int ReadIndex;

RDK::UParamT ClientBuffer;
int ClientError;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UTransferReader(void);
virtual ~UTransferReader(void);
// --------------------------

// --------------------------
// ������ ���������� ������� ������
// --------------------------
/// ������� ������
void ClearPacketList(void);

/// ���������� ����� ������ �����
const UTransferPacket& GetFirstPacket(void) const;

/// ������� ����� ������ �����
void DelFirstPacket(void);

/// ���������� ����� ����� �����
const UTransferPacket& GetLastPacket(void) const;

/// ������� ����� ����� �����
void DelLastPacket(void);

/// ���������� ������ �� ������ �������
const std::list<UTransferPacket>& GetPacketList(void) const;

/// ���������� ����� ������� � �������
int GetNumPackets(void) const;
// --------------------------

// --------------------------
// ������ ��������� ������
// --------------------------
// ������������ ��������� ������ ������
int ProcessDataPart(const UParamT &buffer);

/// ��������� ������ ��������� ���� ��� ����
void ResetProcessing(void);
// --------------------------


};

}
#endif

