object ApplicationOptionsForm: TApplicationOptionsForm
  Left = 0
  Top = 0
  Caption = 'Application Options'
  ClientHeight = 321
  ClientWidth = 527
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 527
    Height = 280
    ActivePage = GeneralTabSheet
    Align = alClient
    TabOrder = 0
    object GeneralTabSheet: TTabSheet
      Caption = 'General'
      inline ApplicationOptionsFrame: TApplicationOptionsFrame
        Left = 0
        Top = 0
        Width = 519
        Height = 252
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        ExplicitWidth = 519
        ExplicitHeight = 252
        inherited ApplicationNameLabeledEdit: TLabeledEdit
          Width = 513
          ExplicitWidth = 513
        end
        inherited MainFormInternalNameLabeledEdit: TLabeledEdit
          Width = 432
          ExplicitWidth = 432
        end
        inherited AutoStartProjectNameLabeledEdit: TLabeledEdit
          Width = 432
          ExplicitWidth = 432
        end
        inherited BrowseAutoLoadingProjectNameButton: TButton
          Left = 441
          ExplicitLeft = 441
        end
        inherited BrowseMainFormNameButton: TButton
          Left = 441
          ExplicitLeft = 441
        end
        inherited HideAdminFormCheckBox: TCheckBox
          Anchors = [akLeft, akTop, akRight]
        end
        inherited DisableAdminFormCheckBox: TCheckBox
          Width = 405
          ExplicitWidth = 405
        end
      end
    end
    object ServerTabSheet: TTabSheet
      Caption = 'Server'
      ImageIndex = 1
      inline ServerOptionsFrame: TServerOptionsFrame
        Left = 0
        Top = 0
        Width = 519
        Height = 252
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        ExplicitWidth = 519
        ExplicitHeight = 252
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 280
    Width = 527
    Height = 41
    Align = alBottom
    TabOrder = 1
    object OKButton: TButton
      Left = 284
      Top = 6
      Width = 75
      Height = 25
      Caption = 'OK'
      Default = True
      TabOrder = 0
      OnClick = OKButtonClick
    end
    object CancelButton: TButton
      Left = 365
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Cancel'
      TabOrder = 1
      OnClick = CancelButtonClick
    end
    object RestoreButton: TButton
      Left = 446
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Restore'
      TabOrder = 2
      OnClick = RestoreButtonClick
    end
  end
end
