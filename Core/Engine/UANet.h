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

#include "UADItem.h"

namespace RDK {

class UANet: public UADItem
{
protected: // �������� ��������

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
UANet(void);
virtual ~UANet(void);
// --------------------------

// --------------------------
// ������ ������� � ���������
// --------------------------
ULinksList& GetLinks(ULinksList &linkslist, UEPtr<UAContainer> netlevel) const;
// --------------------------

// --------------------------
// ��������� ������ ���������� ��������
// --------------------------
// �������� ���� ������ � 'target' � ����������� ���� ���������
// � �������� ����������
// ���� 'stor' == 0, �� �������� �������� ��������������
// � ��� �� ��������� ��� ������������� ���� ������
virtual bool Copy(UEPtr<UAContainer> target, UEPtr<UAContainerStorage> stor=0, bool copystate=false) const;

// ������������ ������������ ����� ������� � ��� ���������
// ��� ����� �����������, ���� Storage == 0
virtual void Free(void);
// --------------------------

// --------------------------
// ������ ������� � �����������
// --------------------------
// ����� ��������� �� ������������ ������� ������� ����
// � �������� ���������� ������� �������
// ����� ���������� 'true' � ������ ������������
// � 'false' � ������ ������������� ����
virtual bool CheckComponentType(UEPtr<UAContainer> comp) const;
// --------------------------

// --------------------------
// ������� ������ ���������� ������������
// --------------------------
protected:
// ��������� ����������� ���������������� ��������
// ��� ���������� ��������� ���������� � ���� ������
// ����� ����� ������ ������ ���� comp ���
// ������� �������� � ������ ���������
virtual bool AAddComponent(UEPtr<UAContainer> comp, UEPtr<UIPointer> pointer=0);

// ��������� ��������������� ���������������� ��������
// ��� �������� ��������� ���������� �� ����� �������
// ����� ����� ������ ������ ���� comp
// ���������� � ������ ���������
virtual bool ADelComponent(UEPtr<UAContainer> comp);
// --------------------------

// ----------------------
// ������ ���������� �������
// ----------------------
public:
// ������������� ����� ����� 'link'
virtual bool CreateLink(const ULink &link);

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
virtual bool CreateLink(const ULinkSide &itemid, const ULinkSide &connectorid);
virtual bool CreateLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);

// ������������� ����� ����� ����� ������� �������� ����
// 'item' � ����������� 'connector'
virtual bool CreateLink(const NameT &item, int item_index,
						const NameT &connector, int connector_index=-1);

// ������������� ��� ����� �� ������� 'linkslist'
virtual bool CreateLinks(const ULinksList &linkslist);

// ��������� ��� ����� � ������� �������� ����, ����
// 'id' - ���� Id �������� ����.
// �����, ���� 'id' - Id ����������, �� ����� ���������
// ����� ����� ����������
virtual bool BreakLink(const ULinkSide &id);

// ��������� ����� 'link'
virtual bool BreakLink(const ULink &link);

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
virtual bool BreakLink(const ULinkSide &item, const ULinkSide &connector);
virtual bool BreakLink(const ULongId &item_id, int item_index, const ULongId &conn_id, int conn_index);

// ��������� ����� ����� ������� �������� ����, 'itemid'
// � ����������� 'connectorid'
virtual bool BreakLink(const NameT &itemname, int item_index,
                        const NameT &connectorname, int connector_index);

// ��������� ��� ����� ����
// �������� �� ���������� ����� � �������� �����
// brklevel - ������, ������������ �������� ����� ��������� �����������
virtual void BreakLinks(UEPtr<UAContainer> brklevel);

// ��������� �������� ����� ����
virtual bool BreakLinks(const ULinksList &linkslist);

// ��������� ��� ���������� ����� ����.
virtual void BreakLinks(void);
// ----------------------

// --------------------------
// ������� ������ ������� � ���������
// --------------------------
protected:
ULinksList& GetLinks(UEPtr<UAContainer> cont, ULinksList &linkslist, UEPtr<UAContainer> netlevel) const;
// --------------------------


};

}

#endif
