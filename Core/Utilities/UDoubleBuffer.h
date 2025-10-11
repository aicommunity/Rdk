/************************************************************************
*	19.10.2017          												*
*	Tseluyko V.V.               										*
*	v.celuyko@rtc.ru            										*
*   Description:                										*
*		Fast thread-safe double buffer									*
************************************************************************/
#ifndef UDOUBLE_BUFFER
#define UDOUBLE_BUFFER

#include <mutex>
#include <vector>
#include "../System/ModernChrono.h"

using namespace std;
using namespace RDK;

// Unix epoch (January 1, 1970)
constexpr TimePoint Epoch = UnixEpoch();

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
	Empty = true;
	TimeStamp = 0;
  }
};

template <class T>
class UDoubleBuffer
{
private:
	std::mutex mtx;
	TimedBuffer<T> A;
	TimedBuffer<T> B;

///----------------------------------------------------------------------
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///----------------------------------------------------------------------
public:
UDoubleBuffer()
{

}
///----------------------------------------------------------------------
~UDoubleBuffer()
{

}
///----------------------------------------------------------------------
bool Write(const T& src )
{
	TimedBuffer<T>* buff = GetPtrForWrite();
	if(!buff)
		return false;
	buff->Data = src;
	std::lock_guard<std::mutex> guard(mtx);
	buff->Empty = false;
	buff->Busy = false;
	return true;
}
///----------------------------------------------------------------------
bool Read(T& dst)
{
	TimedBuffer<T>* buff = GetPtrForRead();
	if(!buff)
		return false;
	dst = buff->Data;
	std::lock_guard<std::mutex> guard(mtx);
	buff->Empty = true;
	buff->Busy = false;
	return true;
}
///----------------------------------------------------------------------
void Clear()
{
	std::lock_guard<std::mutex> guard(mtx);
	A.Clear();
    B.Clear();
}
///----------------------------------------------------------------------
/// Not memory safe methods
///----------------------------------------------------------------------
TimedBuffer<T>* GetPtrForWrite()
{
	std::lock_guard<std::mutex> guard(mtx);
	if(!A.Busy)
	{
		if(A.Empty)
		{
			A.Busy=true;
			A.TimeStamp=GetCurrentTimeMs();
			return &A;
		}
		else
		{
			if(!B.Busy)
			{
				if(B.Empty)
				{
					B.Busy=true;
					B.TimeStamp=GetCurrentTimeMs();
					return &B;
				}
				else
				{
					if(A.TimeStamp>B.TimeStamp)
					{
                        B.Busy=true;
						B.TimeStamp=GetCurrentTimeMs();
						return &B;
					}
					else
					{
						A.Busy=true;
						A.TimeStamp=GetCurrentTimeMs();
						return &A;
					}
				}
			}
			else
			{
				A.Busy=true;
				A.TimeStamp=GetCurrentTimeMs();
				return &A;
			}
		}
	}
	else
	{
		if(!B.Busy)
		{
            B.Busy=true;
			B.TimeStamp=GetCurrentTimeMs();
			return &B;
		}
	}
    return 0;
}
///----------------------------------------------------------------------
TimedBuffer<T>* GetPtrForRead()
{
	std::lock_guard<std::mutex> guard(mtx);
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
void MakeWrited(TimedBuffer<T>* buff)
{
	std::lock_guard<std::mutex> guard(mtx);
	buff->Empty = false;
	buff->Busy = false;
} ///----------------------------------------------------------------------
void MakeReaded(TimedBuffer<T>* buff)
{
	std::lock_guard<std::mutex> guard(mtx);
	buff->Empty = true;
	buff->Busy = false;
}
///----------------------------------------------------------------------
///++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
///----------------------------------------------------------------------
};
#endif
