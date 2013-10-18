#ifndef UTRANSFERPACKET_H
#define UTRANSFERPACKET_H

#include <vector>

#define MAX_PACKET_SIZE 1524000
#define MAX_NUM_PARAMS 20
#define MAX_PARAM_SIZE 1024000

namespace RDK {

// Последовательность идентифицирующая начало пакета
extern unsigned char UPacketPrefix[16];

typedef std::vector<unsigned char> UParamT;
typedef std::vector<unsigned char>::iterator ParamIterator;
typedef std::vector<unsigned char>::const_iterator ParamCIterator;

class UTransferPacket
       {
       protected: // Данные
        // Длина пакета
        int PacketSize;

        // Уникальный номер команды
        int CmdId;

        // Число параметров команды
        int NumParams;

        // Список параметров
        std::vector<UParamT> Params;

        // Контрольная сумма
        unsigned int Checksum;
        
        int Temp;

       public: // Методы
        // -------------------------
        // Конструторы и деструкторы
        // -------------------------
        UTransferPacket(void);
        UTransferPacket(const UTransferPacket &packet);
        UTransferPacket(const UParamT &buffer);
        virtual ~UTransferPacket(void);
        // -------------------------

        // -------------------------
        // Методы доступа к данным
        // -------------------------
        // Возвращает номер команды
        inline int GetCmdId(void) const
         { return CmdId; };

        // Возвращает число параметров команды
        inline int GetNumParams(void) const
         { return NumParams; };

        // Возвращает размер буфера параметра
        inline int GetParamSize(int i) const
         { return Params[i].size(); };

        // Возвращает контрольную сумму
        inline unsigned int GetChecksum(void) const
         { return Checksum; };
        // -------------------------

        // -------------------------
        // Методы управления данными
        // -------------------------
        // Устанавливает номер команды
        bool SetCmdId(int cmdid);

        // Устанавливает число параметров команды
        bool SetNumParams(int numparams);

        // Устанавливает размер буфера параметра
        bool SetParamSize(int i, int size);

        // Устанавливает значение i-го параметра
        // Меняет текущий размер буфера параметра на требуемое значение
        bool SetParam(int i, const UParamT &value, int istart=0, int istop=-1);

		// Устанавливает значение i-го параметра
		// Меняет текущий размер буфера параметра на требуемое значение
		bool SetParam(int i, const std::string &value);
		bool SetParam(int i, const std::vector<char> &value);

		// Устанавливает значение i-го параметра
        // декодируя буфер 'value' и возвращает индекс на следующий 
        // за последним байтом параметра байт буфера
        // или возвращает -1 в случае ошибки
        int DecodeParam(int i, const UParamT &value, int istart=0);

        // Пересчитывает контрольную сумму и возвращает ее
        unsigned int CalcChecksum(void);

        // Вычисляет значение int по участку вектора
        unsigned int IntCompose(const UParamT &value, int istart);

        // Вычисляет участок вектора по значению int
        // и возвращает 
        const UParamT& IntDivide(unsigned int value, UParamT &buffer, int istart) const;
        // -------------------------

        // -------------------------
        // Методы загрузки пакета
        // -------------------------
        // Анализирует полученный буфер и возвращает
        // + число байт недотающих для декодирования пакета
        // - число байт лишних в буфере
        // 0 - если декодирование возможно
        int CheckBuffer(const UParamT &buffer, int buffersize);
		int CheckBuffer(const UParamT &buffer, int buffersize, int start_index);

		// Ищет в буфере начало пакета и возвращает индекс начала
		// или -1 если пакет не найден
		int FindPacketInBuffer(const UParamT &buffer, int start_index);

        // Загружает все данные пакета из массива 'buffer'
        bool Load(const UParamT &buffer, int start_index=0);

        // Сохраняет все данные пакета в массив 'buffer'
        // Память должна быть выделена
        bool Save(UParamT &buffer);

        // Возвращает длину пакета в байтах
        int GetPacketSize(void) const;
        // -------------------------

        // -------------------------
        // Перегруженные операторы
        // -------------------------
        // Оператор присваивания
        UTransferPacket& operator = (const UTransferPacket &packet);

        // Возвращает данные i-го параметра
        inline UParamT& operator () (int i)
         { return Params[i]; };

        // Возвращает j-й байт данных i-го параметра
        inline unsigned char& operator () (int i, int j)
         { return Params[i][j]; };

        // Возвращает данные i-го параметра в виде массива 
        UParamT& operator () (int i, UParamT &buffer, int istart);
        // -------------------------
	   };

/// Класс, обеспечивающий считывание последовательности пакетов из потока
class UTransferReader
{
protected: // Данные
/// Список команд
std::list<UTransferPacket> PacketList;

protected: // Временные переменные
/// Текущий считываемый пакет
UTransferPacket Packet;

/// Флаг, выставялемый когда идет дозагрузка найденного пакета
bool PacketInProgress;

int LastSize;
int ReadIndex;

RDK::UParamT ClientBuffer;
int ClientError;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UTransferReader(void);
virtual ~UTransferReader(void);
// --------------------------

// --------------------------
// Методы управления списком команд
// --------------------------
/// Очищает список
void ClearPacketList(void);

/// Возвращает самый старый пакет
const UTransferPacket& GetFirstPacket(void) const;

/// Удаляет самый старый пакет
void DelFirstPacket(void);

/// Возвращает самый новый пакет
const UTransferPacket& GetLastPacket(void) const;

/// Удаляет самый новый пакет
void DelLastPacket(void);

/// Возвращает ссылку на список пакетов
const std::list<UTransferPacket>& GetPacketList(void) const;

/// Возвращает число пакетов в очереди
int GetNumPackets(void) const;
// --------------------------

// --------------------------
// Методы обработки потока
// --------------------------
// Обрабатывает очередную порцию данных
int ProcessDataPart(const UParamT &buffer);

/// Прерывает текущю обработку если она была
void ResetProcessing(void);
// --------------------------


};

}
#endif

