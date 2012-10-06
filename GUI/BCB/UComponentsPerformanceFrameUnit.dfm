inherited UComponentsPerformanceFrame: TUComponentsPerformanceFrame
  Width = 454
  Height = 326
  ExplicitWidth = 454
  ExplicitHeight = 326
  object Chart: TChart
    Left = 0
    Top = 0
    Width = 454
    Height = 280
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
    Title.Text.Strings = (
      'TChart')
    Title.Visible = False
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
    ExplicitHeight = 326
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
  object Panel1: TPanel
    Left = 0
    Top = 280
    Width = 454
    Height = 46
    Align = alBottom
    TabOrder = 1
    object ShowModeRadioGroup: TRadioGroup
      Left = 1
      Top = 1
      Width = 136
      Height = 44
      Align = alLeft
      Caption = ' Show mode '
      Columns = 2
      ItemIndex = 0
      Items.Strings = (
        'Ms'
        'Percent')
      TabOrder = 0
      OnClick = ShowModeRadioGroupClick
      ExplicitHeight = 36
    end
    object GroupBox1: TGroupBox
      Left = 137
      Top = 1
      Width = 316
      Height = 44
      Align = alClient
      Caption = ' Average interval (counts) '
      TabOrder = 1
      ExplicitLeft = 224
      ExplicitTop = 16
      ExplicitWidth = 185
      ExplicitHeight = 105
      object AverageIntervalSpinEdit: TSpinEdit
        Left = 6
        Top = 16
        Width = 43
        Height = 22
        MaxValue = 1000
        MinValue = 0
        TabOrder = 0
        Value = 10
        OnChange = AverageIntervalSpinEditChange
      end
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
