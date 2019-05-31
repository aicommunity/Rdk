//---------------------------------------------------------------------------

#ifndef DebugControlFrameUnitH
#define DebugControlFrameUnitH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <FileCtrl.hpp>
#include "TUVisualControllerFrameUnit.h"
#include "UComponentsListFormUnit.h"
#include <Vcl.ExtCtrls.hpp>
#include <vector>
//---------------------------------------------------------------------------
class TDebugControlFrame : public TUVisualControllerFrame
{
__published:	// IDE-managed Components
	TGroupBox *DebugImagesGroupBox;
	TCheckBox *EnableDebugImgSaveCheckBox;
	TEdit *ImgTLEdit;
	TButton *ImgTLButton;
	TLabel *ImgTLLabel;
	TEdit *ImgTREdit;
	TLabel *ImgTRLabel;
	TButton *ImgTRButton;
	TEdit *ImgRBEdit;
	TLabel *ImgRBLabel;
	TButton *ImgRBButton;
	TButton *ImgLBButton;
	TEdit *ImgLBEdit;
	TLabel *ImgLBLabel;
	TButton *ApplyButton;
	TButton *SaveOneImageButton;
	TRadioButton *SaveImgCurrentChannelRadioButton;
	TRadioButton *SaveImgActiveChannelsRadioButton;
	TCheckBox *SaveOnlyOnceCheckBox;
	TPanel *TopPanel;
	TGroupBox *SnapshotSettingsGroupBox;
	TButton *MakeSnapshotButton;
	TCheckBox *SaveSnapshotCheckBox;
	TCheckBox *SaveImageCheckBox;
	TRadioButton *SnapshotOneRadioButton;
	TRadioButton *SnapshotActiveRadioButton;
	TCheckBox *EnableDebugModulesCheckBox;
	void __fastcall ImgTLButtonClick(TObject *Sender);
	void __fastcall ImgTRButtonClick(TObject *Sender);
	void __fastcall ImgLBButtonClick(TObject *Sender);
	void __fastcall ImgRBButtonClick(TObject *Sender);
	void __fastcall ApplyButtonClick(TObject *Sender);
	void __fastcall SaveOneImageButtonClick(TObject *Sender);
	void __fastcall MakeSnapshotButtonClick(TObject *Sender);
	void __fastcall EnableDebugModulesCheckBoxClick(TObject *Sender);
	void __fastcall EnableDebugModulesCheckBoxKeyPress(TObject *Sender, System::WideChar &Key);

private:	// User declarations
	bool LinkImages2x2(int channel_id);
	bool SaveImages2x2(int channel_id);
	bool SaveImages2x2path(int channel_id, const std::string path);
	//��������� �� ���� �������� "0" ��� ���������� � "1" ��� ���������
	//������ ���������� ��������� �� �������� �������
	bool SetDebugAll(const std::string &debug_value);
	void SwitchEnableElements(bool enable);
	// ��������� �� ����� ������ ����������-���������
	TUComponentsListForm *MyComponentsListForm;

	std::string ImgLTComponentLongName;
	std::string ImgLTComponentOutput;

	std::string ImgRTComponentLongName;
	std::string ImgRTComponentOutput;

	std::string ImgLBComponentLongName;
	std::string ImgLBComponentOutput;

	std::string ImgRBComponentLongName;
	std::string ImgRBComponentOutput;


	//
	std::string ComponentLongName;
	std::string ImgLTInputName;
	std::string ImgLBInputName;
	std::string ImgRTInputName;
	std::string ImgRBInputName;
	std::string SaveOnlyOneName;
	std::string SaveImagesName;

	String SaveFolderPath;


public:		// User declarations
	__fastcall TDebugControlFrame(TComponent* Owner);
	__fastcall ~TDebugControlFrame(void);
	void AUpdateInterface(void);
	// ��������� ��������� ���������� � xml
	void ASaveParameters(RDK::USerStorageXML &xml);

	// ��������� ��������� ���������� �� xml
	void ALoadParameters(RDK::USerStorageXML &xml);

};
//---------------------------------------------------------------------------
extern PACKAGE TDebugControlFrame *DebugControlFrame;
//---------------------------------------------------------------------------
#endif
