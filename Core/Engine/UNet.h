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
std::vector<UCItem> ItemsList;

/// ��� �����
int InputType;

public:
/// ������������ � �����������
UIPropertyInputBase(void);
virtual ~UIPropertyInputBase(void);

/// ���������� ��� �������� �����-������
virtual int GetInputType(void) const;

public: // ������ ������� � ��������� ������
/// ���������� ��������� �� ���������-��������
virtual const std::vector<UCItem>& GetItemsList(void) const;

/// ���������� ����� ����������� �� �����
virtual int GetNumConnections(void) const;

// ���������� ��������� �� ���������-��������
UNet* GetItem(int c_index=-1);

// ���������� ���������� � ������ ������ � item ��� ������ ������
// ���� ����� ����� �����������
virtual void GetCLink(const UEPtr<UNet> &item, std::vector<UCLink> &buffer) const;
virtual void GetCLink(const UNet* const item, std::vector<UCLink> &buffer) const;

/// ���������� ��� ������������� ����������
virtual std::string GetItemName(int c_index=-1) const;

/// ���������� ������ ��� ������������� ����������
virtual std::string GetItemFullName(int c_index=-1) const;

/// ���������� ��� ������������� ������
virtual std::string GetItemOutputName(int c_index=-1) const;

/// ���������� true ���� ���� ����� �����������
virtual bool IsConnected(void) const;

/// ��������� ����� � ��������� ���� ������������ �� ����� 'item_property_name'
/// ���������� true, ���� �������� �� �������� �����������
virtual bool Disconnect(const NameT &item_property_name, int c_index=-1);

/// ��������� ����� � �������� c_index, ��� ��� ����� ���� c_index == -1
/// ���� c_index ����� �� ���������� ��������, �� �� ������ ������
virtual void Disconnect(int c_index=-1);

/// ��������� ��� ����� �� ���������
virtual void DisconnectAll(void);

// ���������, ���������� �� ����� � �������� �����������
bool IsConnectedTo(const UEPtr<UNet> &item) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool IsConnectedTo(const UEPtr<UNet> &item, const NameT &item_property_name, int c_index=-1) const;

public: // ������ ���������� ���������� �� ������� ������
/// ���������� ��������� �� ������
virtual void const* GetPointer(int index) const=0;

/// ������������� ��������� �� ������
virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// ���������� ��������� �� ������
virtual bool ResetPointer(int index, void* value)=0;

protected:
/// ���������� �����
virtual bool Connect(UNet* item, const std::string &output_name, int &c_index, bool forced_connect_same_item=false);

// ��������� ��� ����� � ��������� ���� 'na'
virtual void Disconnect(UEPtr<UNet> na);

/// ��������� ����� � ��������� ���� 'na' � ������� 'item_property_name'
virtual void Disconnect(UEPtr<UNet> na, const NameT &item_property_name);

/// ��������� ����� � ��������� ���� 'na', ������� 'item_property_name' � ������ 'connector_property_name'
virtual void Disconnect(UEPtr<UNet> na, const NameT &item_property_name, const NameT &connector_property_name, int c_index=-1);
};

