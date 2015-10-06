#ifndef UENGINE_STATE_THREAD_H
#define UENGINE_STATE_THREAD_H

#include <string>
#include <vector>
#define BOOST_THREAD_USE_LIB
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include "../../Deploy/Include/rdk.h"

namespace RDK {

class RDK_LIB_TYPE UEngineStateThread
{
protected:

public:
// --------------------------
// Конструкторы и деструкторы
// --------------------------
UEngineStateThread(void);
virtual ~UEngineStateThread(void);
// --------------------------
};

}

#endif
