//---------------------------------------------------------------------------

#ifndef TVideoGrabberFrameH
#define TVideoGrabberFrameH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "VidGrab.hpp"
#include <Vcl.ExtCtrls.hpp>

class TVideoCaptureThreadNewVideoGrabber;
//---------------------------------------------------------------------------
class TVideoGrabberFrame : public TFrame
{
__published:	// IDE-managed Components
	TVideoGrabber *VideoGrabber1;
	TImage *Image1;
	void __fastcall VideoGrabber1FrameCaptureCompleted(TObject *Sender, Pointer FrameBitmap,
          int BitmapWidth, int BitmapHeight, DWORD FrameNumber, __int64 FrameTime,
          TFrameCaptureDest DestType, UnicodeString FileName, bool Success,
          int FrameId);
	void __fastcall VideoGrabber1DeviceLost(TObject *Sender);
	void __fastcall VideoGrabber1FrameBitmap(TObject *Sender, pFrameInfo FrameInfo,
          pFrameBitmapInfo BitmapInfo);
	void __fastcall VideoGrabber1Log(TObject *Sender, TLogType LogType, UnicodeString Severity,
          UnicodeString InfoMsg);
	void __fastcall VideoGrabber1PlayerEndOfStream(TObject *Sender);
	void __fastcall VideoGrabber1PlayerOpened(TObject *Sender);
	void __fastcall VideoGrabber1PreviewStarted(TObject *Sender);
	void __fastcall VideoGrabber1MouseDown(TObject *Sender, int VideoWindow, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall VideoGrabber1MouseUp(TObject *Sender, int VideoWindow, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall VideoGrabber1MouseMove(TObject *Sender, int VideoWindow, TShiftState Shift,
          int X, int Y);
private:	// User declarations
	/// Capture mode
	int m_mode;
	String m_data;

	/// Captured image access mutex
	TMutex *mutex;

	TVideoCaptureThreadNewVideoGrabber *CallbackThread;
public:		// User declarations
	__fastcall TVideoGrabberFrame(TComponent* Owner);
	__fastcall ~TVideoGrabberFrame(void);

	void SetCallbackThread(TVideoCaptureThreadNewVideoGrabber *callback);
	TVideoCaptureThreadNewVideoGrabber* GetCallbackThread(void);

	void GetImage(TImage *img);
	TBitmap* GetDirectBitmapAccess(void);
	void LockImageAccess(void);
	void UnLockImageAccess(void);

	void InitByAvi(const String &file_name);
	void InitByDevice(int camera_index, int input_index, int size_index, int subtype_index, int analog_index,
			int tv_tuner_channel, int tv_tuner_country_code, int tv_tuner_mode, int tv_tuner_input_type);
	void InitByIpCamera(const String camera_url, const String user_name, const String user_password);

	void Start();
	void Stop();

};
//---------------------------------------------------------------------------
extern PACKAGE TVideoGrabberFrame *VideoGrabberFrame;
//---------------------------------------------------------------------------
#endif
