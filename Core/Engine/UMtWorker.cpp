#include "UMtWorker.h"

namespace RDK {

UMtWorker::UMtWorker(UEPtr<UContainer> owner)
 : Owner(owner)
 , IsProcessing(true)
{
 SleepMutex.unlock();
 CompleteMutex.unlock();
 Thread = std::thread(std::bind(&UMtWorker::Process,this));
}

UMtWorker::~UMtWorker(void)
{
 IsProcessing = false;
 WaitForComplete();
}

void UMtWorker::AddProcessingComponent(const std::string &name)
{
 if(!Owner)
  return;

 if(ProcessingComponents.find(name) != ProcessingComponents.end())
  return;

 std::unique_lock<std::mutex> locker(SleepMutex);

 UEPtr<UContainer> cont = Owner->GetComponent(name,true);

 if(cont)
  ProcessingComponents[name] = cont;
}

void UMtWorker::ClearAllProcessingComponents(void)
{
 std::unique_lock<std::mutex> locker(SleepMutex);
 ProcessingComponents.clear();
}

void UMtWorker::Init(void)
{

}

void UMtWorker::UnInit(void)
{

}

void UMtWorker::Calculate(void)
{

}

void UMtWorker::WaitForComplete(void)
{

}

void UMtWorker::Process(void)
{
 while(IsProcessing)
 {
  SleepMutex.lock();
  std::this_thread::sleep_for(1ms);
 }
}

}
