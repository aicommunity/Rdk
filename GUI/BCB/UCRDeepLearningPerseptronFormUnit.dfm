object UCRDeepLearningPerseptronForm: TUCRDeepLearningPerseptronForm
  Left = 0
  Top = 0
  Caption = 'UCRDeepLearningPerseptronForm'
  ClientHeight = 430
  ClientWidth = 527
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 104
    Top = 13
    Width = 82
    Height = 13
    Caption = 'Perseptron Mode'
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
end
