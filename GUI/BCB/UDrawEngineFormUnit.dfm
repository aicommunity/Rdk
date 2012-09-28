object UDrawEngineForm: TUDrawEngineForm
  Left = 0
  Top = 0
  Caption = 'UDrawEngineForm'
  ClientHeight = 250
  ClientWidth = 750
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
  object Splitter1: TSplitter
    Left = 344
    Top = 0
    Width = 16
    Height = 250
    Align = alRight
  end
  object Panel1: TPanel
    Left = 360
    Top = 0
    Width = 390
    Height = 250
    Align = alRight
    TabOrder = 0
    object RichEdit1: TRichEdit
      Left = 1
      Top = 1
      Width = 388
      Height = 248
      Align = alClient
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      Lines.Strings = (
        'RichEdit1')
      ParentFont = False
      TabOrder = 0
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 344
    Height = 250
    Align = alClient
    TabOrder = 1
    object ScrollBox: TScrollBox
      Left = 1
      Top = 1
      Width = 342
      Height = 248
      Align = alClient
      TabOrder = 0
      object Image: TImage
        Left = 0
        Top = 0
        Width = 305
        Height = 241
        OnMouseDown = ImageMouseDown
        OnMouseMove = ImageMouseMove
        OnMouseUp = ImageMouseUp
      end
    end
  end
end
