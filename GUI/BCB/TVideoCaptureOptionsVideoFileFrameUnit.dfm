object VideoCaptureOptionsVideoFileFrame: TVideoCaptureOptionsVideoFileFrame
  Left = 0
  Top = 0
  Width = 555
  Height = 241
  TabOrder = 0
  object VFCapturePanel: TPanel
    Left = 0
    Top = 0
    Width = 555
    Height = 241
    Align = alClient
    TabOrder = 0
    ExplicitTop = -25
    ExplicitWidth = 445
    ExplicitHeight = 266
    DesignSize = (
      555
      241)
    object VideoFileNameLabel: TLabel
      Left = 8
      Top = 3
      Width = 539
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video file name:'
      ExplicitWidth = 369
    end
    object VFNameEdit: TEdit
      Left = 8
      Top = 22
      Width = 454
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      ExplicitWidth = 344
    end
    object VFBrowseButton: TButton
      Left = 468
      Top = 20
      Width = 79
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Browse'
      TabOrder = 1
      ExplicitLeft = 358
    end
    object VideoTruncPathCheckBox: TCheckBox
      Left = 8
      Top = 49
      Width = 153
      Height = 17
      Caption = 'Truncate full path'
      TabOrder = 2
    end
    object RepeatVideoCheckBox: TCheckBox
      Left = 8
      Top = 72
      Width = 97
      Height = 17
      Caption = 'Repeat'
      TabOrder = 3
    end
    object ProcessAllFramesVideoCheckBox: TCheckBox
      Left = 8
      Top = 95
      Width = 113
      Height = 17
      Caption = 'Process all frames'
      TabOrder = 4
    end
  end
end
