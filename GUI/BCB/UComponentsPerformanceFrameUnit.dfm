inherited UComponentsPerformanceFrame: TUComponentsPerformanceFrame
  Width = 454
  Height = 326
  ExplicitWidth = 454
  ExplicitHeight = 326
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 454
    Height = 280
    ActivePage = TabSheet2
    Align = alClient
    TabOrder = 0
    object TabSheet1: TTabSheet
      Caption = 'General'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 298
      object Chart: TChart
        Left = 0
        Top = 0
        Width = 446
        Height = 252
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
        Legend.Visible = False
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
        BottomAxis.LabelsBehind = True
        BottomAxis.LabelsMultiLine = True
        BottomAxis.LabelsOnAxis = False
        BottomAxis.RoundFirstLabel = False
        BottomAxis.TicksInner.Color = 11119017
        BottomAxis.Title.Font.Name = 'Verdana'
        DepthAxis.Axis.Color = 4210752
        DepthAxis.Grid.Color = 11119017
        DepthAxis.TicksInner.Color = 11119017
        DepthAxis.Title.Font.Name = 'Verdana'
        DepthTopAxis.Axis.Color = 4210752
        DepthTopAxis.Grid.Color = 11119017
        DepthTopAxis.TicksInner.Color = 11119017
        DepthTopAxis.Title.Font.Name = 'Verdana'
        LeftAxis.Axis.Color = 4210752
        LeftAxis.Grid.Color = 11119017
        LeftAxis.TicksInner.Color = 11119017
        LeftAxis.Title.Font.Name = 'Verdana'
        RightAxis.Axis.Color = 4210752
        RightAxis.Grid.Color = 11119017
        RightAxis.TicksInner.Color = 11119017
        RightAxis.Title.Font.Name = 'Verdana'
        TopAxis.Axis.Color = 4210752
        TopAxis.Grid.Color = 11119017
        TopAxis.LabelsAlternate = True
        TopAxis.LabelsMultiLine = True
        TopAxis.LabelStyle = talText
        TopAxis.TicksInner.Color = 11119017
        TopAxis.Title.Font.Name = 'Verdana'
        View3D = False
        Zoom.Pen.Mode = pmNotXor
        Align = alClient
        PopupMenu = PopupMenu
        TabOrder = 0
        DefaultCanvas = 'TGDIPlusCanvas'
        PrintMargins = (
          15
          19
          15
          19)
        ColorPaletteIndex = 13
        object Series1: TBarSeries
          Marks.Emboss.Color = 8618883
          Marks.Shadow.Color = 8618883
          Marks.Shadow.Visible = False
          Marks.Style = smsValue
          Marks.TextAlign = taLeftJustify
          Title = 'Time, ms'
          Emboss.Color = 8750469
          MultiBar = mbNone
          Shadow.Color = 8750469
          XValues.Name = 'X'
          XValues.Order = loAscending
          YValues.Name = 'Bar'
          YValues.Order = loNone
          Data = {
            00060000000000000000C87E400000000000187A400000000000F07940000000
            0000A0794000000000000074400000000000807140}
        end
        object Series2: TBarSeries
          Marks.Emboss.Color = 8487297
          Marks.Shadow.Color = 8553090
          Marks.Style = smsValue
          Title = 'Percent'
          Emboss.Color = 8553090
          MultiBar = mbNone
          Shadow.Color = 8553090
          XValues.Name = 'X'
          XValues.Order = loAscending
          YValues.Name = 'Bar'
          YValues.Order = loNone
        end
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Components Details'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox2: TGroupBox
        Left = 0
        Top = 0
        Width = 446
        Height = 252
        Align = alClient
        Caption = ' Components '
        TabOrder = 0
        ExplicitWidth = 211
        ExplicitHeight = 298
        object ComponentsStringGrid: TStringGrid
          Left = 2
          Top = 15
          Width = 442
          Height = 235
          Align = alClient
          DefaultRowHeight = 20
          TabOrder = 0
          ExplicitWidth = 207
        end
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Interface Details'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox3: TGroupBox
        Left = 0
        Top = 0
        Width = 446
        Height = 252
        Align = alClient
        Caption = ' Interfaces '
        TabOrder = 0
        object InterfacesStringGrid: TStringGrid
          Left = 2
          Top = 15
          Width = 442
          Height = 235
          Align = alClient
          DefaultRowHeight = 20
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
          TabOrder = 0
        end
      end
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
    end
    object GroupBox1: TGroupBox
      Left = 137
      Top = 1
      Width = 316
      Height = 44
      Align = alClient
      Caption = ' Average interval (counts) '
      TabOrder = 1
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
    object SelectAll1: TMenuItem
      Caption = 'Select All'
      OnClick = SelectAll1Click
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
