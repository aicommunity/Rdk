object VideoCaptureOptionsBmpSequenceForm: TVideoCaptureOptionsBmpSequenceForm
  Left = 0
  Top = 0
  ClientHeight = 267
  ClientWidth = 667
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  OnCreate = FormCreate
  DesignSize = (
    667
    267)
  PixelsPerInch = 120
  TextHeight = 17
  object Label7: TLabel
    Left = 10
    Top = 4
    Width = 541
    Height = 17
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Image file name:'
  end
  object ImageSequencePathEdit: TEdit
    Left = 10
    Top = 29
    Width = 541
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
  end
  object ImageSequencePathCheckBox: TCheckBox
    Left = 10
    Top = 64
    Width = 201
    Height = 22
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Truncate full path'
    TabOrder = 1
  end
  object ImageSequencePathBrowseButton: TButton
    Left = 558
    Top = 26
    Width = 104
    Height = 33
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akTop, akRight]
    Caption = 'Browse'
    TabOrder = 2
    OnClick = ImageSequencePathBrowseButtonClick
  end
  object RepeatSequenceCheckBox: TCheckBox
    Left = 10
    Top = 94
    Width = 127
    Height = 22
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Repeat'
    TabOrder = 3
  end
  object ImageSequenceFpsLabeledEdit: TLabeledEdit
    Left = 10
    Top = 145
    Width = 65
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    EditLabel.Width = 28
    EditLabel.Height = 17
    EditLabel.Margins.Left = 4
    EditLabel.Margins.Top = 4
    EditLabel.Margins.Right = 4
    EditLabel.Margins.Bottom = 4
    EditLabel.Caption = 'FPS:'
    TabOrder = 4
    Text = '25.0'
  end
end
