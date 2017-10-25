/************************************************************************
*	19.10.2017          												*
*	Tseluyko V.V.               										*
*	v.celuyko@rtc.ru            										*
*   Description:                										*
*		Fast thread-safe double buffer									*
************************************************************************/
#ifndef UDOUBLE_BUFFER
#define UDOUBLE_BUFFER

#include <boost/thread.hpp>
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <vector>

using namespace std;

template <class T>
struct TimedBuffer
{
  T Data;
  boost::posix_time::ptime TimeStamp;
  bool Busy;
  bool Empty;
  TimedBuffer():Busy(false),Empty(true), TimeStamp(boost::posix_time::not_a_date_time){}
  void Clear()
  {
	Busy = false;
	Empty = true;
	TimeStamp = boost::posix_time::not_a_date_time;
  }
};

template <class T>
class UDoubleBuffer
{
private:
	boost::mutex mtx;
	TimedBuffer<T> A;
	TimedBuffer<T> B;

///----------------------------------------------------------------------
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///----------------------------------------------------------------------
public:
UDoubleBuffer<T>::UDoubleBuffer()
{

}
///----------------------------------------------------------------------
UDoubleBuffer<T>::~UDoubleBuffer()
{

}
///----------------------------------------------------------------------
bool UDoubleBuffer<T>::Write(const T& src)
{
	TimedBuffer<T>* buff = GetPtrForWrite();
	if(!buff)
		return false;
	buff->Data = src;
	boost::lock_guard<boost::mutex> guard(mtx);
	buff->Empty = false;
	buff->Busy = false;
	return true;
}
///----------------------------------------------------------------------
bool UDoubleBuffer<T>::Read(T& dst)
{
	TimedBuffer<T>* buff = GetPtrForRead();
	if(!buff)
		return false;
	dst = buff->Data;
	boost::lock_guard<boost::mutex> guard(mtx);
	buff->Empty = true;
	buff->Busy = false;
	return true;
}
///----------------------------------------------------------------------
void Clear()
{
	boost::lock_guard<boost::mutex> guard(mtx);
	A.Clear();
    B.Clear();
}
///----------------------------------------------------------------------
private:
TimedBuffer<T>* UDoubleBuffer<T>::GetPtrForWrite()
{
	boost::lock_guard<boost::mutex> guard(mtx);
	if(!A.Busy)
	{
		if(A.Empty)
		{
			A.Busy=true;
			A.TimeStamp=boost::posix_time::microsec_clock::local_time();
			return &A;
		}
		else
		{
			if(!B.Busy)
			{
				if(B.Empty)
				{
					B.Busy=true;
					B.TimeStamp=boost::posix_time::microsec_clock::local_time();
					return &B;
				}
				else
				{
					if(A.TimeStamp>B.TimeStamp)
					{
                        B.Busy=true;
						B.TimeStamp=boost::posix_time::microsec_clock::local_time();
						return &B;
					}
					else
					{
						A.Busy=true;
						A.TimeStamp=boost::posix_time::microsec_clock::local_time();
						return &A;
					}
				}
			}
			else
			{
				A.Busy=true;
				A.TimeStamp=boost::posix_time::microsec_clock::local_time();
				return &A;
			}
		}
	}
	else
	{
		if(!B.Busy)
		{
            B.Busy=true;
			B.TimeStamp=boost::posix_time::microsec_clock::local_time();
			return &B;
		}
	}
    return 0;
}
///----------------------------------------------------------------------
TimedBuffer<T>* UDoubleBuffer<T>::GetPtrForRead()
{
	boost::lock_guard<boost::mutex> guard(mtx);
    if(A.TimeStamp>B.TimeStamp)
	{
		if(!A.Busy)
		{
			if(!A.Empty)
			{
				A.Busy=true;
				return &A;
			}
		}
		if(!B.Busy)
		{
			if(!B.Empty)
			{
				B.Busy=true;
				return &B;
			}
		}
		return 0;
	}
	else
	{
		if(!B.Busy)
		{
			if(!B.Empty)
			{
				B.Busy=true;
				return &B;
			}
		}
        if(!A.Busy)
		{
			if(!A.Empty)
			{
				A.Busy=true;
				return &A;
			}
		}
		return 0;
	}
}
///----------------------------------------------------------------------
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///----------------------------------------------------------------------
};
#endif
