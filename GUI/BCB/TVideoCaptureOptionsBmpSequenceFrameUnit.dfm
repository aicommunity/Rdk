object VideoCaptureOptionsBmpSequenceFrame: TVideoCaptureOptionsBmpSequenceFrame
  Left = 0
  Top = 0
  Width = 526
  Height = 242
  TabOrder = 0
  DesignSize = (
    526
    242)
  object Label7: TLabel
    Left = 8
    Top = 3
    Width = 429
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Image file name:'
    ExplicitWidth = 369
  end
  object ImageSequencePathEdit: TEdit
    Left = 8
    Top = 22
    Width = 344
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
  end
  object ImageSequencePathCheckBox: TCheckBox
    Left = 8
    Top = 49
    Width = 153
    Height = 17
    Caption = 'Truncate full path'
    TabOrder = 1
  end
  object ImageSequencePathBrowseButton: TButton
    Left = 358
    Top = 20
    Width = 79
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Browse'
    TabOrder = 2
  end
  object RepeatSequenceCheckBox: TCheckBox
    Left = 8
    Top = 72
    Width = 97
    Height = 17
    Caption = 'Repeat'
    TabOrder = 3
  end
  object ImageSequenceFpsLabeledEdit: TLabeledEdit
    Left = 8
    Top = 111
    Width = 49
    Height = 21
    EditLabel.Width = 22
    EditLabel.Height = 13
    EditLabel.Caption = 'FPS:'
    TabOrder = 4
    Text = '25.0'
  end
end
