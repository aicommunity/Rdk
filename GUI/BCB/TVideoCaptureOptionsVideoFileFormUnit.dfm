object VideoCaptureOptionsVideoFileForm: TVideoCaptureOptionsVideoFileForm
  Left = 0
  Top = 0
  ClientHeight = 289
  ClientWidth = 724
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 17
  object VFCapturePanel: TPanel
    Left = 0
    Top = 0
    Width = 724
    Height = 289
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    TabOrder = 0
    DesignSize = (
      724
      289)
    object VideoFileNameLabel: TLabel
      Left = 10
      Top = 4
      Width = 704
      Height = 17
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video file name:'
    end
    object VFNameEdit: TEdit
      Left = 10
      Top = 29
      Width = 593
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
    end
    object VFBrowseButton: TButton
      Left = 611
      Top = 26
      Width = 103
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akTop, akRight]
      Caption = 'Browse'
      TabOrder = 1
      OnClick = VFBrowseButtonClick
    end
    object VideoTruncPathCheckBox: TCheckBox
      Left = 10
      Top = 116
      Width = 201
      Height = 23
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Truncate full path'
      TabOrder = 2
    end
    object RepeatVideoCheckBox: TCheckBox
      Left = 10
      Top = 146
      Width = 127
      Height = 23
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Repeat'
      TabOrder = 3
    end
    object ProcessAllFramesVideoCheckBox: TCheckBox
      Left = 10
      Top = 177
      Width = 148
      Height = 22
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Process all frames'
      TabOrder = 4
    end
    object FpsLabeledEdit: TLabeledEdit
      Left = 10
      Top = 81
      Width = 704
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 21
      EditLabel.Height = 17
      EditLabel.Margins.Left = 4
      EditLabel.Margins.Top = 4
      EditLabel.Margins.Right = 4
      EditLabel.Margins.Bottom = 4
      EditLabel.Caption = 'Fps'
      TabOrder = 5
    end
    object DesiredWidthLabeledEdit: TLabeledEdit
      Left = 164
      Top = 226
      Width = 47
      Height = 25
      EditLabel.Width = 37
      EditLabel.Height = 17
      EditLabel.Caption = 'Width'
      TabOrder = 6
      Text = '640'
    end
    object DesiredHeightLabeledEdit: TLabeledEdit
      Left = 217
      Top = 226
      Width = 48
      Height = 25
      EditLabel.Width = 39
      EditLabel.Height = 17
      EditLabel.Caption = 'Height'
      TabOrder = 7
      Text = '480'
    end
    object DesiredResFlagCheckBox: TCheckBox
      Left = 10
      Top = 206
      Width = 148
      Height = 17
      Caption = 'Use user resolution'
      TabOrder = 8
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
