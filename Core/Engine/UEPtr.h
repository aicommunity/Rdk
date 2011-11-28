#ifndef UEPTR_H
#define UEPTR_H

#include "../Utilities/UPtr.h"
//#include "UEnvException.h"

namespace RDK {

/* Указатель c подсчетом ссылок (слабый аналог shared_ptr) */
template<typename T>
class UESharedPtr
{
public: // Исключения
class UFEUsingZeroPtr: /*public UFatalException,*/ public T::IException {};
 /*
	protected:
		template<class T>
        class obj_val
        {
        public:
            obj_val(T *obj) : _obj(obj), _ref(1) {};
            ~obj_val()
            {
                if(_obj) delete _obj;
            }
            int release(){ return --_ref; }
            void take(){ ++_ref; }
            void set(T *obj){ _obj = obj; }
            T* value() const { return _obj; }
            T** ref_value() { return &_obj; }
			int counter() const { return _ref; }
            void set_counter(int counter) { _ref=counter; }

        private:
            T *_obj;
            int _ref;
        };
	protected:
        obj_val<T> *_val;

    public:

		UESharedPtr() throw() : _val(new obj_val<T>(0))
        {
        }


		UESharedPtr(T* obj) throw() : _val(new obj_val<T>(obj))
        {
        }


        template<class C>
		explicit UESharedPtr(C* obj) throw() : _val(new obj_val<T>(obj))
        {
        }

		UESharedPtr(const UESharedPtr<T>& ptr) throw() : _val(0)
        {
            if(ptr.get())
            {
                _val = ptr._val;
                   _val->take();
            }
            else
            {
				_val = new obj_val<T>(0);
			}
        }


		~UESharedPtr() throw()
        {
            if(_val->release() == 0)
            {
                delete _val;
            }
        }


        T* operator=(T* obj) throw()
        {
            reset(obj);
            return _val->value();
        }


        template<class C>
        T* operator=(C* obj) throw()
        {
            reset(obj);
            return _val->value();
        }


		T* operator=(const UESharedPtr<T>& ptr) throw()
        {
            reset(ptr);
            return _val->value();
        }


        T* get() const
        {
            return _val->value();
        }


        T& operator*() const throw()
        {
			if(!_val->value())
			 throw UFEUsingZeroPtr();
			//assert(_val->value() != 0);
            return *(_val->value());
        }

        T** get_ref() throw()
		{
//            assert(_val->value() == 0);
			if(!_val->value())
			 throw UFEUsingZeroPtr();
			  return _val->ref_value();
		}


		T* operator->() throw()
		{
			if(!_val->value())
			 throw UFEUsingZeroPtr();
 //           assert(_val->value() != 0);
			return _val->value();
		}

		const T* operator->() const throw()
		{
//			assert(_val->value() != 0);
			if(!_val->value())
			 throw UFEUsingZeroPtr();
			return _val->value();
		}

		bool operator!() const throw()
		{
			return _val->value() == 0;
        }

        bool operator==(T *obj) const throw()
        {
            return _val->value() == obj;
        }


        void reset(T *obj = 0) throw()
        {
            if(get() != 0)
            {
                if(_val->release() == 0)
                {
                    delete _val;
                }
                _val = new obj_val<T>(obj);
            }
            else
            {
                _val->set(obj);
            }
        }


        void reset(const UESharedPtr<T>& ptr) throw()
        {
            if(ptr.get())
            {
                if(_val->release() == 0)
                {
                    delete _val;
                }
                _val = ptr._val;
                _val->take();
            }
            else
            {
                reset(0);
            }
		}


        bool unique() const
        {
            return _val->counter() == 1;
        }


        int use_count() const
		{
            return _val->counter();
		}

		operator T* (void) const
		{
		 return _val->value();
		}

		void init(T* pointer, int counter)
		{
            if(ptr.get())
			{
                if(_val->release() == 0)
                {
                    delete _val;
				}
			}

			_val=new obj_val<T>(0);
			_val.set(pointer);
            _val.set_counter(counter+1);
		}       */

//////////////////////////
protected:
T* PData;

// Счетчик ссылок
long* Counter;

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UESharedPtr(void);
UESharedPtr(T* pdata);
UESharedPtr(const UESharedPtr<T> &p);
~UESharedPtr(void);
// --------------------------

// --------------------------
// Операторы
// --------------------------
UESharedPtr<T>& operator = (const UESharedPtr<T> &p);

UESharedPtr<T>& operator = (T *p);

//UESharedPtr<T>& operator = (UEPtr<T> &p);

//UESharedPtr<T>& operator = (UEPtr<T> &p);

bool operator == (const UESharedPtr<T> &p);

bool operator != (const UESharedPtr<T>&p);

//bool operator == (const T *p);

//bool operator != (const T *p);

bool operator ! (void) const;

T* operator -> (void) const;

T& operator * (void);

operator T* (void) const;


// Возвращает число ссылок на объект
long GetCounter(void) const;

long* GetPCounter(void) const;

// Очищает указатель от текущего  объекта
void Clear(void);

T* GetPData(void) const;

// Инициализирует умный указатель
UESharedPtr<T>& Init(long* counter, T* pdata);
// --------------------------

// --------------------------
// Скрытые методы управления
// --------------------------
protected:
// --------------------------
};

/* Простой указатель (слабый аналог weak_ptr) */
template<typename T>
class UEPtr
{
protected:
T* PData;

public: // Исключения
class UFEUsingZeroPtr: /*public UFatalException,*/ public T::IException {};

public: // Методы
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEPtr(void);
UEPtr(T* pdata);
UEPtr(const UEPtr<T> &p);
UEPtr(const UESharedPtr<T> &p);
~UEPtr(void);
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
T* Get(void) const;
// --------------------------

// --------------------------
// Операторы
// --------------------------
UEPtr<T>& operator = (UEPtr<T> &p);

UEPtr<T>& operator = (UESharedPtr<T> &p);

UEPtr<T>& operator = (T *p);

bool operator == (const UEPtr<T> &p);

bool operator != (const UEPtr<T> &p);

bool operator == (const T *p);

bool operator != (const T *p);

bool operator ! (void) const;

T* operator -> (void) const;

T& operator * (void);

operator T* (void) const;
// --------------------------
};




/* class UEPtr */
// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<typename T>
UEPtr<T>::UEPtr(void)
: PData(0)
{
};

template<typename T>
UEPtr<T>::UEPtr(T* pdata)
: PData(pdata)
{
// PData=pdata;
};

template<typename T>
UEPtr<T>::UEPtr(const UEPtr<T> &p)
: PData(p.PData)
{
// PData=p.PData;
};

template<typename T>
UEPtr<T>::UEPtr(const UESharedPtr<T> &p)
: PData(p.GetPData())
{

}


template<typename T>
UEPtr<T>::~UEPtr(void)
{
 PData=0;
};
// --------------------------

// --------------------------
// Методы доступа к данным
// --------------------------
template<typename T>
T* UEPtr<T>::Get(void) const
{
 return PData;
}
// --------------------------

// --------------------------
// Операторы
// --------------------------
template<typename T>
UEPtr<T>& UEPtr<T>::operator = (UEPtr<T> &p)
{
 PData=p.PData;
 return *this;
};

template<typename T>
UEPtr<T>& UEPtr<T>::operator = (UESharedPtr<T> &p)
{
 PData=p.get();
 return *this;
}


template<typename T>
UEPtr<T>& UEPtr<T>::operator = (T *p)
{
 PData=p;
 return *this;
};

template<typename T>
bool UEPtr<T>::operator == (const UEPtr<T> &p)
{ return (PData == p.PData)?true:false; };

template<typename T>
bool UEPtr<T>::operator != (const UEPtr<T> &p)
{ return (PData != p.PData)?true:false; };

template<typename T>
bool UEPtr<T>::operator == (const T *p)
{ return (PData == p)?true:false; };

template<typename T>
bool UEPtr<T>::operator != (const T *p)
{ return (PData != p)?true:false; };

template<typename T>
bool UEPtr<T>::operator ! (void) const
{ return (PData)?false:true; };

template<typename T>
T* UEPtr<T>::operator -> (void) const
{
 return PData;
};

template<typename T>
T& UEPtr<T>::operator * (void)
{
 if(!PData)
  throw UFEUsingZeroPtr();

 return *PData;
};

template<typename T>
UEPtr<T>::operator T* (void) const
{
 return PData;
}
// --------------------------


// Class UESharedPtr
// --------------------------
// Конструкторы и деструкторы
// --------------------------
template<typename T>
UESharedPtr<T>::UESharedPtr(void)
: Counter(new long(1)),PData(0)
{
};

template<typename T>
UESharedPtr<T>::UESharedPtr(T* pdata)
: PData(pdata),
  Counter(new long(1))
{
};

template<typename T>
UESharedPtr<T>::UESharedPtr(const UESharedPtr<T> &p)
: PData(p),
  Counter(p.Counter)
{
 if(Counter)
  ++(*Counter);
};

template<typename T>
UESharedPtr<T>::~UESharedPtr(void)
{
 if(Counter && --(*Counter) == 0)
 {
  delete Counter;
  Counter=0;
  if(PData)
   delete PData;
 }
};
// --------------------------

// --------------------------
// Операторы
// --------------------------
template<typename T>
UESharedPtr<T>& UESharedPtr<T>::operator = (const UESharedPtr<T> &p)
{
 if(Counter)
 {
  if(--(*Counter) == 0)
  {
   delete Counter;
   Counter=0;
   if(PData)
	delete PData;
  }
 }
 PData=p.PData;

// if(PData)
 {
  Counter=p.Counter;
  ++(*Counter);
 }
// else
//  Counter=new long(1);

 return *this;
};

template<typename T>
UESharedPtr<T>& UESharedPtr<T>::operator = (T *p)
{
 if(Counter)
 {
  if(--(*Counter) == 0)
  {
   delete Counter;
   Counter=0;
   if(PData)
	delete PData;
  }
 }
 PData=p;

 if(PData)
  Counter=new long(1);
 else
  Counter=new long(1);

 return *this;
};

template<typename T>
T& UESharedPtr<T>::operator * (void)
{
 if(!PData)
  throw UFEUsingZeroPtr();

 return *PData;
};

template<typename T>
bool UESharedPtr<T>::operator == (const UESharedPtr<T> &p)
{ return (PData == p.PData)?true:false; };

template<typename T>
bool UESharedPtr<T>::operator != (const UESharedPtr<T> &p)
{ return (PData != p.PData)?true:false; };
/*
template<typename T>
bool UESharedPtr<T>::operator == (const T *p)
{ return (PData == p)?true:false; };

template<typename T>
bool UESharedPtr<T>::operator != (const T *p)
{ return (PData != p)?true:false; };
*/
template<typename T>
bool UESharedPtr<T>::operator ! (void) const
{ return (PData)?false:true; };

template<typename T>
T* UESharedPtr<T>::operator -> (void) const
{
 if(!PData)
  throw UFEUsingZeroPtr();

 return PData;
};


template<typename T>
UESharedPtr<T>::operator T* (void) const
{
 return PData;
}


// Возвращает число ссылок на объект
template<typename T>
long UESharedPtr<T>::GetCounter(void) const
{ return *Counter; };

template<typename T>
long* UESharedPtr<T>::GetPCounter(void) const
{ return Counter; };

// Очищает указатель от текущего  объекта
template<typename T>
void UESharedPtr<T>::Clear(void)
{
 if(Counter && --(*Counter) == 0)
 {
  delete Counter;
  Counter=0;
  if(PData)
   delete PData;
 }
}

template<typename T>
T* UESharedPtr<T>::GetPData(void) const
{
 return PData;
}


// Инициализирует умный указатель
template<typename T>
UESharedPtr<T>& UESharedPtr<T>::Init(long* counter, T* pdata)
{
 Clear();
 Counter=counter;
 ++(*Counter);
 PData=pdata;
 return *this;
}
// --------------------------


// -----------------------------------------------------------
// Приведение  типов
// -----------------------------------------------------------
template<class T, class U>
UESharedPtr<T> static_pointer_cast(UESharedPtr<U> const & r)
{
 UESharedPtr<T> result;
 result.Init(r.GetPCounter(),static_cast<T*>(r.GetPData()));
// result.reset(static_cast<T*>(r.get()));
 return result;
}

template<class T, class U>
UESharedPtr<T> const_pointer_cast(UESharedPtr<U> const & r)
{
 UESharedPtr<T> result;
 result.Init(r.GetPCounter(),const_cast<T*>(r.GetPData()));
 return result;
}

template<class T, class U>
UESharedPtr<T> dynamic_pointer_cast(UESharedPtr<U> const & r)
{
 UESharedPtr<T> result;
 result.Init(r.GetPCounter(),dynamic_cast<T*>(r.GetPData()));
 //result.reset(dynamic_cast<T*>(r.get()));
 return result;
}

template<class T, class U>
UEPtr<T> static_pointer_cast(UEPtr<U> const & r)
{
 UEPtr<T> result;
 result=static_cast<T*>(r.Get());
 return result;
}

template<class T, class U>
UEPtr<T> const_pointer_cast(UEPtr<U> const & r)
{
 UEPtr<T> result;
 result=const_cast<T*>(r.Get());
 return result;
}

template<class T, class U>
UEPtr<T> dynamic_pointer_cast(UEPtr<U> const & r)
{
 UEPtr<T> result;
 result=dynamic_cast<T*>(r.Get());
 return result;
}
// -----------------------------------------------------------

}

#endif

