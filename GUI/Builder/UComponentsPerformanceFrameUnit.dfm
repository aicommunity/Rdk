object UComponentsPerformanceFrame: TUComponentsPerformanceFrame
  Left = 0
  Top = 0
  Width = 454
  Height = 326
  TabOrder = 0
  object Chart: TChart
    Left = 0
    Top = 0
    Width = 454
    Height = 326
    BackWall.Brush.Gradient.Direction = gdBottomTop
    BackWall.Brush.Gradient.EndColor = clWhite
    BackWall.Brush.Gradient.StartColor = 15395562
    BackWall.Brush.Gradient.Visible = True
    BackWall.Transparent = False
    Foot.Font.Name = 'Verdana'
    Gradient.Direction = gdBottomTop
    Gradient.EndColor = clWhite
    Gradient.MidColor = 15395562
    Gradient.StartColor = 15395562
    Gradient.Visible = True
    LeftWall.Color = 14745599
    Legend.Alignment = laTop
    Legend.Font.Name = 'Verdana'
    RightWall.Color = 14745599
    SubFoot.Font.Name = 'Verdana'
    SubTitle.Font.Name = 'Verdana'
    Title.Font.Name = 'Verdana'
    BottomAxis.Axis.Color = 4210752
    BottomAxis.Grid.Color = 11119017
    BottomAxis.LabelsAlternate = True
    BottomAxis.LabelsFont.Name = 'Verdana'
    BottomAxis.LabelsMultiLine = True
    BottomAxis.LabelsSeparation = 0
    BottomAxis.TicksInner.Color = 11119017
    BottomAxis.Title.Font.Name = 'Verdana'
    DepthAxis.Axis.Color = 4210752
    DepthAxis.Grid.Color = 11119017
    DepthAxis.LabelsFont.Name = 'Verdana'
    DepthAxis.TicksInner.Color = 11119017
    DepthAxis.Title.Font.Name = 'Verdana'
    DepthTopAxis.Axis.Color = 4210752
    DepthTopAxis.Grid.Color = 11119017
    DepthTopAxis.LabelsFont.Name = 'Verdana'
    DepthTopAxis.TicksInner.Color = 11119017
    DepthTopAxis.Title.Font.Name = 'Verdana'
    LeftAxis.Axis.Color = 4210752
    LeftAxis.Grid.Color = 11119017
    LeftAxis.LabelsFont.Name = 'Verdana'
    LeftAxis.TicksInner.Color = 11119017
    LeftAxis.Title.Font.Name = 'Verdana'
    RightAxis.Axis.Color = 4210752
    RightAxis.Grid.Color = 11119017
    RightAxis.LabelsFont.Name = 'Verdana'
    RightAxis.TicksInner.Color = 11119017
    RightAxis.Title.Font.Name = 'Verdana'
    TopAxis.Axis.Color = 4210752
    TopAxis.Grid.Color = 11119017
    TopAxis.LabelsAlternate = True
    TopAxis.LabelsFont.Name = 'Verdana'
    TopAxis.LabelsMultiLine = True
    TopAxis.TicksInner.Color = 11119017
    TopAxis.Title.Font.Name = 'Verdana'
    View3D = False
    Align = alClient
    PopupMenu = PopupMenu
    TabOrder = 0
    ColorPaletteIndex = 13
    object Series1: TBarSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Style = smsValue
      Marks.Visible = True
      Title = 'Time, ms'
      Emboss.Color = 8553090
      Shadow.Color = 8553090
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Bar'
      YValues.Order = loNone
      Data = {
        00060000000000000000C87E400000000000187A400000000000F07940000000
        0000A0794000000000000074400000000000807140}
    end
    object Series2: TBarSeries
      Marks.Arrow.Visible = True
      Marks.Callout.Brush.Color = clBlack
      Marks.Callout.Arrow.Visible = True
      Marks.Emboss.Color = 8487297
      Marks.Shadow.Color = 8553090
      Marks.Style = smsValue
      Marks.Visible = True
      Title = 'Percent'
      Emboss.Color = 8553090
      Shadow.Color = 8553090
      XValues.Name = 'X'
      XValues.Order = loAscending
      YValues.Name = 'Bar'
      YValues.Order = loNone
    end
  end
  object PopupMenu: TPopupMenu
    Left = 152
    Top = 152
    object SelectSource1: TMenuItem
      Caption = 'Select Source'
      OnClick = SelectSource1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object ClearAll1: TMenuItem
      Caption = 'Clear All'
      OnClick = ClearAll1Click
    end
  end
end
