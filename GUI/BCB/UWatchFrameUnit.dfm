inherited UWatchFrame: TUWatchFrame
  Width = 656
  Height = 469
  TabStop = True
  ExplicitWidth = 656
  ExplicitHeight = 469
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
    object AddWatch1: TMenuItem
      Caption = 'Add Time-Y Watch'
      OnClick = AddWatch1Click
    end
    object AddXPulseWatch1: TMenuItem
      Caption = 'Add X-Pulse Watch'
      OnClick = AddXPulseWatch1Click
    end
    object AddXYWatch1: TMenuItem
      Caption = 'Add XY Watch'
      Enabled = False
    end
    object N1: TMenuItem
      Caption = 'Watch Control'
      OnClick = N1Click
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object bmp1: TMenuItem
      Caption = 'Save Picture'
      OnClick = bmp1Click
    end
  end
end
