/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UControllerH
#define UControllerH

#include <vector>
#include <map>
#include <typeinfo>
#include <list>
#include "UModule.h"
#include <memory>
#include "../Math/MDMatrix.h"

namespace RDK {

class UContainer;
class UIProperty;

using namespace std;

// UController is always managed via shared_ptr (stored in UContainer::Controllers)
// Add enable_shared_from_this to allow safe access to shared_ptr
class RDK_LIB_TYPE UController: public RDK::UModule,
                                public std::enable_shared_from_this<UController>
{
protected: // ���������
// ���� ���������� ���������� ������ �����������
bool Enabled;

protected: // ������
// ��������� �� ����������
UContainer* Component;

public:
// --------------------------
// ������������ � �����������
// --------------------------
UController(void);
virtual ~UController(void);
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
// ���� ���������� ���������� ������ �����������
bool IsEnabled(void) const;
bool IsEnabled(bool value);

// ���������� ��� ����������
std::string GetComponentName(void) const;
std::string GetComponentName(UContainer *level) const;
// --------------------------

// --------------------------
// ������ ���������� �����������
// --------------------------
public:
// ��������� ��������� � �����������
virtual bool Link(UContainer* component, bool forchilds=false);

// ���������� ��������� �� ����������
virtual bool UnLink(bool forchilds=false);

// ��������� ���������
virtual bool Update(void);
// --------------------------

// --------------------------
// ������� ������ ���������� �����������
// --------------------------
protected:
// ��������� ��������� � �����������
virtual bool ALink(UContainer* component, bool forchilds=false);

// ���������� ��������� �� ����������
virtual bool AUnLink(bool forchilds=false);

// ��������� ���������
virtual bool AUpdate(void)=0;
// --------------------------
};

/// �������� ����� ����� ������
struct UControllerData: public UController
{
/// ��������-�������� ������
std::shared_ptr<UIProperty> Property;
/// ��� ��������
//std::string PropertyName;

/// ���������� ��������� �������� ����� ������
int NumPoints;

/// �����������
UControllerData(void);

/// ������ ������ �������� ������
void SetNumPoints(int value);

/// ���������� ��� ��������
std::string GetPropertyName(void) const;

/// ��������� �������� ������ �������� �����
virtual bool AUpdate(void)=0;

/// ������� ��� �������� ������
virtual void Clear(void)=0;

/// ����������� ����� ����� ������
virtual bool Configure(UContainer *container, std::shared_ptr<UIProperty> property)=0;

/// ���������� ��� ������
virtual const type_info& GetDataType(void) const;
};

struct UControllerDataReader: public UControllerData
{
/// ��������� �������� �� ������� �������� ������ (�������)
double TimeInterval;

/// ��� ������ ��������
/// 0 - �� ����������
/// 1 - ������� �������� ���� double
/// 2 - ������� �������� ���� int
/// 3 - �������� ���� MDMatrix<double>
/// 4 - �������� ���� MDMatrix<int>
/// 5 - �������� ���� MDVector<double>
/// 6 - �������� ���� MDVector<int>
int PropertyType;

/// ���������� ������� �� ������� ������� ������
int MRow, MCol;

/// ������
std::list<double> XData;
std::list<double> YData;

public:
/// �����������
UControllerDataReader(void);

/// ������ ����� ��������� � ��������
virtual void SetTimeInterval(double value);

/// ���������� ������� �� ������� ������� ������
virtual void SetMatrixCoord(int row, int col);

// ���������� ��������� �� ����������
virtual bool AUnLink(bool forchilds=false);

/// ��������� �������� ������ �������� �����
virtual bool AUpdate(void);

/// ������� ��� �������� ������
virtual void Clear(void);

/// ����������� ����� ����� ������
virtual bool Configure(UContainer *container, std::shared_ptr<UIProperty> property);

protected:
/// ������ ������ �������� ������
void SetNumPoints(int value);
};

/// �����, �������������� ������, ������� �������� � ������� ����
/// MDMatrix<double>, � ������� ������.
/// ������ ��������: ������� ������� � ������� ����������� �������
/// ����� ������� ����� ���� ������������
struct UControllerDataReaderTimeEvents: public UControllerDataReader
{
public:
UControllerDataReaderTimeEvents(void);

protected:
/// ���������� ������� �� ������� ������� ������
virtual void SetMatrixCoord(int row, int col);

/// ��������� �������� ������ �������� �����
virtual bool AUpdate(void);
};
              /*

template <class T>
struct UDataSourceT: public UControllerData
{
/// ������
std::map<double, MDMatrix<T> > Data;

UDataSourceT(void);

/// ��������� �������� ������ �������� �����
virtual bool AUpdate(void);

/// ��������� ������ � ������� ������
virtual void ExtractData(std::map<double, MDMatrix<T> > &buffer);

/// ������� ��� �������� ������
virtual void Clear(void);

/// ����������� ����� ����� ������
virtual bool Configure(UContainer *container, const std::string &property_name);

/// ���������� ��� ������
virtual const type_info& GetDataType(void) const;
};

template<class T>
UDataSourceT<T>::UDataSourceT(void)
 : UControllerData()
{
}

/// ��������� �������� ������ �������� �����
template<class T>
bool UDataSourceT<T>::AUpdate(void)
{
 const MDMatrix<T> *data=Component->AccessPropertyData<MDMatrix<T> >(PropertyName);
 if(!data)
 {
  return false;
 }

 if(LastReadTime>=LastWriteTime)
 {
  Data.clear();
  LastReadTime=0.0;
 }


 Data[Component->GetTime().GetDoubleTime()]=*data;
 LastWriteTime=Component->GetTime().GetDoubleTime();
 return true;
};

/// ��������� ������ � ������� ������
template<class T>
void UDataSourceT<T>::ExtractData(std::map<double, MDMatrix<T> > &buffer)
{
 buffer=Data;
 LastReadTime=Component->GetTime().GetDoubleTime();
}

/// ������� ��� �������� ������
template<class T>
void UDataSourceT<T>::Clear(void)
{
 Data.clear();
 LastReadTime=LastWriteTime=0.0;
};

/// ����������� ����� ����� ������
template<class T>
bool UDataSourceT<T>::Configure(UContainer *container, const std::string &property_name)
{
 if(PropertyName == property_name && Component == container)
  return true;

 UnLink();
 Clear();

 if(!Link(container))
  return false;

 PropertyName.clear();

 std::shared_ptr<UIProperty> prop=Component->FindProperty(property_name);
 if(!prop)
 {
  UnLink();
  return false;
 }

 if(prop->GetLanguageType() != typeid(MDMatrix<T>))
 {
  UnLink();
  return false;
 }

 PropertyName=property_name;
 Enabled=true;
 return true;
};

/// ���������� ��� ������
template<class T>
const type_info& UDataSourceT<T>::GetDataType(void) const
{
 return typeid(MDMatrix<T>);
}

                  */
}

#endif
