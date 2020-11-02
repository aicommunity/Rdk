#ifndef UServerTransportH
#define UServerTransportH

#include "../../Deploy/Include/rdk.h"
#include "UIVisualController.h"
#include "URpcCommandInternal.h"

namespace RDK {
class UApplication;

class RDK_LIB_TYPE UServerTransport: public UAppController
{
friend class UApplication;
protected: // Параметры

protected: // Данные
/// Указатель на экземпляр приложения
UEPtr<UApplication> Application;

/// Карта читалок пакетов, по числу источников удаленного приема
std::map<std::string, RDK::UTransferReader> PacketReaders;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UServerTransport(void);
virtual ~UServerTransport(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
/// Возвращает указатель на экземпляр приложения
UEPtr<UApplication> GetApplication(void);
bool SetApplication(UEPtr<UApplication> value);
// --------------------------

// --------------------------
// Методы транспортировки данных
// --------------------------

//Задает адрес и порт входящего интерфейса сервера
virtual void SetServerBinding(std::string &interface_address, int port);

//Получение адреса интерфейса управления сервером
virtual std::string GetServerBindingInterfaceAddress(void);

//Получение адреса интерфейса управления сервером
virtual int GetServerBindingPort(void) const;

virtual int GetSocketState(std::string bind);

virtual bool ServerIsActive();

/// Обрабатывает входящие данные, преобразовывая пакеты в сообщения
/// 01.04.2020 пока в этом классе, возможно уедет потом в дочерние *Tcp, *Http, *Udp
void ProcessIncomingData(std::string &bind, std::vector<URpcCommandInternal> &command_list);

/// Читает входящие байты из выбранного источника, контекст привязки
/// всегда определяется строкой вне зависимости от типа транспорта
virtual int ReadIncomingBytes(std::string &bind, std::vector<unsigned char> &bytes);

/// Отправить ответ на команду соответствующему получателю
virtual void SendResponseBuffer(std::vector<unsigned char> buffer, std::string &responce_addr);

///Подсоединяет клиента для чтения данных
///Фактически - создает объект для обработки пакетов от конкретного клиента
virtual void ClientConnect(std::string& bind);
///Отсоединяет клиента для чтения данных
///Фактически - очищает объект для обработки пакетов от конкретного клиента
virtual void ClientDisconnect(std::string& bind);
///Инициировать остановку сервера, отключить все приемники
virtual void ServerStop();
/// По аналогии с ServerStop, инициировать включение сервера
/// (переопределять в конечных классах)
virtual void ServerStart();

/// Кодирует строку в вектор
void ConvertStringToVector(const std::string &source, UParamT &dest);
void ConvertVectorToString(const UParamT &source, std::string &dest);

// --------------------------
// Общие методы управления контроллером
// --------------------------
// Сохраняет параметры интерфейса в xml
//virtual void SaveParameters(RDK::USerStorageXML &xml);

// Загружает параметры интерфейса из xml
//virtual void LoadParameters(RDK::USerStorageXML &xml);
// --------------------------

};

}//namespace RDK
#endif
