object VideoCaptureOptionsBmpFrame: TVideoCaptureOptionsBmpFrame
  Left = 0
  Top = 0
  Width = 480
  Height = 236
  TabOrder = 0
  DesignSize = (
    480
    236)
  object Label3: TLabel
    Left = 8
    Top = 3
    Width = 429
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Image file name:'
    ExplicitWidth = 369
  end
  object PictureTruncPathCheckBox: TCheckBox
    Left = 8
    Top = 49
    Width = 153
    Height = 17
    Caption = 'Truncate full path'
    TabOrder = 0
  end
  object PictureFileFpsLabeledEdit: TLabeledEdit
    Left = 8
    Top = 87
    Width = 49
    Height = 21
    EditLabel.Width = 22
    EditLabel.Height = 13
    EditLabel.Caption = 'FPS:'
    TabOrder = 1
    Text = '25.0'
  end
  object OpenImageFileButton: TButton
    Left = 358
    Top = 20
    Width = 79
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Browse'
    TabOrder = 2
  end
  object ImageFileNameEdit: TEdit
    Left = 8
    Top = 22
    Width = 344
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 3
  end
end
