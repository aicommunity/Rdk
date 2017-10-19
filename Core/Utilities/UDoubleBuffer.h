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
#include <exception>
#include <algorithm>

using namespace std;

template <typename T>
struct TimedBuffer
{
  vector<T> Data;
  size_t Size;
  boost::posix_time::ptime TimeStamp;
  bool Busy;
  bool Empty;
  TimedBuffer():Size(0),Busy(false),Empty(true){}
};

template <typename T>
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
bool UDoubleBuffer<T>::Write(T* src, const size_t& size)
{
	if(!src)
		throw invalid_argument("Src is empty");
	TimedBuffer<T>* buff = GetPtrForWrite();
	if(!buff)
		return false;
	if(buff->Data.size() < size)
	   buff->Data.resize(size);
	buff->Size = size;
	copy(src, src+size*sizeof(T)-1, buff->Data.begin());
	boost::lock_guard<boost::mutex> guard(mtx);
	buff->Empty = false;
	buff->Busy = false;
	return true;
}
///----------------------------------------------------------------------
bool UDoubleBuffer<T>::Read(T* dst, size_t& size)
{
	if(!dst)
		throw invalid_argument("Dst not exist");
	TimedBuffer<T>* buff = GetPtrForRead();
	if(!buff)
		return false;
	copy(buff->Data.begin(), buff->Data.begin()+ buff->Size*sizeof(T)-1, dst);
	size = buff->Size;
	boost::lock_guard<boost::mutex> guard(mtx);
	buff->Empty = true;
	buff->Busy = false;
	return true;
}
size_t UDoubleBuffer<T>::GetMaxSize()
{
	boost::lock_guard<boost::mutex> guard(mtx);
    return A.Data.size()>B.Data.size()?A.Data.size():B.Data.size();
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
