object UServerControlForm: TUServerControlForm
  Left = 0
  Top = 0
  Caption = 'Server Control'
  ClientHeight = 490
  ClientWidth = 721
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
    Width = 721
    Height = 490
    ActivePage = OptionsTabSheet
    Align = alClient
    TabOrder = 0
    OnChange = PageControlChange
    object ControlTabSheet: TTabSheet
      Caption = 'Control'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox3: TGroupBox
        Left = 0
        Top = 0
        Width = 713
        Height = 421
        Align = alClient
        Caption = ' Channels performance '
        TabOrder = 0
        object PerformanceChart: TChart
          Left = 2
          Top = 15
          Width = 709
          Height = 373
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
          BottomAxis.LabelsFont.Name = 'Verdana'
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
          TopAxis.LabelsFont.Name = 'Verdana'
          TopAxis.TicksInner.Color = 11119017
          TopAxis.Title.Font.Name = 'Verdana'
          Align = alClient
          TabOrder = 0
          ColorPaletteIndex = 13
          object Series1: TBarSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = True
            Title = 'Model'
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Bar'
            YValues.Order = loNone
          end
          object Series2: TBarSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = True
            Title = 'Transport+Standby'
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Bar'
            YValues.Order = loNone
          end
          object Series3: TBarSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = True
            Title = 'Total'
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Bar'
            YValues.Order = loNone
          end
        end
        object Panel: TPanel
          Left = 2
          Top = 388
          Width = 709
          Height = 31
          Align = alBottom
          TabOrder = 1
          Visible = False
          object FpsRadioButton: TRadioButton
            Left = 8
            Top = 6
            Width = 57
            Height = 17
            Caption = 'Fps'
            Checked = True
            TabOrder = 0
            TabStop = True
          end
          object RadioButton2: TRadioButton
            Left = 120
            Top = 6
            Width = 113
            Height = 17
            Caption = 'Frame duration (ms)'
            TabOrder = 1
          end
        end
      end
      object Panel4: TPanel
        Left = 0
        Top = 421
        Width = 713
        Height = 41
        Align = alBottom
        TabOrder = 1
        object ServerStartButton: TButton
          Left = 10
          Top = 6
          Width = 75
          Height = 27
          Caption = 'Start'
          TabOrder = 0
          OnClick = ServerStartButtonClick
        end
        object ServerStopButton: TButton
          Left = 91
          Top = 8
          Width = 75
          Height = 25
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
        Width = 317
        Height = 421
        Align = alLeft
        TabOrder = 0
        object GroupBox1: TGroupBox
          Left = 1
          Top = 1
          Width = 315
          Height = 152
          Align = alTop
          Caption = ' Main options '
          TabOrder = 0
          DesignSize = (
            315
            152)
          object ServerControlPortLabeledEdit: TLabeledEdit
            Left = 6
            Top = 73
            Width = 299
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 91
            EditLabel.Height = 13
            EditLabel.Caption = 'Server control port'
            TabOrder = 0
            Text = '45045'
          end
          object NumberOfChannelsLabeledEdit: TLabeledEdit
            Left = 6
            Top = 121
            Width = 299
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
            Top = 29
            Width = 299
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 62
            EditLabel.Height = 13
            EditLabel.Caption = 'Server Name'
            TabOrder = 2
            Text = 'Server'
          end
        end
        object GroupBox2: TGroupBox
          Left = 1
          Top = 153
          Width = 315
          Height = 267
          Align = alClient
          Caption = ' Channel names '
          TabOrder = 1
          ExplicitTop = 111
          ExplicitHeight = 309
          object ChannelNamesStringGrid: TStringGrid
            Left = 2
            Top = 15
            Width = 311
            Height = 250
            Align = alClient
            ColCount = 2
            DefaultRowHeight = 20
            RowCount = 2
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing]
            TabOrder = 0
            OnKeyDown = ChannelNamesStringGridKeyDown
            ExplicitHeight = 292
          end
        end
      end
      object Panel2: TPanel
        Left = 317
        Top = 0
        Width = 396
        Height = 421
        Align = alClient
        TabOrder = 1
        inline UHttpServerFrame: TUHttpServerFrame
          Left = 1
          Top = 1
          Width = 394
          Height = 52
          Align = alTop
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          ExplicitLeft = 1
          ExplicitTop = 1
          ExplicitWidth = 394
          ExplicitHeight = 52
          inherited Image1: TImage
            Width = 394
            Height = 52
            ExplicitWidth = 394
            ExplicitHeight = 52
          end
          inherited IdHTTPServer: TIdHTTPServer
            OnCommandGet = UHttpServerFrameIdHTTPServerCommandGet
          end
        end
      end
      object Panel3: TPanel
        Left = 0
        Top = 421
        Width = 713
        Height = 41
        Align = alBottom
        TabOrder = 2
        object ApplyOptionsButton: TButton
          Left = 7
          Top = 6
          Width = 75
          Height = 27
          Caption = 'Apply'
          TabOrder = 0
          OnClick = ApplyOptionsButtonClick
        end
        object ReturnOptionsButton: TButton
          Left = 88
          Top = 6
          Width = 75
          Height = 27
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
  object TcpServer: TTcpServer
    LocalHost = '127.0.0.1'
    LocalPort = '45050'
    OnAccept = TcpServerAccept
    OnGetThread = TcpServerGetThread
    OnListening = TcpServerListening
    Left = 144
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
    Left = 112
    Top = 144
  end
end
