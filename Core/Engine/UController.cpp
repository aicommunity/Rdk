/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2010.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UCONTROLLER_CPP
#define UCONTROLLER_CPP

#include <algorithm>
#include "UController.h"
#include "UContainer.h"

namespace RDK {

// --------------------------
// Конструкторы и деструкторы
// --------------------------
UController::UController(void)
 : Component(0)
{
 Enabled=false;
}

UController::~UController(void)
{
 UnLink();
}
// --------------------------

// --------------------------
// Методы управления параметрами
// --------------------------
// Флаг разрешения обновления данных контроллера
bool UController::IsEnabled(void) const
{
 return Enabled;
}

bool UController::IsEnabled(bool value)
{
 if(Enabled == value)
  return true;

 Enabled=value;
 return true;
}

// Возвращает имя компонента
std::string UController::GetComponentName(void) const
{
 std::string result;
 if(Component)
 {
  result=Component->GetFullName();
  result=result.substr(result.find_first_of(".")+1);
 }
 return result;
}

std::string UController::GetComponentName(UContainer *level) const
{
 if(Component)
  return Component->GetLongName(level);
 return std::string("");
}
// --------------------------

// --------------------------
// Методы управления интерфейсом
// --------------------------
// Связывает интерфейс с компонентом
bool UController::Link(UContainer* component, bool forchilds)
{
 if(!component)
  return false;

 if(Component == component)
  return true;

 Component=component;
 Component->AddController(this,forchilds);

 if(!ALink(component))
 {
  component->DelController(this,forchilds);
  Component=0;
  return false;
 }

 return true;
}

// Отвязывает интерфейс от компонента
bool UController::UnLink(bool forchilds)
{
 if(!AUnLink(forchilds))
  return false;

 if(Component)
  Component->DelController(this,forchilds);
 Component=0;
 return true;
}

// Обновляет интерфейс
bool UController::Update(void)
{
 if(!Enabled)
  return true;

 if(!Component)
  return true;

 return AUpdate();
}

// Связывает интерфейс с компонентом
bool UController::ALink(UContainer* component, bool forchilds)
{
 return true;
}

// Отвязывает интерфейс от компонента
bool UController::AUnLink(bool forchilds)
{
 return true;
}
// --------------------------

UControllerData::UControllerData(void)
 : NumPoints(1)
{
}

/// Задает размер хранимых данных
void UControllerData::SetNumPoints(int value)
{
 if(value<0)
  return;

 NumPoints=value;
}


std::string UControllerData::GetPropertyName(void) const
{
 if(Property)
  return Property->GetName();
 return std::string("");
}


/// Возвращает тип данных
const type_info& UControllerData::GetDataType(void) const
{
 if(Property)
  return Property->GetLanguageType();

 // TODO: Нет возвращаемого значения
}

// *****************************************************************

UControllerDataReader::UControllerDataReader(void)
 : PropertyType(0), MRow(0), MCol(0)
{
}

/// Задает длину интервала в секундах
void UControllerDataReader::SetTimeInterval(double value)
{
 TimeInterval=value;
 if(value>0.001)
  SetNumPoints(int(value*Component->GetTimeStep()));
 else
  SetNumPoints(100000);
}

/// Координаты матрицы по которым берутся данные
void UControllerDataReader::SetMatrixCoord(int row, int col)
{
 MRow=row;
 MCol=col;
}

// Отвязывает интерфейс от компонента
bool UControllerDataReader::AUnLink(bool forchilds)
{
 PropertyType=0;
 return UControllerData::AUnLink(forchilds);
}

/// Обновляет хранимые данные добавляя новые
bool UControllerDataReader::AUpdate(void)
{
 if(!Property)
  return false;

 double x(0.0), y(0.0);

 if(PropertyType == 1)
 {
  const double *data=reinterpret_cast<const double*>(Property->GetMemoryArea());
  if(!data)
   return false;

  y=*data;
 }
 else
 if(PropertyType == 2)
 {
  const int *data=reinterpret_cast<const int*>(Property->GetMemoryArea());
  if(!data)
   return false;

  y=*data;
 }
 else
 if(PropertyType == 3)
 {
  const MDMatrix<double> *data=reinterpret_cast<const MDMatrix<double>*>(Property->GetMemoryArea());
  if(!data)
   return false;
  if(data->GetCols()<=MCol)
   return false;
  if(data->GetRows()<=MRow)
   return false;
  y=(*data)(MRow,MCol);
 }
 else
 if(PropertyType == 4)
 {
  const MDMatrix<int> *data=reinterpret_cast<const MDMatrix<int>*>(Property->GetMemoryArea());
  if(!data)
   return false;
  if(data->GetCols()<=MCol)
   return false;
  if(data->GetRows()<=MRow)
   return false;
  y=(*data)(MRow,MCol);
 }
 else
  return false;

 x=Component->GetTime().GetDoubleTime();
 XData.push_back(x);
 YData.push_back(y);
 while(int(XData.size())>NumPoints)
  XData.erase(XData.begin());
 while(int(YData.size())>NumPoints)
  YData.erase(YData.begin());
 return true;
}

/// Удаляет все хранимые данные
void UControllerDataReader::Clear(void)
{
 XData.clear();
 YData.clear();
}

/// Настраивает точку съема данных
bool UControllerDataReader::Configure(UContainer *container, UEPtr<UIProperty> property)
{
 if(Property == property && Component == container)
  return true;

 UnLink();
 Clear();

 if(!Link(container))
  return false;

 if(!property)
 {
  UnLink();
  return false;
 }

 if(property->GetLanguageType() == typeid(double))
 {
  PropertyType=1;
 }
 else
 if(property->GetLanguageType() == typeid(double))
 {
  PropertyType=2;
 }
 else
 if(property->GetLanguageType() == typeid(MDMatrix<double>))
 {
  PropertyType=3;
 }
 else
 if(property->GetLanguageType() == typeid(MDMatrix<int>))
 {
  PropertyType=4;
 }
 else
 {
  UnLink();
  return false;
 }

 Property=property;
 Enabled=true;
 return true;
}

/// Задает размер хранимых данных
void UControllerDataReader::SetNumPoints(int value)
{
 UControllerData::SetNumPoints(value);
}

// *********************************************************************
UControllerDataReaderTimeEvents::UControllerDataReaderTimeEvents(void)
{
}

/// Координаты матрицы по которым берутся данные
void UControllerDataReaderTimeEvents::SetMatrixCoord(int row, int col)
{
 UControllerDataReader::SetMatrixCoord(row, col);
}

/// Обновляет хранимые данные добавляя новые
bool UControllerDataReaderTimeEvents::AUpdate(void)
{
 if(!Property)
  return false;

 if(PropertyType != 3)
  return false;

 double x(0.0);

 const MDMatrix<double> *data=reinterpret_cast<const MDMatrix<double>*>(Property->GetMemoryArea());
 if(!data)
  return false;

 if(data->GetCols()>0 && data->GetRows()>0)
 {
  for(int i=0;i<data->GetCols();i++)
  {
   x=(*data)(0,i);
   std::list<double>::iterator I=XData.begin();
   bool check(false);
   for(;I != XData.end();I++)
   {
	if(*I == x)
	{
	 check=true;
	 break;
	}
   }

   if(check)
	continue;

   XData.push_back(x);
   YData.push_back(0);
   XData.push_back(x+0.00001);
   YData.push_back(1);
   XData.push_back(x+0.00002);
   YData.push_back(0);
   if(int(XData.size())>NumPoints)
	XData.erase(XData.begin());
   if(int(YData.size())>NumPoints)
    YData.erase(YData.begin());
  }

  // Удаляем лишнее
  if(XData.size()>1)
  {
   double t1=XData.front();
   double t2=XData.back();
   while(t2-t1>TimeInterval && !XData.empty())
   {
	XData.erase(XData.begin());
	YData.erase(YData.begin());
	t1=XData.front();
   }
  }
 }

 return true;
}


}


#endif