class RDK_LIB_TYPE UIPropertyOutputBase: virtual public UIPropertyOutput
{
protected: // ������
std::vector<PUAConnector> RelatedConnectors;

/// ��������� �� ����������-��������� ������
std::vector<UNet*> Connectors;

/// ��� ������ ���������-��������� ������
std::vector<std::string> ConnectorInputNames;

public: // ������������ � �����������
UIPropertyOutputBase(void);
virtual ~UIPropertyOutputBase(void);

public: // ������ ������� � ������������ ������
/// ���������� ����� ������������ ������
virtual size_t GetNumConnectors(void) const;

/// ���������� ��������� �� ���������-��������
virtual UNet* GetConnector(int index);

/// ���������� ��� ������������� ����� ����������-���������
virtual std::string GetConnectorInputName(int index) const;

/// ������������� ����� ����� ������ �� ������ input_property
virtual bool Connect(UIPropertyInput *input_property);

/// ��������� ����� ����� ������ �� ������ input_property
virtual bool Disconnect(UIPropertyInput *input_property);

// ��������� ����� ������ ����� ������� � ����������� �� Id 'id'.
//virtual bool Disconnect(const UId &id);

// ��������� ����� ������ ����� ������� �� �����
// ������������� ������������.
virtual void DisconnectAll(void);

public: // ������ ���������� ���������� �� ������� ������
/// ���������� ��������� �� ������
virtual void const* GetPointer(int index) const=0;

/// ������������� ��������� �� ������
//virtual bool SetPointer(int index, void* value, UIProperty* output)=0;

/// ���������� ��������� �� ������
//virtual bool ResetPointer(int index, void* value)=0;

protected:
// ������������� ����� � ����������� 'c'
virtual bool Connect(UEPtr<UNet> c, const NameT &connector_property_name, int &c_index, bool forced_connect_same_item=false);

/// ��������� ��� ����� ������ ����� ������� � ����������� 'c'.
virtual void Disconnect(UEPtr<UNet> c);

// ��������� ����� ������ ����� ������� � ����������� 'c' �� �������
virtual void Disconnect(UEPtr<UNet> c, const NameT &connector_property_name, int c_index=-1);

// ����������  ��������� �� ������ �����������.
//virtual UEPtr<UConnector> GetAConnectorByIndex(int c_index=-1) const;

// ���������, ���������� �� ����� � �������� �����������
bool CheckLink(const UEPtr<UNet> &connector, int c_index) const;

// ���������, ���������� �� ����� � �������� ����������� � ���������� ������
bool CheckLink(const UEPtr<UNet> &connector, const NameT &connector_property_name, int c_index=-1) const;

// ����������������� ��� ����� ����� ������ �� ����� connectors
virtual void BuildLinks(void);
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
virtual void BreakInputLinks(const NameT &connectorname, const NameT &connector_index, int connector_c_index=-1);

// ��������� ��� ����� ����
// �������� �� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual void BreakLinks(UEPtr<UContainer> brklevel);

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
ULinksList& GetLinks(ULinksList &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals=false, UEPtr<UContainer> internal_level=0) const;

// ��������� ��� ����� ����� ����� ������������ � ���� xml � ����� buffer
// ������� ����� ����� ����������
// ���� 'sublevel' == -1, �� ���������� ����� ��� ����� ����� �������� � ����� �������� �����������
// ������� �������. �������� ����������� � ��� �������.
// ���� 'sublevel' == 0, �� ���������� ����� ������ ����� ����� ���������
ULinksList& GetPersonalLinks(UEPtr<RDK::UNet> cont, ULinksList &linkslist, UEPtr<UContainer> netlevel, int sublevel=-1);
// --------------------------
/*
/// ��������� ��� �������� ����� � �����������
virtual void DisconnectAllInputs(void);

/// ��������� ��� ��������� ����� �� �����������
virtual void DisconnectAllOutputs(void);

/// ��������� ��� ����� � �������� �������
virtual void DisconnectFromOutput(const std::string &property_name);

/// ��������� ������� ����� ����� ������� ����� ���������� � ������ cont
bool IsLinkExists(UEPtr<UNet> cont, const NameT &itemname,
						const NameT &connectorname, int connector_c_index);
                */
/// ���� ��������-����� �� ��������� �������
virtual UIPropertyOutput* FindOutputProperty(const NameT &property_name) const;
virtual UIPropertyInput* FindInputProperty(const NameT &property_name) const;

/// ���������� ����� ������
virtual int GetNumInputs(void) const;

/// ���������� ����� ������
virtual int GetNumOutputs(void) const;

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
ULinksList& GetLinks(UEPtr<UContainer> cont, ULinksList &linkslist, UEPtr<UContainer> netlevel, bool exclude_internals, UEPtr<UContainer> internal_level=0) const;

ULinksList& GetPersonalLinks(UEPtr<UContainer> cont, UEPtr<UContainer> cont2, ULinksList &linkslist, UEPtr<UContainer> netlevel) const;
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
