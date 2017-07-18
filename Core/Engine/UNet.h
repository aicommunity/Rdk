/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2002.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */
#ifndef UANetH
#define UANetH

#include "UContainer.h"
#include "UStorage.h"
#include "ULocalProperty.h"


namespace RDK {

class UNet;
typedef UEPtr<UNet> PUAConnector;


class RDK_LIB_TYPE UIPropertyInputBase: virtual public UIPropertyInput
{
protected: // ������
/// ������ ������������ �� ����� ����������
//std::vector<UCItem> ItemsList;

/// ��������� �� ��������-��������� ������
std::list<UEPtr<UIPropertyOutput> > ConnectedProperties;

/// ��� �����
int InputType;

public:
/// ������������ � �����������
UIPropertyInputBase(void);
virtual ~UIPropertyInputBase(void);

/// ���������� ��� �������� �����-������
virtual int GetInputType(void) const;

public: // ������ ������� � ��������� ������
/// ���������� ����� ����������� �� �����
virtual int GetNumConnections(void) const;

/// ���������� ��������� �� ��������-��������� ������
virtual const std::list<UEPtr<UIPropertyOutput> > GetConnectedProperties(void) const;

/// ���������� ������ ��� ������������� ����������
virtual std::string GetItemFullName(int c_index=-1) const;

/// ���������� ��� ������������� ������
virtual std::string GetItemOutputName(int c_index=-1) const;

/// ���������� true ���� ���� ����� �����������
virtual bool IsConnected(void) const;

// ���������, ���������� �� ����� � �������� �����������
virtual bool IsConnectedTo(const UIPropertyOutput *output_property) const;

/// ��������� ����� �� ��������� output_property
virtual bool Disconnect(UIPropertyOutput *output_property);

/// ��������� ��� ����� �� ���������
virtual bool DisconnectAll(void);

public:
/// ��������� �������� �� ���������� ����� �� ��������� output_property
virtual bool ConnectToOutput(UIPropertyOutput *output_property);

/// ��������� �������� �� ����������� ����� �� ��������� output_property
virtual bool DisconnectFromOutput(UIPropertyOutput *output_property);

public: // ������ ���������� ���������� �� ������� ������
/// ���������� ��������� �� ������
//virtual void const* GetPointer(int index) const=0;

/// ������������� ��������� �� ������
//virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// ���������� ��������� �� ������
//virtual bool ResetPointer(int index, void* value)=0;
};

class RDK_LIB_TYPE UIPropertyOutputBase: virtual public UIPropertyOutput
{
protected: // ������
/// ��������� �� ��������-��������� ������
std::list<UEPtr<UIPropertyInput> > ConnectedProperties;

public: // ������������ � �����������
UIPropertyOutputBase(void);
virtual ~UIPropertyOutputBase(void);

public: // ������ ������� � ������������ ������
/// ���������� ����� ������������ ������
virtual size_t GetNumConnectors(void) const;

/// ���������� ��������� �� ��������-��������� ������
virtual const std::list<UEPtr<UIPropertyInput> > GetConnectedProperties(void) const;

/// ������������� ����� ����� ������ �� ������ input_property
virtual bool Connect(UIPropertyInput *input_property);

/// ��������� ����� ����� ������ �� ������ input_property
virtual bool Disconnect(UIPropertyInput *input_property);

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
virtual bool DisconnectAll(void);

/// ���������� true ���� ����� ��������� � ���������� �����
virtual bool IsConnectedTo(UIPropertyInput *input_property);

/// ���������� true ���� ����� ��������� � ������ �� ������ ���������� ����������
virtual bool IsConnectedTo(UNet *component);

public: // ������ ���������� ���������� �� ������� ������
/// ���������� ��������� �� ������
//virtual void const* GetPointer(int index) const=0;
};



class RDK_LIB_TYPE UNet: public UContainer
{
protected: // �������� ��������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UNet(void);
virtual ~UNet(void);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<UContainer> comp) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
virtual bool AAddComponent(UEPtr<UContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UContainer> comp);
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
public:
// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
virtual bool Copy(UEPtr<UContainer> target, UEPtr<UStorage> stor=0, bool copystate=false) const;

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
virtual void Free(void);
// --------------------------

// ----------------------
// ������ ���������� �������
// ----------------------
public:
// ������������� ����� ����� 'link'
bool CreateLink(const ULink &link, bool forced_connect_same_item=false);

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
bool CreateLink(const ULinkSide &itemid, const ULinkSide &connectorid, bool forced_connect_same_item=false);

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
virtual bool CreateLink(const NameT &item, const NameT &item_index,
						const NameT &connector, const NameT &connector_index, int connector_c_index=-1, bool forced_connect_same_item=false);

// ������������� ��� ����� �� ������� 'linkslist'
bool CreateLinks(const ULinksList &linkslist, UEPtr<UNet> owner_level=0);

// ��������� ����� 'link'
bool BreakLink(const ULink &link);

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
bool BreakLink(const ULinkSide &item, const ULinkSide &connector);

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
virtual bool BreakLink(const NameT &item, const NameT &item_property_name,
						const NameT &connector, const NameT &connector_property_name, int connector_c_index=-1);

// ��������� ��� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
virtual bool BreakLink(const NameT &itemname, const NameT &connectorname);

// ��������� ��� ����� ����� ������� �������� ���� � ������ ������������
virtual bool BreakOutputLinks(void);
virtual bool BreakOutputLinks(const NameT &itemname);
virtual bool BreakOutputLinks(const NameT &itemname, const NameT &item_property_name);

// ��������� ����� �� ����� connector_index ���������� 'connectorid'
virtual bool BreakInputLinks(void);
virtual bool BreakInputLinks(const NameT &connectorname, const NameT &connector_index, int connector_c_index=-1);

// ��������� ��� ����� ����
// �������� �� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual bool BreakLinks(UEPtr<UContainer> brklevel);

// ��������� ��� ����� �������
// �������� ��� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual bool DisconnectBy(UEPtr<UContainer> brklevel);

// ��������� �������� ����� ����
virtual bool BreakLinks(const ULinksList &linkslist);

// ��������� ��� ���������� ����� ����.
virtual void BreakLinks(void);


// ���������, ���������� �� �������� �����
bool IsLinkExists(const ULinkSide &item, const ULinkSide &connector);

bool IsLinkExists(const NameT &itemname, const NameT &item_property_name,
						const NameT &connectorname, const NameT &connector_property_name, int connector_c_index=-1);
bool IsLinkExists(const NameT &itemname,
						const NameT &connectorname);
// ----------------------


// --------------------------
// ������ ������� � ���������
// --------------------------
// ��������� ��� ����� ���������� � ���� xml � ����� buffer
ULinksList& GetLinks(ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals=false, UEPtr<UNet> internal_level=0) const;

// ��������� ��� ����� ���������� � ���� xml � ����� buffer
// ������� ����� ����� ����������
// ���� 'sublevel' == -1, �� ���������� ����� ��� ����� ����� �������� � ����� �������� �����������
// ������� �������. �������� ����������� � ��� �������.
// ���� 'sublevel' == 0, �� ���������� ����� ������ ����� ����� ���������
ULinksList& GetPersonalLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, int sublevel=-1);
// --------------------------

