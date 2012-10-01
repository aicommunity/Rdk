object UDrawEngineForm: TUDrawEngineForm
  Left = 0
  Top = 0
  Caption = 'Model Editor'
  ClientHeight = 369
  ClientWidth = 749
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 504
    Top = 0
    Width = 16
    Height = 369
    Align = alRight
    ExplicitLeft = 339
  end
  object Panel1: TPanel
    Left = 520
    Top = 0
    Width = 229
    Height = 369
    Align = alRight
    TabOrder = 0
    inline UClassesListFrame: TUClassesListFrame
      Left = 1
      Top = 1
      Width = 227
      Height = 367
      Align = alClient
      DoubleBuffered = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 0
      ExplicitLeft = 1
      ExplicitTop = 1
      ExplicitWidth = 227
      ExplicitHeight = 367
      inherited StringGrid: TStringGrid
        Width = 227
        Height = 367
        OnMouseMove = UClassesListFrameStringGridMouseMove
        OnMouseUp = UClassesListFrameStringGridMouseUp
        ExplicitWidth = 227
        ExplicitHeight = 367
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 504
    Height = 369
    Align = alClient
    TabOrder = 1
    object ScrollBox: TScrollBox
      Left = 1
      Top = 1
      Width = 502
      Height = 367
      Align = alClient
      TabOrder = 0
      OnResize = ScrollBoxResize
      object Image: TImage
        Left = 0
        Top = 0
        Width = 305
        Height = 241
        OnDragDrop = ImageDragDrop
        OnDragOver = ImageDragOver
        OnMouseDown = ImageMouseDown
        OnMouseMove = ImageMouseMove
        OnMouseUp = ImageMouseUp
      end
    end
  end
  object PopupMenu: TPopupMenu
    Left = 384
    Top = 176
    object Breakinputlink1: TMenuItem
      Caption = 'Break input link'
      OnClick = Breakinputlink1Click
    end
  end
end
