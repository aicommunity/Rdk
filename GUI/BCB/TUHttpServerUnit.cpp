//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TUHttpServerUnit.h"
#include "myrdk.h"
#include "TUBitmap.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "TUVisualControllerFrameUnit"
#pragma resource "*.dfm"
TUHttpServerFrame *UHttpServerFrame;
//---------------------------------------------------------------------------
__fastcall TUHttpServerFrame::TUHttpServerFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
 MemStream=0;
 Bitmap=0;
 Init();
}

__fastcall TUHttpServerFrame::~TUHttpServerFrame(void)
{
 UnInit();
}
//---------------------------------------------------------------------------
// --------------------------
// Методы управления сервером
// --------------------------
/// Возвращает порт
int TUHttpServerFrame::GetListenPort(void) const
{
 return IdHTTPServer->Bindings->Items[0]->Port;
}

/// Устанавливает новый порт
/// Возвращает 0 в случае успеха
int TUHttpServerFrame::SetListenPort(int port)
{
 ServerListenOff();
 IdHTTPServer->Bindings->Items[0]->Port=port;
 return 0;
}

void TUHttpServerFrame::Init(void)
{
 MemStream=new TMemoryStream;
 Bitmap=new Graphics::TBitmap;
}

void TUHttpServerFrame::UnInit(void)
{
 ServerListenOff();

 if(MemStream)
  delete MemStream;

 if(Bitmap)
  delete Bitmap;
}


/// Включает сервер
int TUHttpServerFrame::ServerListenOn(void)
{
 IdHTTPServer->Active=true;
}

/// Выключает сервер
int TUHttpServerFrame::ServerListenOff(void)
{
 IdHTTPServer->Active=false;
}
// -------------------------

//---------------------------------------------------------------------------
void __fastcall TUHttpServerFrame::ProcessMimePart(TIdMessageDecoder *&VDecoder, bool &VMsgEnd, std::vector<char> &arg)
{
 TMemoryStream* LMStream;
 TIdMessageDecoder* LNewDecoder;

 LMStream = new TMemoryStream;
 try
 {
  LNewDecoder = VDecoder->ReadBody(LMStream, VMsgEnd);
  try
  {
	  LMStream->Position = 0;
	  // use VDecoder.Headers and LMStream as needed...
   arg.resize(LMStream->Size);
   if(arg.size()>0)
   {
	LMStream->ReadBuffer(&arg[0],LMStream->Size);
	LMStream->Position=0;
	int data_size=arg.size();
	if(data_size>1)
	{
	 char* data=&arg[0];
	 if((data[0] == '\r' && data[1] == '\n'))
	  arg.erase(arg.begin(),arg.begin()+2);
	 if(data[data_size-2]== '\r')
	  arg.resize(data_size-2);
	}
   }
  }
  catch(...)
  {
   delete LNewDecoder;
   throw;
  }
  delete VDecoder;
  VDecoder = LNewDecoder;
  }
  __finally
  {
   //	FreeAndNil(LMStream);
   delete LMStream;
  }
}

void __fastcall TUHttpServerFrame::ParsingSimpleRequest(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
		  TIdHTTPResponseInfo *AResponseInfo, std::map<std::string,std::vector<char> > &args)
{
  int params_count=ARequestInfo->Params->Count;
  for(int i=0;i<params_count;i++)
  {
   std::string param=AnsiString(ARequestInfo->Params->Strings[i]).c_str();
   std::vector<std::string> params;
   RDK::separatestring(param,params,'=');
   if(params.size() == 2)
   {
	args[params[0]].resize(params[1].size());
	if(params[1].size()>0)
	 memcpy(&args[params[0]][0],&params[1][0],params[1].size());
   }
  }
}

