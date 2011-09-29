object WatchFrame: TWatchFrame
  Left = 0
  Top = 0
  Width = 656
  Height = 469
  TabOrder = 0
  TabStop = True
  object Chart1: TChart
    Left = 0
    Top = 0
    Width = 656
    Height = 469
    BackWall.Brush.Color = clWhite
    BackWall.Brush.Style = bsClear
    Legend.Alignment = laBottom
    Legend.CheckBoxesStyle = cbsRadio
    Legend.ColorWidth = 50
    Legend.DividingLines.SmallDots = True
    Legend.Inverted = True
    Legend.LeftPercent = 80
    Legend.LegendStyle = lsSeries
    Legend.PositionUnits = muPercent
    Legend.Symbol.Pen.Width = 5
    Legend.Symbol.Width = 50
    Legend.TopPercent = 10
    Legend.TopPos = 6
    MarginBottom = 0
    MarginTop = 0
    Title.Text.Strings = (
      ' ')
    DepthAxis.Automatic = False
    DepthAxis.AutomaticMaximum = False
    DepthAxis.AutomaticMinimum = False
    DepthAxis.Maximum = -0.700000000000000600
    DepthAxis.Minimum = -1.700000000000001000
    LeftAxis.Title.Angle = 0
    View3D = False
    Align = alClient
    Color = clWhite
    PopupMenu = PopupMenu
    TabOrder = 0
    ColorPaletteIndex = 13
  end
  object SavePictureDialog: TSavePictureDialog
    Filter = 'Bitmaps (*.bmp)|*.bmp|Enhanced Metafiles (*.emf)|*.emf'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofEnableSizing]
    Title = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1075#1088#1072#1092#1080#1082' '#1082#1072#1082
    Left = 16
    Top = 16
  end
  object PopupMenu: TPopupMenu
    Left = 72
    Top = 16
    object N1: TMenuItem
      Caption = #1059#1087#1088#1072#1074#1083#1077#1085#1080#1077
      OnClick = N1Click
    end
    object bmp1: TMenuItem
      Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1075#1088#1072#1092#1080#1082
      OnClick = bmp1Click
    end
  end
end
