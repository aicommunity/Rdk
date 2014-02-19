object VideoCaptureOptionsSharedMemoryFrame: TVideoCaptureOptionsSharedMemoryFrame
  Left = 0
  Top = 0
  Width = 464
  Height = 200
  TabOrder = 0
  DesignSize = (
    464
    200)
  object Label11: TLabel
    Left = 8
    Top = 3
    Width = 177
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Pipe index:'
  end
  object Label12: TLabel
    Left = 8
    Top = 51
    Width = 177
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Pipe uid:'
  end
  object PipeIndexEdit: TEdit
    Left = 8
    Top = 20
    Width = 429
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    Text = '0'
  end
  object PipeUidEdit: TEdit
    Left = 8
    Top = 68
    Width = 429
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 1
    Text = 'USharedMemory0'
  end
end
