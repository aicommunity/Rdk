object VideoCaptureOptionsHttpServerForm: TVideoCaptureOptionsHttpServerForm
  Left = 0
  Top = 0
  ClientHeight = 169
  ClientWidth = 479
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  OnCreate = FormCreate
  DesignSize = (
    479
    169)
  PixelsPerInch = 96
  TextHeight = 13
  object Label10: TLabel
    Left = 8
    Top = 3
    Width = 161
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Listen port:'
    ExplicitWidth = 177
  end
  object ListerPortEdit: TEdit
    Left = 8
    Top = 20
    Width = 463
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    Text = '80'
  end
end
