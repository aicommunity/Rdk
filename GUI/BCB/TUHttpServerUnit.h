//---------------------------------------------------------------------------

#ifndef TUHttpServerUnitH
#define TUHttpServerUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdCustomHTTPServer.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdHTTPServer.hpp>
#include <Web.HTTPApp.hpp>
#include <Web.HTTPProd.hpp>
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdHTTP.hpp>
#include <IdHTTPServer.hpp>
#include <IdTCPConnection.hpp>
#include <IdCoder.hpp>
#include <IdCoder3to4.hpp>
#include <IdCoderMIME.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <IdMessageCoder.hpp>
#include <IdMessageCoderMIME.hpp>

#include <string>
#include <map>
#include <vector>
#include "myrdk.h"
//---------------------------------------------------------------------------
class TUHttpServerFrame : public TFrame
{
__published:	// IDE-managed Components
	TIdHTTPServer *IdHTTPServer;
	TPageProducer *PageProducer;
	TImage *Image1;
	void __fastcall IdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
          TIdHTTPResponseInfo *AResponseInfo);
private:	// User declarations
public:		// User declarations
	__fastcall TUHttpServerFrame(TComponent* Owner);
	__fastcall ~TUHttpServerFrame(void);

	TMemoryStream* MemStream;
	TStringStream* DataStream;
	TBitmap *Bitmap;

	std::map<std::string,std::vector<char> > ParsedRequestArgs;


	RDK::UBitmap TempUBitmap;


	void Init(void);
	void UnInit(void);

void __fastcall ParsingSimpleRequest(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
		  TIdHTTPResponseInfo *AResponseInfo, std::map<std::string,std::vector<char> > &args);

void __fastcall ParsingMultiRequest(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
		  TIdHTTPResponseInfo *AResponseInfo, std::map<std::string,std::vector<char> > &args);

void __fastcall ProcessMimePart(TIdMessageDecoder *&VDecoder, bool &VMsgEnd, std::vector<char> &arg);

RDK::UBitmap& DecodeParamAsBitmap(const std::vector<char> &param);

};
//---------------------------------------------------------------------------
extern PACKAGE TUHttpServerFrame *UHttpServerFrame;
//---------------------------------------------------------------------------
#endif
