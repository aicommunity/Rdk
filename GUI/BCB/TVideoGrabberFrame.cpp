//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TVideoGrabberFrame.h"
#include "TVideoSourceThread.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "VidGrab"
#pragma resource "*.dfm"
TVideoGrabberFrame *VideoGrabberFrame;

extern String TVGrabberLicenseString;

//---------------------------------------------------------------------------
__fastcall TVideoGrabberFrame::TVideoGrabberFrame(TComponent* Owner)
	: TFrame(Owner)
{
	mutex=new TMutex(false);

	std::string lic_string=AnsiString(TVGrabberLicenseString).c_str();
	std::vector<std::string> licenses;
	RDK::separatestring(lic_string,licenses,'|');
	for(size_t i=0;i<licenses.size();i++)
	 VideoGrabber1->LicenseString=licenses[i].c_str();//TVGrabberLicenseString;

	CallbackThread=0;

	InitByAvi("");
	Start();
	Stop();
}

__fastcall TVideoGrabberFrame::~TVideoGrabberFrame(void)
{
	//mutex->Acquire();
	delete mutex;
}

void TVideoGrabberFrame::SetCallbackThread(TVideoCaptureThreadNewVideoGrabber *callback)
{
 CallbackThread=callback;
}

TVideoCaptureThreadNewVideoGrabber* TVideoGrabberFrame::GetCallbackThread(void)
{
 return CallbackThread;
}



//---------------------------------------------------------------------------

void TVideoGrabberFrame::InitByAvi(const String &file_name)
{
 m_mode = 1;
// VideoGrabber1->StopPreview();
 VideoGrabber1->AutoStartPlayer = false;
 VideoGrabber1->PlayerFileName = file_name;
 VideoGrabber1->BurstType = fc_TBitmap;
 VideoGrabber1->BurstMode = true;
 VideoGrabber1->BurstCount = 0;
}

void TVideoGrabberFrame::InitByDevice(int camera_index, int input_index, int size_index, int subtype_index, int analog_index,
			int tv_tuner_channel, int tv_tuner_country_code, int tv_tuner_mode, int tv_tuner_input_type)
{
	m_mode = 2;

 VideoGrabber1->BurstType = fc_TBitmap;
 VideoGrabber1->BurstMode = true;
 VideoGrabber1->BurstCount = 0;
}

void TVideoGrabberFrame::InitByIpCamera(const String camera_url, const String user_name, const String user_password)
{
	m_mode = 3;
	VideoGrabber1->ClosePlayer();
	VideoGrabber1->VideoSource = vs_IPCamera;
	VideoGrabber1->IPCameraURL = camera_url;
	VideoGrabber1->OpenURLAsync = true;
	VideoGrabber1->SetAuthentication(at_IPCamera, user_name, user_password);

 VideoGrabber1->BurstType = fc_TBitmap;
 VideoGrabber1->BurstMode = true;
 VideoGrabber1->BurstCount = 0;
}

//---------------------------------------------------------------------------
void TVideoGrabberFrame::Start()
{
	switch(m_mode)
	{
		case 1:
			VideoGrabber1->OpenPlayer();
			VideoGrabber1->RunPlayer();
		break;

		case 2:
			VideoGrabber1->StartPreview();
		break;

		case 3:
			VideoGrabber1->StartPreview();
		break;
	}

}

void TVideoGrabberFrame::Stop()
{
 VideoGrabber1->Stop();
/*
	switch(m_mode)
	{
		case 0:
			VideoGrabber1->ClosePlayer();
		break;
		case 1:
			VideoGrabber1->StopPreview();
		break;
	}
	*/
}

void __fastcall TVideoGrabberFrame::VideoGrabber1FrameCaptureCompleted(TObject *Sender,
		  Pointer FrameBitmap, int BitmapWidth, int BitmapHeight, DWORD FrameNumber,
		  __int64 FrameTime, TFrameCaptureDest DestType, UnicodeString FileName,
		  bool Success, int FrameId)
{
   if(DestType != fc_TBitmap)
   	return;

   mutex->Acquire();
   Graphics::TBitmap *Frame_Bitmap;
   Frame_Bitmap = (Graphics::TBitmap*) FrameBitmap;
   Image1->Picture->Assign(Frame_Bitmap);
   if(CallbackThread)
    CallbackThread->WriteSourceSafe(Frame_Bitmap,TDateTime::CurrentDateTime().operator double(),false);
   mutex->Release();
}
//---------------------------------------------------------------------------
void TVideoGrabberFrame::GetImage(TImage *img)
{
	mutex->Acquire();
	img->Picture->Assign(Image1->Picture);
	mutex->Release();
}

TBitmap* TVideoGrabberFrame::GetDirectBitmapAccess(void)
{
 return Image1->Picture->Bitmap;
}

void TVideoGrabberFrame::LockImageAccess(void)
{
	mutex->Acquire();
}

void TVideoGrabberFrame::UnLockImageAccess(void)
{
	mutex->Release();
}
//---------------------------------------------------------------------------

