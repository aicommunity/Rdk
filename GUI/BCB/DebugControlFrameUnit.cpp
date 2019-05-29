//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "DebugControlFrameUnit.h"
#include "../../../Libraries/Rtv-StatisticLib/Core/RTVSaveDebugImages.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDebugControlFrame *DebugControlFrame;
const SELDIRHELP = 1000;
//---------------------------------------------------------------------------
__fastcall TDebugControlFrame::TDebugControlFrame(TComponent* Owner)
	: TUVisualControllerFrame(Owner)
{
	MyComponentsListForm=new TUComponentsListForm(this);
	ComponentLongName = "SaveDebugImages";
	ImgLTInputName = "ImgLT";
	ImgRTInputName = "ImgRT";
	ImgLBInputName = "ImgLB";
	ImgRBInputName = "ImgRB";
	SaveOnlyOneName = "SaveImagesOnce";
	SaveImagesName =  "SaveImages";
    SaveFolderPath = "";
}
//---------------------------------------------------------------------------
__fastcall TDebugControlFrame::~TDebugControlFrame(void)
{
 if(MyComponentsListForm)
  delete MyComponentsListForm;
}
void __fastcall TDebugControlFrame::ImgTLButtonClick(TObject *Sender)
{
 MyComponentsListForm->UpdateInterface(true);
 if(MyComponentsListForm->ShowIOSelect() != mrOk)
  return;
 ImgLTComponentLongName = MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 ImgLTComponentOutput = MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
}
//---------------------------------------------------------------------------
void TDebugControlFrame::AUpdateInterface(void)
{

  if((!ImgLTComponentLongName.empty())&&(!ImgLTComponentOutput.empty()))
	ImgTLEdit->Text=(UnicodeString)(ImgLTComponentLongName.c_str())+"->"+(UnicodeString)(ImgLTComponentOutput.c_str());
  if((!ImgLBComponentLongName.empty())&&(!ImgLBComponentOutput.empty()))
	ImgLBEdit->Text=(UnicodeString)(ImgLBComponentLongName.c_str())+"->"+(UnicodeString)(ImgLBComponentOutput.c_str());
  if((!ImgRTComponentLongName.empty())&&(!ImgRTComponentOutput.empty()))
	ImgTREdit->Text=(UnicodeString)(ImgRTComponentLongName.c_str())+"->"+(UnicodeString)(ImgRTComponentOutput.c_str());
  if((!ImgRBComponentLongName.empty())&&(!ImgRBComponentOutput.empty()))
	ImgRBEdit->Text=(UnicodeString)(ImgRBComponentLongName.c_str())+"->"+(UnicodeString)(ImgRBComponentOutput.c_str());
}
void __fastcall TDebugControlFrame::ImgTRButtonClick(TObject *Sender)
{
MyComponentsListForm->UpdateInterface(true);
 if(MyComponentsListForm->ShowIOSelect() != mrOk)
  return;
 ImgRTComponentLongName = MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 ImgRTComponentOutput = MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
 AUpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TDebugControlFrame::ImgLBButtonClick(TObject *Sender)
{
MyComponentsListForm->UpdateInterface(true);
 if(MyComponentsListForm->ShowIOSelect() != mrOk)
  return;
 ImgLBComponentLongName = MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 ImgLBComponentOutput = MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
 AUpdateInterface();
}
//---------------------------------------------------------------------------

void __fastcall TDebugControlFrame::ImgRBButtonClick(TObject *Sender)
{
MyComponentsListForm->UpdateInterface(true);
 if(MyComponentsListForm->ShowIOSelect() != mrOk)
  return;
 ImgRBComponentLongName = MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentLongName();
 ImgRBComponentOutput = MyComponentsListForm->ComponentsListFrame1->GetSelectedComponentOutput();
 AUpdateInterface();
}
//---------------------------------------------------------------------------

bool TDebugControlFrame::LinkImages2x2(int channel_id)
{
    //Нет пустых входов, так как выборочно не отцепить
	if((ImgTLEdit->Text!="")&&(ImgTREdit->Text!="")&&(ImgLBEdit->Text!="")&&(ImgRBEdit->Text!=""))
	{
		//Отцепляем старые выходы
		MModel_BreakAllComponentInputLinks(channel_id, ComponentLongName.c_str());
		MModel_CreateLinkByName(channel_id, ImgLTComponentLongName.c_str(), ImgLTComponentOutput.c_str(),ComponentLongName.c_str(), ImgLTInputName.c_str());
		MModel_CreateLinkByName(channel_id, ImgRTComponentLongName.c_str(), ImgRTComponentOutput.c_str(),ComponentLongName.c_str(), ImgRTInputName.c_str());
		MModel_CreateLinkByName(channel_id, ImgLBComponentLongName.c_str(), ImgLBComponentOutput.c_str(),ComponentLongName.c_str(), ImgLBInputName.c_str());
		MModel_CreateLinkByName(channel_id, ImgRBComponentLongName.c_str(), ImgRBComponentOutput.c_str(),ComponentLongName.c_str(), ImgRBInputName.c_str());
		if(EnableDebugImgSaveCheckBox->Checked)
		{
			MModel_SetComponentParameterValue(channel_id, ComponentLongName.c_str(), SaveImagesName.c_str(), "1");
		}
		else
		{
			MModel_SetComponentParameterValue(channel_id, ComponentLongName.c_str(), SaveImagesName.c_str(), "0");
		}
		if(SaveOnlyOnceCheckBox->Checked)
		{
			MModel_SetComponentParameterValue(channel_id, ComponentLongName.c_str(), SaveOnlyOneName.c_str(), "1");
		}
		else
		{
			MModel_SetComponentParameterValue(channel_id, ComponentLongName.c_str(), SaveOnlyOneName.c_str(), "0");
		}
	}
}

bool TDebugControlFrame::SaveImages2x2(int channel_id)
{
 RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock(channel_id);
 if(!model)
  return false;
 RDK::UEPtr<RTV::RTVSaveDebugImages> debug_imgs = RDK::dynamic_pointer_cast<RTV::RTVSaveDebugImages>(model->GetComponentL(ComponentLongName.c_str()));
 if(!debug_imgs)
  return false;

 debug_imgs->SaveDebugImages();
 return true;
}

bool TDebugControlFrame::SaveImages2x2path(int channel_id, const std::string path)
{
 RDK::UELockPtr<RDK::UContainer> model = RDK::GetModelLock(channel_id);
 if(!model)
  return false;
 RDK::UEPtr<RTV::RTVSaveDebugImages> debug_imgs = RDK::dynamic_pointer_cast<RTV::RTVSaveDebugImages>(model->GetComponentL(ComponentLongName.c_str()));
 if(!debug_imgs)
  return false;

 debug_imgs->SaveDebugImages(path);
 return true;
}

void __fastcall TDebugControlFrame::ApplyButtonClick(TObject *Sender)
{
	if(SaveImgCurrentChannelRadioButton->Checked)
	{
	 LinkImages2x2(Core_GetSelectedChannelIndex());
	}
	else if(SaveImgActiveChannelsRadioButton->Checked)
	{
	 int num_ch = Core_GetNumChannels();
	 for(int i=0; i<num_ch; i++)
	 {
		//RDK::UEPtr<RDK::UNet> camera = RDK::static_pointer_cast<RDK::UNet>(model->GetComponentL(componentLongName.c_str(), true));
		// if(!camera)
		//  return;
		std::string act = MModel_GetComponentParameterValue(i, "Pipeline1", "Activity");
		if(act==std::string("1"))
		{
		  LinkImages2x2(i);
        }
     }
	}

	//Как заставить систему сцепить вход на выход?
	//RDK_LIB_TYPE int RDK_CALL Model_CreateLinkByName(const char* stringid1, const char* item_property_name, const char* stringid2, const char* connector_property_name);
    //Model_CreateLi`nkByName(
}
//---------------------------------------------------------------------------

void __fastcall TDebugControlFrame::SaveOneImageButtonClick(TObject *Sender)
{
	if(SaveImgCurrentChannelRadioButton->Checked)
	{
	 SaveImages2x2(Core_GetSelectedChannelIndex());
	}
	else if(SaveImgActiveChannelsRadioButton->Checked)
	{
	 int num_ch = Core_GetNumChannels();
	 for(int i=0; i<num_ch; i++)
	 {
		//RDK::UEPtr<RDK::UNet> camera = RDK::static_pointer_cast<RDK::UNet>(model->GetComponentL(componentLongName.c_str(), true));
		// if(!camera)
		//  return;
		std::string act = MModel_GetComponentParameterValue(i, "Pipeline1", "Activity");
		if(act==std::string("1"))
		{
		  SaveImages2x2(i);
		}
	 }
	}
}
//---------------------------------------------------------------------------

void __fastcall TDebugControlFrame::MakeSnapshotButtonClick(TObject *Sender)
{
	bool doselect = SelectDirectory(SaveFolderPath, TSelectDirOpts() << sdAllowCreate << sdPerformCreate << sdPrompt,SELDIRHELP);

	if(doselect)
	{
		if(SnapshotOneRadioButton->Checked)
		{
		 if(SaveImageCheckBox->Checked)
		  SaveImages2x2path(Core_GetSelectedChannelIndex(), AnsiString(SaveFolderPath).c_str());
		  
		 //if(SaveSnapshotCheckBox)
				//Место для сохранения параметров модели
		}
		else if(SnapshotActiveRadioButton->Checked)
		{
		 int num_ch = Core_GetNumChannels();
		 for(int i=0; i<num_ch; i++)
		 {
			//RDK::UEPtr<RDK::UNet> camera = RDK::static_pointer_cast<RDK::UNet>(model->GetComponentL(componentLongName.c_str(), true));
			// if(!camera)
			//  return;
			std::string act = MModel_GetComponentParameterValue(i, "Pipeline1", "Activity");
			if(act==std::string("1"))
			{
			  if(SaveImageCheckBox->Checked)
			   SaveImages2x2path(i, AnsiString(SaveFolderPath).c_str());

			  //if(SaveSnapshotCheckBox)
				//Место для сохранения параметров модели
			}
		 }
		}
	}


}
//---------------------------------------------------------------------------
// Сохраняет параметры интерфейса в xml
void TDebugControlFrame::ASaveParameters(RDK::USerStorageXML &xml)
{
	xml.WriteBool("SnapshotOne",SnapshotOneRadioButton->Checked);    
	xml.WriteBool("SaveOne",SaveImgCurrentChannelRadioButton->Checked);
	xml.WriteBool("SaveSnapshotCheckBox",SaveSnapshotCheckBox->Checked);
	xml.WriteBool("SaveImageCheckBox",SaveImageCheckBox->Checked);
	xml.WriteBool("EnableDebugImgSaveCheckBox",EnableDebugImgSaveCheckBox->Checked);
	xml.WriteBool("SaveOnlyOnceCheckBox",SaveOnlyOnceCheckBox->Checked);

	xml.WriteString("ImgLTComponentLongName", ImgLTComponentLongName);
	xml.WriteString("ImgLTComponentOutput", ImgLTComponentOutput);

	xml.WriteString("ImgRTComponentLongName", ImgRTComponentLongName);
	xml.WriteString("ImgRTComponentOutput", ImgRTComponentOutput);

	xml.WriteString("ImgLBComponentLongName", ImgLBComponentLongName);
	xml.WriteString("ImgLBComponentOutput", ImgLBComponentOutput);

	xml.WriteString("ImgRBComponentLongName", ImgRBComponentLongName);
	xml.WriteString("ImgRBComponentOutput", ImgRBComponentOutput);
}

// Загружает параметры интерфейса из xml
void TDebugControlFrame::ALoadParameters(RDK::USerStorageXML &xml)
{
  bool sn1 = xml.ReadBool("SnapshotOne",SnapshotOneRadioButton->Checked);
  if(sn1)
  {
   SnapshotOneRadioButton->Checked = true;
  }
  else
  {
   SnapshotActiveRadioButton->Checked = true;
  }
  bool sn2 = xml.ReadBool("SaveOne",SaveImgCurrentChannelRadioButton->Checked);
  if(sn2)
  {
	SaveImgCurrentChannelRadioButton->Checked = true;
  }
  else
  {
   SaveImgActiveChannelsRadioButton->Checked = true;
  }
 SaveSnapshotCheckBox->Checked = xml.ReadBool("SaveSnapshotCheckBox",SaveSnapshotCheckBox->Checked);
 SaveImageCheckBox->Checked = xml.ReadBool("SaveImageCheckBox",SaveImageCheckBox->Checked);
 EnableDebugImgSaveCheckBox->Checked = xml.ReadBool("EnableDebugImgSaveCheckBox",EnableDebugImgSaveCheckBox->Checked);
 SaveOnlyOnceCheckBox->Checked = xml.ReadBool("SaveOnlyOnceCheckBox",SaveOnlyOnceCheckBox->Checked);

 ImgLTComponentLongName = xml.ReadString("ImgLTComponentLongName", ImgLTComponentLongName);
 ImgLTComponentOutput = xml.ReadString("ImgLTComponentOutput", ImgLTComponentOutput);

 ImgRTComponentLongName = xml.ReadString("ImgRTComponentLongName", ImgRTComponentLongName);
 ImgRTComponentOutput = xml.ReadString("ImgRTComponentOutput", ImgRTComponentOutput);

 ImgLBComponentLongName = xml.ReadString("ImgLBComponentLongName", ImgLBComponentLongName);
 ImgLBComponentOutput = xml.ReadString("ImgLBComponentOutput", ImgLBComponentOutput);

 ImgRBComponentLongName = xml.ReadString("ImgRBComponentLongName", ImgRBComponentLongName);
 ImgRBComponentOutput = xml.ReadString("ImgRBComponentOutput", ImgRBComponentOutput);
	
}
