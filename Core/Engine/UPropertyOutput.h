#ifndef UOUTPUT_PROPERTY_H
#define UOUTPUT_PROPERTY_H

#include "ULocalProperty.h"
#include "UADItem.h"

namespace RDK {

// -----------------------------------------------------------------------------
// Output properties
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputBase: public ULProperty<T,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputBase(const string &name, OwnerT * const owner, int input_type)
 : ULProperty<T,OwnerT,type>(name, owner)
{
 this->IoType=input_type;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
operator T* (void)
{
 return this->PData;
}

/*bool operator ! (void) const
{
 return (GetPointer(0))?true:false;
};

T* operator -> (void)
{
 return &this->v;
};

T& operator * (void)
{
 return this->v;
};

operator T* (void)
{
 return &this->v;
}

T& Value(void)
{
 return this->v;
}

const T& Value(void) const
{
 return this->v;
}

T& operator [] (int i)
{
 return this->v;
}

const T& operator [] (int i) const
{
 return this->v;
}
       */
// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �� ������ ������
void const * GetPointer(int index) const
{
 return &this->v;
}

// ������������� ��������� �� ������ ������
bool SetPointer(int index, void* value)
{
 return true;
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputData: public UPropertyOutputBase<T,OwnerT,type>
{
protected:

public: // ������

// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputData(const string &name, OwnerT * const owner)
 : UPropertyOutputBase<T,OwnerT,type>(name, owner, ipSingle | ipData)
{

};

/// Deprecated
UPropertyOutputData(const string &name, OwnerT * const owner, int index)
 : UPropertyOutputBase<T,OwnerT,type>(name, owner, ipSingle | ipData)
{

};
// -----------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputCBase: protected UCLProperty<std::vector<T>,OwnerT,type>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputCBase(const string &name, OwnerT * const owner, int input_type)
 : UCLProperty<std::vector<T>,OwnerT,type>(name, owner)
{
 this->IoType=input_type;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
T& Value(int i)
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}

const T& Value(int i) const
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}

T& operator [] (int i)
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}

const T& operator [] (int i) const
{
 if(int(this->v.size())<=i)
 #ifdef __BORLANDC__
  throw UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #else
  throw typename UCLProperty<std::vector<T>,OwnerT>::EPropertyRangeError(UVBaseProperty<std::vector<T*>,OwnerT>::GetOwnerName(),UVBaseProperty<std::vector<T*>,OwnerT>::GetName(),
	0,int(this->v.size()),i);
 #endif

 return this->v[i];
}
// --------------------------
};

template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputCData: public UPropertyOutputCBase<T,OwnerT,type>
{
protected:

public: // ������

// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputCData(const string &name, OwnerT * const owner)
 : UPropertyOutputCBase<T,OwnerT,type>(name, owner, ipRange | ipData)
{

};


/// Deprecated
UPropertyOutputCData(const string &name, OwnerT * const owner, int index)
 : UPropertyOutputCBase<T,OwnerT,type>(name, owner, ipRange | ipData)
{

};
// -----------------------------
};

// -----------------------------------------------------------------------------
// Output virtual properties
// -----------------------------------------------------------------------------
template<typename T, typename OwnerT>
class UVPropertyOutputBase: public UVProperty<T,OwnerT>, /*public UPropertyIOBase, */public UIPropertyOutput
{
protected:

public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVPropertyOutputBase(OwnerT * const owner, T* data, int input_type)
 : UVProperty<T,OwnerT>(owner,data)
{
 this->IoType=input_type;
};
// -----------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------

// --------------------------
// ������ ���������� ����������
// --------------------------
// ���������� ��������� �� ������ ������
void const * GetPointer(int index) const
{
 return this->PData;
}

// ������������� ��������� �� ������ ������
bool SetPointer(int index, void* value)
{
 return true;
}
// --------------------------
};

template<typename T, typename OwnerT>
class UVPropertyOutputData: public UVPropertyOutputBase<T,OwnerT>
{
protected:

public: // ������

// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UVPropertyOutputData(OwnerT * const owner, T* data)
 : UVPropertyOutputBase<T,OwnerT>(owner, data, ipSingle | ipData)
{

};

/// Deprecated
UVPropertyOutputData(OwnerT * const owner, T* data, int index)
 : UVPropertyOutputBase<T,OwnerT>(owner, data, ipSingle | ipData)
{

};
// -----------------------------
};

}
#endif
