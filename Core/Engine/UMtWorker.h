#ifndef UMTWORKER_H
#define UMTWORKER_H

#include <thread>
#include <mutex>
#include <map>
#include <string>
#include <functional>
#include "UEPtr.h"
#include "UContainer.h"

namespace RDK {

class UMtWorker
{
protected:
 UEPtr<UContainer> Owner;

 std::map<std::string, UEPtr<UContainer>> ProcessingComponents;

private:
 std::thread Thread;

 std::mutex SleepMutex;
 std::mutex CompleteMutex;
 std::atomic_bool IsProcessing;

public:
 UMtWorker(UEPtr<UContainer> owner);
 ~UMtWorker(void);

 void AddProcessingComponent(const std::string &name);
 void ClearAllProcessingComponents(void);

 void Init(void);
 void UnInit(void);

 void Calculate(void);
 void WaitForComplete(void);

protected:
 void Process(void);
};

}

#endif