public:
/// ���� ��������-����� �� ��������� �������
virtual UIPropertyOutput* FindOutputProperty(const NameT &property_name);

/// ���� ��������-���� �� ��������� �������
virtual UIPropertyInput* FindInputProperty(const NameT &property_name);

/// ���������� ����� ������
virtual int GetNumInputs(void);

/// ���������� ����� ������
virtual int GetNumOutputs(void);

// --------------------------
// ������ ������������ ���������
// --------------------------
// ���������� �������� ���������� �� ��������������
virtual bool GetComponentProperties(RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ���������� ���������� �������� ���������� �� ��������������
// ������ ��� buffer ������ ���� ��������!
virtual bool GetComponentSelectedProperties(RDK::USerStorageXML *serstorage);

// ���������� �������� ���������� �� �������������� � ����������
// ������ ��� buffer ������ ���� ��������!
virtual bool GetComponentPropertiesEx(RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ������������� �������� ���������� �� ��������������
virtual int SetComponentProperties(RDK::USerStorageXML *serstorage);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� � xml
virtual bool SaveComponent(RDK::USerStorageXML *serstorage, bool links, unsigned int params_type_mask);

// ��������� ��� ���������� ������ ����������, � ���� ��� �������� ���������, ��������
// ���������� ��������� �� xml
virtual bool LoadComponent(RDK::USerStorageXML *serstorage, bool links);

// ��������� ��� �������� ���������� � ��� �������� ��������� � xml
virtual bool SaveComponentProperties(RDK::USerStorageXML *serstorage, unsigned int type_mask);

// ��������� ��� �������� ���������� � ��� �������� ��������� �� xml
virtual bool LoadComponentProperties(RDK::USerStorageXML *serstorage);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// ������� ���� ���������
virtual void SetGlobalComponentPropertyValue(UId classid, const char *paramname, const char *buffer);

// ������������� �������� �������� ���� �������� ����������� ���������� stringid, ����������� �� ������ class_stringid
// � ����������, ����������� �� ������ 'class_owner_stringid' ������� ���� ���������
virtual void SetGlobalOwnerComponentPropertyValue(UId classid, UId owner_classid, const char *paramname, const char *buffer);

// ��������� ��� ����� ������ ���������� � ���� xml � ����� buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentInternalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ������������� ��� ����� ������ ���������� stringid �� ������ xml � ������ buffer
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int SetComponentInternalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ��������� ��� ������� ����� � ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentInputLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// ��������� ��� �������� ����� �� ���������� stringid � ���� xml � ����� buffer
// ���� 'sublevel' == -2, �� ���������� ����� ���� ��������� �������
// ��� ��������� ���� � ��� ������������ ���������.
// ���� 'sublevel' == -1, �� ���������� ����� ���� �������� �������
// ��� ��������� ����.
// ���� 'sublevel' == 0, �� ���������� ����� �������� ������ ���� ����
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentOutputLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level, int sublevel=-1);

// ��������� ��� ������� ����� c ����������� cont � ��� ��������� ������������ � ���� xml � ����� buffer
// ���������� � ������ ����������� ������������ ��������� ���������� cont!
// ����� ����������� �� ������ ���������� owner_level
// ���� owner_level �� �����, �� ����� ����������� �� ������ �������� ����������
virtual int GetComponentPersonalLinks(RDK::USerStorageXML *serstorage, RDK::UNet* owner_level);

// ��������� ���������� ������ ����������, � ��� _����������������_ �������� ���������, ��������
// ���������� ��������� � xml
virtual bool SaveComponentDrawInfo(RDK::USerStorageXML *serstorage);
// --------------------------

// ----------------------
// ������ ���������� ������������ �������� ������
// ----------------------
/// ��������� ������������� ���������� � �������� ������
template<typename T>
UEPtr<T> FindComponentByNameAndType(const NameT &component_name);

/// ��������� ������������� ���������� � �������� ������ � ������� ���
/// ��� �������������.
/// ���������� ��������� �� ��������� ���������, ���� �� ��� ��������
/// ��� 0
template<typename T>
UEPtr<T> AddMissingComponent(const NameT &component_name, const NameT &class_name);
// ----------------------

// --------------------------
// ������� ������ ������� � ���������
// --------------------------
protected:
ULinksList& GetLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals, UEPtr<UNet> internal_level=0) const;

