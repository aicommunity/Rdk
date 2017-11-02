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
using namespace boost::posix_time;

ptime const Epoch(boost::gregorian::date(1970,1,1));

template <class T>
struct TimedBuffer
{
  T Data;
  bool Busy;
  bool Empty;
  uint64_t TimeStamp;
  TimedBuffer():Busy(false),Empty(true),TimeStamp(0){}
  void Clear()
  {
	Busy = false;
	Empty = true;
	TimeStamp = 0;
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
/// Not memory safe methods
///----------------------------------------------------------------------
TimedBuffer<T>* UDoubleBuffer<T>::GetPtrForWrite()
{
	boost::lock_guard<boost::mutex> guard(mtx);
	if(!A.Busy)
	{
		if(A.Empty)
		{
			A.Busy=true;
			A.TimeStamp=(microsec_clock::local_time() - Epoch).total_microseconds();
			return &A;
		}
		else
		{
			if(!B.Busy)
			{
				if(B.Empty)
				{
					B.Busy=true;
					B.TimeStamp=(microsec_clock::local_time() - Epoch).total_microseconds();
					return &B;
				}
				else
				{
					if(A.TimeStamp>B.TimeStamp)
					{
                        B.Busy=true;
						B.TimeStamp=(microsec_clock::local_time() - Epoch).total_microseconds();
						return &B;
					}
					else
					{
						A.Busy=true;
						A.TimeStamp=(microsec_clock::local_time() - Epoch).total_microseconds();
						return &A;
					}
				}
			}
			else
			{
				A.Busy=true;
				A.TimeStamp=(microsec_clock::local_time() - Epoch).total_microseconds();
				return &A;
			}
		}
	}
	else
	{
		if(!B.Busy)
		{
            B.Busy=true;
			B.TimeStamp=(microsec_clock::local_time() - Epoch).total_microseconds();
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
void UDoubleBuffer<T>::MakeWrited(TimedBuffer<T>* buff)
{
	boost::lock_guard<boost::mutex> guard(mtx);
	buff->Empty = false;
	buff->Busy = false;
} ///----------------------------------------------------------------------
void UDoubleBuffer<T>::MakeReaded(TimedBuffer<T>* buff)
{
	boost::lock_guard<boost::mutex> guard(mtx);
	buff->Empty = true;
	buff->Busy = false;
}
///----------------------------------------------------------------------
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///----------------------------------------------------------------------
};
#endif
