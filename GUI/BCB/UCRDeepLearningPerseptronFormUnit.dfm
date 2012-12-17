object UCRDeepLearningPerseptronForm: TUCRDeepLearningPerseptronForm
  Left = 0
  Top = 0
  Caption = 'UCRDeepLearningPerseptronForm'
  ClientHeight = 382
  ClientWidth = 345
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
  object Label1: TLabel
    Left = 104
    Top = 13
    Width = 82
    Height = 13
    Caption = 'Perseptron Mode'
  end
  object Splitter1: TSplitter
    Left = 0
    Top = 241
    Width = 345
    Height = 15
    Cursor = crVSplit
    Align = alBottom
    ExplicitTop = 89
  end
  object ResetTrainingButton: TButton
    Left = 8
    Top = 8
    Width = 81
    Height = 25
    Caption = 'Reset Training'
    TabOrder = 0
    OnClick = ResetTrainingButtonClick
  end
  object ModeComboBox: TComboBox
    Left = 192
    Top = 10
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 1
    Text = 'Recognition'
    OnSelect = ModeComboBoxSelect
    Items.Strings = (
      'Recognition'
      'Training Step 1'
      'Training Step 2')
  end
  object Button1: TButton
    Left = 8
    Top = 48
    Width = 81
    Height = 25
    Caption = 'Add Layer'
    TabOrder = 2
    OnClick = Button1Click
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 256
    Width = 345
    Height = 126
    Align = alBottom
    Caption = 'GroupBox1'
    TabOrder = 3
    object Image: TImage
      Left = 2
      Top = 36
      Width = 341
      Height = 88
      Align = alClient
      ExplicitLeft = 136
      ExplicitTop = 56
      ExplicitWidth = 105
      ExplicitHeight = 105
    end
    object LayersComboBox: TComboBox
      Left = 2
      Top = 15
      Width = 341
      Height = 21
      Align = alTop
      Style = csDropDownList
      TabOrder = 0
    end
  end
end