ULinksList& GetPersonalLinks(UEPtr<UNet> cont, UEPtr<UNet> cont2, ULinksList &linkslist, UEPtr<UNet> netlevel) const;

// ��������� ��� ����� ���������� � ���� xml � ����� buffer
virtual ULinksList& GetOutputLinks(ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals=false, UEPtr<UNet> internal_level=0) const;
virtual ULinksList& GetInputLinks(ULinksList &linkslist, UEPtr<UNet> netlevel, bool exclude_internals=false, UEPtr<UNet> internal_level=0) const;

// ��������� ��� ����� ���������� � ���� xml � ����� buffer
// ������� ����� ����� ����������
// ���� 'sublevel' == -1, �� ���������� ����� ��� ����� ����� �������� � ����� �������� �����������
// ������� �������. �������� ����������� � ��� �������.
// ���� 'sublevel' == 0, �� ���������� ����� ������ ����� ����� ���������
virtual ULinksList& GetOutputPersonalLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, int sublevel=-1);
virtual ULinksList& GetInputPersonalLinks(UEPtr<UNet> cont, ULinksList &linkslist, UEPtr<UNet> netlevel, int sublevel=-1);
// --------------------------
};

// ----------------------
// ������ ���������� ������������ �������� ������
// ----------------------
/// ��������� ������������� ���������� � �������� ������
template<typename T>
UEPtr<T> UNet::FindComponentByNameAndType(const NameT &component_name)
{
 UEPtr<T> comp=dynamic_pointer_cast<T>(GetComponent(component_name,true));
 if(comp)
  return comp;

 return 0;
}


/// ��������� ������������� ���������� � �������� ������ � ������� ���
/// ��� �������������.
/// ���������� ��������� �� ��������� ���������, ���� �� ��� ��������
/// ��� 0
template<typename T>
UEPtr<T> UNet::AddMissingComponent(const NameT &component_name, const NameT &class_name)
{
 UEPtr<T> comp=dynamic_pointer_cast<T>(GetComponent(component_name,true));
 if(comp)
  return comp;

 if(!Storage)
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - Storage not found"));
  return comp;
 }

 UEPtr<UComponent> proto=Storage->TakeObject(class_name);
 if(!proto)
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - Component not found in storage. ClassName=")+class_name);
  return comp;
 }
 comp=dynamic_pointer_cast<T>(proto);
 comp->SetName(component_name);
 comp->SetTimeStep(TimeStep);
 if(!AddComponent(comp))
 {
  LogMessage(RDK_EX_WARNING, std::string("AddMissingComponent - AddComponent failed. ClassName=")+class_name);
  Storage->ReturnObject(comp);
  comp=0;
  return comp;
 }
 return comp;
}
// ----------------------



}

#endif
