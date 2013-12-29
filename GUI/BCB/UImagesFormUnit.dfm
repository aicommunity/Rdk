object UImagesForm: TUImagesForm
  Left = 567
  Top = 277
  Caption = 'UImagesForm'
  ClientHeight = 487
  ClientWidth = 728
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
  inline ImagesFrame: TUImagesFrame
    Left = 0
    Top = 0
    Width = 728
    Height = 487
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 728
    ExplicitHeight = 487
    inherited DrawGrid: TDrawGrid
      Width = 728
      Height = 470
      Options = [goVertLine, goHorzLine, goDrawFocusSelected, goRowSizing, goColSizing]
      ExplicitWidth = 728
      ExplicitHeight = 487
    end
    inherited ScrollBox1: TScrollBox
      Width = 728
      Height = 470
      ExplicitWidth = 728
      ExplicitHeight = 470
    end
    inherited Panel1: TPanel
      Top = 470
      Width = 728
      inherited ShowLegendCheckBox: TCheckBox
        Top = 470
        Width = 728
        ExplicitTop = 470
        ExplicitWidth = 728
      end
    end
  end
end
