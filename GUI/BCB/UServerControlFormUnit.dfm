object UServerControlForm: TUServerControlForm
  Left = 0
  Top = 0
  Caption = 'Server Control'
  ClientHeight = 513
  ClientWidth = 754
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 754
    Height = 513
    ActivePage = ControlTabSheet
    Align = alClient
    TabOrder = 0
    OnChange = PageControlChange
    object ControlTabSheet: TTabSheet
      Caption = 'Control'
      object GroupBox3: TGroupBox
        Left = 0
        Top = 0
        Width = 746
        Height = 443
        Align = alClient
        Caption = ' Channels performance '
        TabOrder = 0
        object PerformanceChart: TChart
          Left = 2
          Top = 15
          Width = 742
          Height = 393
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
          TopAxis.TicksInner.Color = 11119017
          TopAxis.Title.Font.Name = 'Verdana'
          Zoom.Pen.Mode = pmNotXor
          Align = alClient
          TabOrder = 0
          DefaultCanvas = 'TGDIPlusCanvas'
          ColorPaletteIndex = 13
          object Series1: TBarSeries
            Title = 'Model'
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Bar'
            YValues.Order = loNone
          end
          object Series2: TBarSeries
            Title = 'Transport+Standby'
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Bar'
            YValues.Order = loNone
          end
          object Series3: TBarSeries
            Title = 'Total'
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Bar'
            YValues.Order = loNone
          end
        end
        object Panel: TPanel
          Left = 2
          Top = 408
          Width = 742
          Height = 33
          Align = alBottom
          TabOrder = 1
          Visible = False
          object FpsRadioButton: TRadioButton
            Left = 8
            Top = 6
            Width = 60
            Height = 18
            Caption = 'Fps'
            Checked = True
            TabOrder = 0
            TabStop = True
          end
          object RadioButton2: TRadioButton
            Left = 126
            Top = 6
            Width = 118
            Height = 18
            Caption = 'Frame duration (ms)'
            TabOrder = 1
          end
        end
      end
      object Panel4: TPanel
        Left = 0
        Top = 443
        Width = 746
        Height = 42
        Align = alBottom
        TabOrder = 1
        object ServerStartButton: TButton
          Left = 10
          Top = 6
          Width = 79
          Height = 28
          Caption = 'Start'
          TabOrder = 0
          OnClick = ServerStartButtonClick
        end
        object ServerStopButton: TButton
          Left = 95
          Top = 8
          Width = 79
          Height = 26
          Caption = 'Stop'
          TabOrder = 1
          OnClick = ServerStopButtonClick
        end
      end
    end
    object OptionsTabSheet: TTabSheet
      Caption = 'Options'
      ImageIndex = 1
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 332
        Height = 444
        Align = alLeft
        TabOrder = 0
        object GroupBox1: TGroupBox
          Left = 1
          Top = 1
          Width = 330
          Height = 201
          Align = alTop
          Caption = ' Main options '
          TabOrder = 0
          DesignSize = (
            330
            201)
          object ServerControlPortLabeledEdit: TLabeledEdit
            Left = 210
            Top = 122
            Width = 110
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 57
            EditLabel.Height = 13
            EditLabel.Caption = 'Binding port'
            TabOrder = 0
            Text = '45045'
          end
          object NumberOfChannelsLabeledEdit: TLabeledEdit
            Left = 6
            Top = 171
            Width = 314
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 83
            EditLabel.Height = 13
            EditLabel.Caption = 'Channels number'
            TabOrder = 1
            Text = '1'
          end
          object ServerNameLabeledEdit: TLabeledEdit
            Left = 6
            Top = 35
            Width = 314
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 62
            EditLabel.Height = 13
            EditLabel.Caption = 'Server Name'
            TabOrder = 2
            Text = 'Server'
          end
          object ServerIdLabeledEdit: TLabeledEdit
            Left = 6
            Top = 76
            Width = 314
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 45
            EditLabel.Height = 13
            EditLabel.Caption = 'Server Id'
            TabOrder = 3
            Text = 'Server'
          end
          object BindingAddressLabeledEdit: TLabeledEdit
            Left = 6
            Top = 122
            Width = 198
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 75
            EditLabel.Height = 13
            EditLabel.Caption = 'Binging address'
            TabOrder = 4
            Text = '127.0.0.1'
          end
        end
        object GroupBox2: TGroupBox
          Left = 1
          Top = 202
          Width = 330
          Height = 241
          Align = alClient
          Caption = ' Channel names '
          TabOrder = 1
          object ChannelNamesStringGrid: TStringGrid
            Left = 2
            Top = 15
            Width = 326
            Height = 224
            Align = alClient
            ColCount = 2
            DefaultRowHeight = 20
            RowCount = 2
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing]
            TabOrder = 0
            OnKeyDown = ChannelNamesStringGridKeyDown
          end
        end
      end
      object Panel2: TPanel
        Left = 332
        Top = 0
        Width = 414
        Height = 444
        Align = alClient
        TabOrder = 1
        inline UHttpServerFrame: TUHttpServerFrame
          Left = 1
          Top = 387
          Width = 412
          Height = 56
          Align = alBottom
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          ExplicitLeft = 1
          ExplicitTop = 387
          ExplicitWidth = 412
          ExplicitHeight = 56
          inherited Image1: TImage
            Width = 412
            Height = 56
            ExplicitLeft = 0
            ExplicitTop = 0
            ExplicitWidth = 414
            ExplicitHeight = 55
          end
        end
        object GroupBox4: TGroupBox
          Left = 1
          Top = 1
          Width = 412
          Height = 110
          Align = alTop
          Caption = ' Metadata options '
          TabOrder = 1
          DesignSize = (
            412
            110)
          object MetadataComponentNameLabeledEdit: TLabeledEdit
            Left = 4
            Top = 35
            Width = 404
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 85
            EditLabel.Height = 13
            EditLabel.Caption = 'Component Name'
            TabOrder = 0
          end
          object MetadataComponentStateNameLabeledEdit: TLabeledEdit
            Left = 6
            Top = 76
            Width = 403
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 114
            EditLabel.Height = 13
            EditLabel.Caption = 'Component State Name'
            TabOrder = 1
          end
        end
      end
      object Panel3: TPanel
        Left = 0
        Top = 444
        Width = 746
        Height = 41
        Align = alBottom
        TabOrder = 2
        object ApplyOptionsButton: TButton
          Left = 7
          Top = 6
          Width = 79
          Height = 28
          Caption = 'Apply'
          TabOrder = 0
          OnClick = ApplyOptionsButtonClick
        end
        object ReturnOptionsButton: TButton
          Left = 92
          Top = 6
          Width = 78
          Height = 28
          Caption = 'Return'
          TabOrder = 1
          OnClick = ReturnOptionsButtonClick
        end
      end
    end
  end
  object CommandTimer: TTimer
    Interval = 10
    OnTimer = CommandTimerTimer
    Left = 80
    Top = 40
  end
  object IdTCPServer: TIdTCPServer
    Bindings = <
      item
        IP = '127.0.0.1'
        Port = 45045
      end>
    DefaultPort = 0
    OnConnect = IdTCPServerConnect
    OnDisconnect = IdTCPServerDisconnect
    OnExecute = IdTCPServerExecute
    Left = 208
    Top = 40
  end
  object ServerRestartTimer: TTimer
    Enabled = False
    OnTimer = ServerRestartTimerTimer
    Left = 256
    Top = 40
  end
end
