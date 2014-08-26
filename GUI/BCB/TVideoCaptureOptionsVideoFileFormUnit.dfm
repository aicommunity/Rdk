object VideoCaptureOptionsVideoFileForm: TVideoCaptureOptionsVideoFileForm
  Left = 0
  Top = 0
  ClientHeight = 221
  ClientWidth = 554
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object VFCapturePanel: TPanel
    Left = 0
    Top = 0
    Width = 554
    Height = 221
    Align = alClient
    TabOrder = 0
    ExplicitWidth = 539
    ExplicitHeight = 203
    DesignSize = (
      554
      221)
    object VideoFileNameLabel: TLabel
      Left = 8
      Top = 3
      Width = 538
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video file name:'
      ExplicitWidth = 369
    end
    object VFNameEdit: TEdit
      Left = 8
      Top = 22
      Width = 453
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      ExplicitWidth = 438
    end
    object VFBrowseButton: TButton
      Left = 467
      Top = 20
      Width = 79
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Browse'
      TabOrder = 1
      OnClick = VFBrowseButtonClick
      ExplicitLeft = 452
    end
    object VideoTruncPathCheckBox: TCheckBox
      Left = 8
      Top = 89
      Width = 153
      Height = 17
      Caption = 'Truncate full path'
      TabOrder = 2
    end
    object RepeatVideoCheckBox: TCheckBox
      Left = 8
      Top = 112
      Width = 97
      Height = 17
      Caption = 'Repeat'
      TabOrder = 3
    end
    object ProcessAllFramesVideoCheckBox: TCheckBox
      Left = 8
      Top = 135
      Width = 113
      Height = 17
      Caption = 'Process all frames'
      TabOrder = 4
    end
    object FpsLabeledEdit: TLabeledEdit
      Left = 8
      Top = 62
      Width = 538
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 17
      EditLabel.Height = 13
      EditLabel.Caption = 'Fps'
      TabOrder = 5
      ExplicitWidth = 523
    end
  end
  object VideoOpenDialog: TOpenDialog
    DefaultExt = 'avi'
    Filter = #1042#1080#1076#1077#1086'|*.avi|'#1042#1089#1077' '#1092#1072#1081#1083#1099'|*.*'
    FilterIndex = 0
    Left = 456
    Top = 152
  end
end
