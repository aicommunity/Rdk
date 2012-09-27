object UDrawEngineForm: TUDrawEngineForm
  Left = 0
  Top = 0
  Caption = 'UDrawEngineForm'
  ClientHeight = 250
  ClientWidth = 482
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 315
    Top = 0
    Width = 167
    Height = 250
    Align = alRight
    TabOrder = 0
    ExplicitLeft = 360
    ExplicitHeight = 243
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 315
    Height = 250
    Align = alClient
    TabOrder = 1
    ExplicitWidth = 625
    ExplicitHeight = 522
    object ScrollBox: TScrollBox
      Left = 1
      Top = 1
      Width = 313
      Height = 248
      Align = alClient
      TabOrder = 0
      ExplicitLeft = 176
      ExplicitTop = 64
      ExplicitHeight = 257
      object Image: TImage
        Left = 0
        Top = 0
        Width = 305
        Height = 241
        OnMouseDown = ImageMouseDown
      end
    end
  end
end
