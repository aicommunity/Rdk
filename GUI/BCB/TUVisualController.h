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
protected: // ������
// ��������� �������� ����
RDK::USerStorageXML* Xml;

// ������ <���� �������� � xml,���� ��� ����������/��������>
std::map<std::string, TForm* > Forms;

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
TUFormsSerialize(void);
~TUFormsSerialize(void);
// --------------------------

// --------------------------
// ������ ���������� �������
// --------------------------
// ���������� ���������� ����
RDK::USerStorageXML* GetXml(void);
void SetXml(RDK::USerStorageXML *xml);

// ��������� ����� � ������ �������������
void AddForm(const std::string &name, TForm *form);

// ������� ����� �� ������ �������������
void DelForm(TForm *form);
void DelForm(const std::string &name);

// ������� ��� ����� �� ������ �������������
void ClearForms(void);
// --------------------------

// --------------------------
// ������ ������������
// --------------------------
// ��������� ��� ������ ���� � xml
void Save(void);

// ��������� ��� ������ ���� �� xml
void Load(void);

// ��������� xml � ����
void SaveXmlToFile(const String &filename);

// ��������� xml �� �����
void LoadXmlFromFile(const String &filename);
// --------------------------

// --------------------------
// ��������������� ������ ������������
// --------------------------
protected:
// ��������� ������ ����� � xml
void SaveForm(RDK::USerStorageXML &xml, TForm *form);

// ��������� ������ ����� �� xml
void LoadForm(RDK::USerStorageXML &xml, TForm *form);
// --------------------------
};           */

#endif
