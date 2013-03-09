#ifndef TUVisualControllerH
#define TUVisualControllerH

#include <vcl.h>
#include <map>
#include <string>
#include "TUVisualControllerFrameUnit.h"
#include "TUVisualControllerFormUnit.h"
	   /*
class TUFormsSerialize
{
protected: // Данные
// Хранилище настроек форм
RDK::USerStorageXML* Xml;

// Список <имен разделов в xml,форм для сохранения/загрузки>
std::map<std::string, TForm* > Forms;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
TUFormsSerialize(void);
~TUFormsSerialize(void);
// --------------------------

// --------------------------
// Методы управления данными
// --------------------------
// Управление хранилищем форм
RDK::USerStorageXML* GetXml(void);
void SetXml(RDK::USerStorageXML *xml);

// Добавляет форму в список сериализуемых
void AddForm(const std::string &name, TForm *form);

// Удаляет форму из списка сериализуемых
void DelForm(TForm *form);
void DelForm(const std::string &name);

// Удаляет все формы из списка сериализуемых
void ClearForms(void);
// --------------------------

// --------------------------
// Методы сериализации
// --------------------------
// Сохраняет все данные форм в xml
void Save(void);

// Загружает все данные форм из xml
void Load(void);

// Сохраняет xml в файл
void SaveXmlToFile(const String &filename);

// Загружает xml из файла
void LoadXmlFromFile(const String &filename);
// --------------------------

// --------------------------
// Вспомогательные методы сериализации
// --------------------------
protected:
// Сохраняет данные формы в xml
void SaveForm(RDK::USerStorageXML &xml, TForm *form);

// Загружает данные формы из xml
void LoadForm(RDK::USerStorageXML &xml, TForm *form);
// --------------------------
};           */

#endif
