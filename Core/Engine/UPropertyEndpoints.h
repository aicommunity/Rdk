//---------------------------------------------------------------------------

#ifndef UPropertyEndpointsH
#define UPropertyEndpointsH

#include "UProperty.h"
//---------------------------------------------------------------------------

namespace RDK {

/// �������������: �������� - �������
template<typename V, typename OwnerT, unsigned int type>
class UProperty<std::vector<V>,OwnerT,type>: public UPropertyLocal<std::vector<V>,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<std::vector<V>,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<std::vector<V>,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag) //ipSingle | ipData
{
};

/// �������� ������� �� �������
typename std::vector<V>::reference operator [] (size_t index)
{
 return this->Value[index];
}

const typename std::vector<V>::const_reference operator [] (size_t index) const
{
 return this->Value[index];
}

void resize(size_t size, const V &val)
{
 Value.resize(size,val);
}

void resize(size_t size)
{
 Value.resize(size);
}

void assign(size_t size, const V &val)
{
 Value.assign(size,val);
}

typename std::vector<V>::const_iterator begin(void) const
{
 return Value.begin();
}

typename std::vector<V>::iterator begin(void)
{
 return Value.begin();
}

typename std::vector<V>::const_iterator end(void) const
{
 return Value.end();
}

typename std::vector<V>::iterator end(void)
{
 return Value.end();
}

size_t size(void) const
{
 return Value.size();
}

/// �������� ������������
UProperty<std::vector<V>,OwnerT,type>& operator = (const std::vector<V> &value)
{
 this->SetData(value);
 return *this;
};

};

/// �������������: �������� - MDMatrix
template<typename V, typename OwnerT, unsigned int type>
class UProperty<MDMatrix<V>,OwnerT,type>: public UPropertyLocal<MDMatrix<V>,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<MDMatrix<V>,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<MDMatrix<V>,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag) //ipSingle | ipData
{
};

/// �������� ������� �� �������
V& operator [] (size_t index)
{
 return Value[index];
}

const V& operator [] (size_t index) const
{
 return Value[index];
}

V& operator () (int row, int col)
{
 return Value(row,col);
}

const V& operator () (int row, int col) const
{
 return Value(row,col);
}

void Resize(int rows, int cols)
{
 Value.Resize(rows,cols);
}

void Resize(const MMatrixSize &size)
{
 Value.Resize(size);
}

void Assign(int rows, int cols, const V &val)
{
 Value.Assign(rows,cols,val);
}

void Assign(int rows, int cols, const V *val)
{
 Value.Assign(rows,cols,val);
}

int GetRows(void) const
{
 return Value.GetRows();
}

int GetCols(void) const
{
 return Value.GetCols();
}

void ToZero(void) const
{
 Value.ToZero();
}

/// �������� ������������
UProperty<MDMatrix<V>,OwnerT,type>& operator = (const MDMatrix<V> &value)
{
 this->SetData(value);
 return *this;
};

};


/// �������������: �������� - double
template<typename OwnerT, unsigned int type>
class UProperty<double,OwnerT,type>: public UPropertyLocal<double,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<double,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<double,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag)
{
};

operator double (void) const
{ return Value; }

UProperty<double,OwnerT,type>& operator = (const double &value)
{
 this->SetData(value);
 return *this;
};

};

/// �������������: �������� - int
template<typename OwnerT, unsigned int type>
class UProperty<int,OwnerT,type>: public UPropertyLocal<int,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<int,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<int,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag)
{
};

operator int (void) const
{ return Value; }

UProperty<int,OwnerT,type>& operator = (const int &value)
{
 this->SetData(value);
 return *this;
};

int operator ++ (void)
{
 if(SetterR)
  SetData(Value+1);
 else
  ++Value;
 return Value;
}

int operator ++ (int)
{
 int temp=Value;
 if(SetterR)
  SetData(Value+1);
 else
  Value++;
 return temp;
}

