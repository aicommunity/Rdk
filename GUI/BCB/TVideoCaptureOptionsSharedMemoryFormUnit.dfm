object VideoCaptureOptionsSharedMemoryForm: TVideoCaptureOptionsSharedMemoryForm
  Left = 0
  Top = 0
  ClientHeight = 162
  ClientWidth = 448
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  OnCreate = FormCreate
  DesignSize = (
    448
    162)
  PixelsPerInch = 96
  TextHeight = 13
  object Label11: TLabel
    Left = 8
    Top = 3
    Width = 161
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Pipe index:'
    ExplicitWidth = 177
  end
  object Label12: TLabel
    Left = 8
    Top = 51
    Width = 161
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Pipe uid:'
    ExplicitWidth = 177
  end
  object PipeIndexEdit: TEdit
    Left = 8
    Top = 20
    Width = 432
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    Text = '0'
  end
  object PipeUidEdit: TEdit
    Left = 8
    Top = 68
    Width = 432
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 1
    Text = 'USharedMemory0'
  end
end
