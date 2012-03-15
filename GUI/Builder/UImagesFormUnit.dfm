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
    TabOrder = 0
    ExplicitWidth = 629
    ExplicitHeight = 562
    inherited DrawGrid: TDrawGrid
      Width = 728
      Height = 487
      Options = [goVertLine, goHorzLine, goDrawFocusSelected, goRowSizing, goColSizing]
      ExplicitWidth = 649
      ExplicitHeight = 519
    end
  end
end