int operator -- (void)
{
 if(SetterR)
  SetData(Value-1);
 else
  ++Value;
 return Value;
}

int operator -- (int)
{
 int temp=Value;
 if(SetterR)
  SetData(Value-1);
 else
  Value--;
 return temp;
}

};

/// �������������: �������� - int
template<typename OwnerT, unsigned int type>
class UProperty<unsigned int,OwnerT,type>: public UPropertyLocal<unsigned int,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<unsigned int,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<unsigned int,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag)
{
};

operator unsigned int (void) const
{ return Value; }

UProperty<unsigned int,OwnerT,type>& operator = (const unsigned int &value)
{
 this->SetData(value);
 return *this;
};

unsigned int operator ++ (void)
{
 if(SetterR)
  SetData(Value+1);
 else
  ++Value;
 return Value;
}

unsigned int operator ++ (int)
{
 int temp=Value;
 if(SetterR)
  SetData(Value+1);
 else
  Value++;
 return temp;
}

unsigned int operator -- (void)
{
 if(SetterR)
  SetData(Value-1);
 else
  ++Value;
 return Value;
}

unsigned int operator -- (int)
{
 unsigned int temp=Value;
 if(SetterR)
  SetData(Value-1);
 else
  Value--;
 return temp;
}

};

/// �������������: �������� - bool
template<typename OwnerT, unsigned int type>
class UProperty<bool,OwnerT,type>: public UPropertyLocal<bool,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<bool,OwnerT>::SetterRT setmethod=0, bool dynamic_prop_flag=false)
 : UPropertyLocal<bool,OwnerT,type>(name, owner, setmethod, dynamic_prop_flag)
{
};

operator bool (void) const
{ return Value; }

UProperty<bool,OwnerT,type>& operator = (const bool &value)
{
 this->SetData(value);
 return *this;
};

};

/// deprecated
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class ULProperty: public UPropertyLocal<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
ULProperty(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner, setmethod) //ipSingle | ipData
{
};

// �������� ������������
ULProperty<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};


};
                               /*
template<typename T, typename OwnerT, unsigned int type=ptPubParameter>
class UPropertyParam: public UPropertyLocal<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyParam(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyLocal<T,OwnerT,type>(name, owner,setmethod) //ipSingle | ipData
{
};

// �������� ������������
UPropertyParam<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};

template<typename T, typename OwnerT, unsigned int type=ptPubState>
class UPropertyState: public UProperty<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyState(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipData
{
};

// �������� ������������
UPropertyState<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};
									 */
template<typename T, typename OwnerT, unsigned int type=ptPubOutput>
class UPropertyOutputData: public UProperty<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyOutputData(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner,setmethod) //ipSingle | ipData
{
};

// �������� ������������
UPropertyOutputData<T,OwnerT,type>& operator = (const T &value)
{
 this->SetData(value);
 return *this;
};

};
/*
template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInput: public UProperty<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInput(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipComp
{
};

};*/

template<typename T, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputData: public UProperty<T,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputData(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UProperty<T,OwnerT,type>(name, owner, setmethod) // ipSingle | ipDatas
{
};

};

template<typename T, typename RangeT, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputDataRange: public UPropertyRange<T,RangeT,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputDataRange(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyRange<T,RangeT,OwnerT,type>(name, owner, setmethod) // ipRange | ipData
{
};

};

/// deprecated
template<typename T, typename RangeT, typename OwnerT, unsigned int type=ptPubInput>
class UPropertyInputDataC: public UPropertyRange<T,RangeT,OwnerT,type>
{
public: // ������
// --------------------------
// ������������ � �����������
// --------------------------
//����������� �������������.
UPropertyInputDataC(const string &name, OwnerT * const owner, typename UPropertyVirtual<T,OwnerT>::SetterRT setmethod=0)
 : UPropertyRange<T,RangeT,OwnerT,type>(name, owner, setmethod) // ipRange | ipData
{
};

};

}
#endif