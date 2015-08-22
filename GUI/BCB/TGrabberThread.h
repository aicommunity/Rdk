//---------------------------------------------------------------------------

#ifndef TGrabberThreadH
#define TGrabberThreadH
//---------------------------------------------------------------------------
#include <vcl.h>
#include <list>
#include "TVideoGrabberFrame.h"

class TGrabberThread: public TThread
{
protected: // Data
	TVideoGrabberFrame *frame;
private:
	int m_mode;
	String m_data;
	TMutex *CommandMutex;
	/// Command queue
	std::list<int> CommandQueue;
public:
	__fastcall TGrabberThread(int Mode, String Data);
	virtual __fastcall ~TGrabberThread(void);
	virtual void __fastcall Execute(void);

	TVideoGrabberFrame* GetVideoGrabberFrame(void);
	void GetData(TImage *img);
	void StartCapture();
	void StopCapture();
	void StartCaptureThread();
	void StopCaptureThread();
	HANDLE ExistsEvent;

};

#endif
