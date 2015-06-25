//---------------------------------------------------------------------------

#pragma hdrstop

#include "TGrabberThread.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

__fastcall TGrabberThread::TGrabberThread(int Mode, String Data)
{
	m_mode=Mode;
	m_data=Data;
	ExistsEvent=CreateEvent(0,TRUE,FALSE,0);
	CommandMutex = new TMutex(false);
}

TVideoGrabberFrame* TGrabberThread::GetVideoGrabberFrame(void)
{
 return frame;
}

void __fastcall TGrabberThread::Execute(void)
{
	frame = new TVideoGrabberFrame(0);
//	frame->InitByAvi("");
//	frame->Start();
//	frame->Stop();
/*	if(m_mode == 0)
	{
	 frame->InitByAvi(m_data);
	}
	else
	if(m_mode == 1)
	{
	 frame->InitByIpCamera(m_data,"admin","admin");
	}
           */
	SetEvent(ExistsEvent);
	while(!Terminated)
	{
	  // extract command form queue
	  CommandMutex->Acquire();
	  int cmd=0;
	  if(!CommandQueue.empty())
	  {
	   cmd=CommandQueue.front();
	   CommandQueue.erase(CommandQueue.begin());
	  }
	  CommandMutex->Release();

	  // apply command if exist
	  switch (cmd)
	  {
	  case 1:
	   StartCapture();
	  break;

	  case 2:
	   StopCapture();
	  break;

	  }
	  Sleep(10);
	}

}
void TGrabberThread::GetData(TImage *img)
{
	frame->GetImage(img);
}

__fastcall TGrabberThread::~TGrabberThread()
{
	Terminate();
	WaitFor();
	delete frame;
}

void TGrabberThread::StartCapture()
{
	frame->Start();
}
void TGrabberThread::StopCapture()
{
    frame->Stop();
}
void TGrabberThread::StartCaptureThread()
{
	CommandMutex->Acquire();
	CommandQueue.push_back(1);
	CommandMutex->Release();
}

void TGrabberThread::StopCaptureThread()
{
   CommandMutex->Acquire();
   CommandQueue.push_back(2);
   CommandMutex->Release();
}
