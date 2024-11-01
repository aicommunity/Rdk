object UImagesForm: TUImagesForm
  Left = 567
  Top = 277
  Caption = 'Images View Window'
  ClientHeight = 609
  ClientWidth = 910
  Color = clBtnFace
  DefaultMonitor = dmDesktop
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  PixelsPerInch = 120
  TextHeight = 17
  inline ImagesFrame: TUImagesFrame
    Left = 0
    Top = 0
    Width = 910
    Height = 609
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 910
    ExplicitHeight = 609
    inherited DrawGrid: TDrawGrid
      Width = 910
      Height = 588
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Options = [goVertLine, goHorzLine, goDrawFocusSelected, goRowSizing, goColSizing]
      ExplicitWidth = 910
      ExplicitHeight = 588
    end
    inherited ScrollBox1: TScrollBox
      Width = 910
      Height = 588
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      ExplicitWidth = 910
      ExplicitHeight = 588
      inherited FullImage: TImage
        Left = 296
        Top = 215
        Width = 132
        Height = 131
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ExplicitLeft = 296
        ExplicitTop = 215
        ExplicitWidth = 132
        ExplicitHeight = 131
      end
    end
    inherited Panel1: TPanel
      Top = 588
      Width = 910
      Height = 21
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      ExplicitTop = 588
      ExplicitWidth = 910
      ExplicitHeight = 21
      inherited Labelshow: TLabel
        Height = 19
      end
      inherited Labelsize: TLabel
        Left = 601
        Height = 19
      end
      inherited ShowLegendCheckBox: TCheckBox
        Width = 110
        Height = 19
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ExplicitWidth = 110
        ExplicitHeight = 19
      end
      inherited ShowHistogramCheckBox: TCheckBox
        Left = 149
        Width = 130
        Height = 19
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ExplicitLeft = 149
        ExplicitWidth = 130
        ExplicitHeight = 19
      end
      inherited TiledSizeRadioButton: TRadioButton
        Left = 734
        Width = 82
        Height = 19
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ExplicitLeft = 734
        ExplicitWidth = 82
        ExplicitHeight = 19
      end
      inherited OriginalSizeRadioButton: TRadioButton
        Left = 632
        Width = 102
        Height = 19
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ExplicitLeft = 632
        ExplicitWidth = 102
        ExplicitHeight = 19
      end
      inherited ShowInfoCheckBox: TCheckBox
        Left = 279
        Width = 100
        Height = 19
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ExplicitLeft = 279
        ExplicitWidth = 100
        ExplicitHeight = 19
      end
      inherited IndChannelsCheckBox: TCheckBox
        Left = 379
        Width = 122
        Height = 19
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ExplicitLeft = 379
        ExplicitWidth = 122
        ExplicitHeight = 19
      end
      inherited ProportionalSizeRadioButton: TRadioButton
        Left = 816
        Width = 93
        Height = 19
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ExplicitLeft = 816
        ExplicitWidth = 93
        ExplicitHeight = 19
      end
      inherited CenterImageCheckBox: TCheckBox
        Left = 501
        Width = 68
        Height = 19
        ExplicitLeft = 501
        ExplicitWidth = 68
        ExplicitHeight = 19
      end
    end
  end
end
