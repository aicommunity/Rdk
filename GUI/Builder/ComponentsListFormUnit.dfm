object ComponentsListForm: TComponentsListForm
  Left = 857
  Top = 64
  Caption = 'Components List'
  ClientHeight = 379
  ClientWidth = 565
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  PixelsPerInch = 96
  TextHeight = 13
  inline ComponentsListFrame1: TComponentsListFrame
    Left = 0
    Top = 0
    Width = 565
    Height = 379
    Align = alClient
    TabOrder = 0
    ExplicitLeft = 109
    ExplicitTop = -188
    inherited StringGrid: TStringGrid
      Width = 565
      Height = 345
    end
    inherited HeaderControl: THeaderControl
      Width = 565
    end
  end
end