void __fastcall TUHttpServerFrame::ParsingMultiRequest(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
		  TIdHTTPResponseInfo *AResponseInfo, std::map<std::string,std::vector<char> > &args)
{

  String LBoundary, LBoundaryStart, LBoundaryEnd;
  TIdMessageDecoder* LDecoder=0;
  String LLine;
  bool LBoundaryFound, LIsStartBoundary, LMsgEnd;

  LBoundary = ExtractHeaderSubItem(ARequestInfo->ContentType, "boundary", QuoteHTTP);
  if(LBoundary == "")
  {
   AResponseInfo->ResponseNo = 400;
   AResponseInfo->CloseConnection = true;
   AResponseInfo->WriteHeader();
   return;
  }

  LBoundaryStart = "--" + LBoundary;
  LBoundaryEnd = LBoundaryStart + "--";

  LDecoder = new TIdMessageDecoderMIME(0,"");
  try
  {
   ((TIdMessageDecoderMIME*)LDecoder)->MIMEBoundary = LBoundary;
   LDecoder->SourceStream = ARequestInfo->PostStream;
   LDecoder->FreeSourceStream = false;

   LBoundaryFound = false;
   LIsStartBoundary = false;
   do
   {
	LLine = ReadLnFromStream(ARequestInfo->PostStream, -1, true);
	if(LLine == LBoundaryStart)
	{
	 LBoundaryFound = true;
	 LIsStartBoundary = true;
	}
	else
	if(LLine == LBoundaryEnd)
	{
	 LBoundaryFound = true;
	}
   } while(!LBoundaryFound);

   if( (!LBoundaryFound) || (!LIsStartBoundary) )
   {
	AResponseInfo->ResponseNo = 400;
	AResponseInfo->CloseConnection = true;
	AResponseInfo->WriteHeader();
	return;
   }

   LMsgEnd = false;

   do
   {
	((TIdMessageDecoderMIME*)LDecoder)->MIMEBoundary = LBoundary;
	LDecoder->SourceStream = ARequestInfo->PostStream;
	LDecoder->FreeSourceStream = false;

	LDecoder->ReadHeader();
	std::string name;
	if(LDecoder->Headers->Count > 0)
	{
	 name=AnsiString(LDecoder->Headers->Strings[0]).c_str();
	 std::string::size_type i=name.find("name=\"");
	 if(i != std::string::npos)
	 {
	  std::string::size_type j=name.find("\"",i+6);
	  name=name.substr(i+6,j-(i+6));
	 }
	 else
	  name=RDK::sntoa(args.size()+1);

	}
	std::vector<char> arg;
	switch(LDecoder->PartType)
	{
	case mcptText:
	case mcptAttachment:
	  ProcessMimePart(LDecoder, LMsgEnd, arg);
	  args[name]=arg;
	break;
	case mcptIgnore:
	  delete LDecoder;
	  LDecoder = new TIdMessageDecoderMIME(0,"");
	break;
	case mcptEOF:
	  delete LDecoder;
	  LDecoder=0;
	  LMsgEnd = true;
	break;
	}
   } while( (LDecoder != 0) && !LMsgEnd);

  }
  __finally
  {
	  delete LDecoder;
  }

}

void __fastcall TUHttpServerFrame::IdHTTPServerCommandGet(TIdContext *AContext, TIdHTTPRequestInfo *ARequestInfo,
		  TIdHTTPResponseInfo *AResponseInfo)
{
 if(!IsHeaderMediaType(ARequestInfo->ContentType, "multipart/form-data"))
 {
  ParsingSimpleRequest(AContext, ARequestInfo, AResponseInfo, ParsedRequestArgs);
 }
 else // Загрузка файла
 {
  ParsingMultiRequest(AContext, ARequestInfo,  AResponseInfo, ParsedRequestArgs);
 }
}

RDK::UBitmap& TUHttpServerFrame::DecodeParamAsBitmap(std::vector<char> &param)
{
 if(param.empty())
 {
  TempUBitmap.Clear();
  return TempUBitmap;
 }
 MemStream->Clear();
/*
 for(int i=0;i<param.size()-1;i++)
  if(param[i]=='\r' && param[i+1]=='\n')
  {
   param.erase(param.begin()+i,param.begin()+i+1);
  }
*/
 MemStream->Write(&param[0],param.size());
 MemStream->Position=0;
 MemStream->SaveToFile("tmp.bmp");
 MemStream->Position=0;

 Image1->Picture->Bitmap->LoadFromStream(MemStream);
 Image1->Repaint();
 TempUBitmap<<Image1->Picture->Bitmap;

 return TempUBitmap;
}

//---------------------------------------------------------------------------
