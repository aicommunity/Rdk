/* ***********************************************************
@Copyright Alexsandr V. Bakhshiev, 2005.
E-mail:        alexab@ailab.ru
Url:           http://ailab.ru

This file is part of the project: RDK

File License:       New BSD License
Project License:    New BSD License
See file license.txt for more information
*********************************************************** */

#ifndef USUPPORT_CPP
#define USUPPORT_CPP


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <mem.h>

#include "USupport.h"
#include "UPtr.cpp"

namespace RDK {

// ��������� �����
string ustrbuf;
wstring uwstrbuf;
// --------------------------------------------------



// �������� ������� ����� ����� � ��������� digs ������ ����� �������
int fraction(double d, int digs)
{
 d-=(int)d;
 d*=pow(10.0,digs);

 return int(d);
}


}
#endif


 
