object UServerControlForm: TUServerControlForm
  Left = 0
  Top = 0
  Caption = 'Server Control'
  ClientHeight = 485
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
    Height = 485
    ActivePage = HttpServerTabSheet
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
        Width = 746
        Height = 415
        Align = alClient
        Caption = ' Channels performance '
        TabOrder = 0
        object PerformanceChart: TChart
          Left = 2
          Top = 15
          Width = 742
          Height = 365
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
          Top = 380
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
        Top = 415
        Width = 746
        Height = 42
        Align = alBottom
        TabOrder = 1
      end
    end
    object OptionsTabSheet: TTabSheet
      Caption = 'Server Settings'
      ImageIndex = 1
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 332
        Height = 416
        Align = alLeft
        TabOrder = 0
        object GroupBox1: TGroupBox
          Left = 1
          Top = 1
          Width = 330
          Height = 160
          Align = alTop
          Caption = 'Main options'
          TabOrder = 0
          DesignSize = (
            330
            160)
          object NumberOfChannelsLabeledEdit: TLabeledEdit
            Left = 6
            Top = 117
            Width = 314
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 83
            EditLabel.Height = 13
            EditLabel.Caption = 'Channels number'
            TabOrder = 0
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
            TabOrder = 1
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
            TabOrder = 2
            Text = 'Server'
          end
        end
        object GroupBox2: TGroupBox
          Left = 1
          Top = 161
          Width = 330
          Height = 254
          Align = alClient
          Caption = ' Channel names '
          TabOrder = 1
          object ChannelNamesStringGrid: TStringGrid
            Left = 2
            Top = 15
            Width = 326
            Height = 237
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
        Height = 416
        Align = alClient
        TabOrder = 1
        inline UHttpServerFrame: TUHttpServerFrame
          Left = 1
          Top = 359
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
          ExplicitTop = 359
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
        Top = 416
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
    object TcpServerTabSheet: TTabSheet
      Caption = 'Tcp Server'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Panel6: TPanel
        Left = 0
        Top = 416
        Width = 746
        Height = 41
        Align = alBottom
        TabOrder = 0
        object ServerStopButton: TButton
          Left = 263
          Top = 6
          Width = 79
          Height = 26
          Caption = 'Stop'
          TabOrder = 0
          OnClick = ServerStopButtonClick
        end
        object ServerStartButton: TButton
          Left = 178
          Top = 6
          Width = 79
          Height = 28
          Caption = 'Start'
          TabOrder = 1
          OnClick = ServerStartButtonClick
        end
        object TcpApplyButton: TButton
          Left = 8
          Top = 5
          Width = 79
          Height = 28
          Caption = 'Apply'
          TabOrder = 2
          OnClick = TcpApplyButtonClick
        end
        object TcpReturnButton: TButton
          Left = 93
          Top = 6
          Width = 79
          Height = 27
          Caption = 'Return'
          TabOrder = 3
          OnClick = HttpReturnButtonClick
        end
      end
      object GroupBox6: TGroupBox
        Left = 0
        Top = 0
        Width = 746
        Height = 113
        Align = alTop
        Caption = 'Tcp server options'
        TabOrder = 1
        object TcpServerIndicatorLabel: TLabel
          Left = 32
          Top = 67
          Width = 121
          Height = 13
          Caption = 'Tcp Server indicator label'
        end
        object TcpServerIndicatorPanel: TPanel
          Left = 3
          Top = 61
          Width = 23
          Height = 25
          Color = clRed
          ParentBackground = False
          TabOrder = 0
        end
        object BindingAddressLabeledEdit: TLabeledEdit
          Left = 3
          Top = 34
          Width = 198
          Height = 21
          EditLabel.Width = 75
          EditLabel.Height = 13
          EditLabel.Caption = 'Binging address'
          TabOrder = 1
          Text = '127.0.0.1'
        end
        object ServerControlPortLabeledEdit: TLabeledEdit
          Left = 207
          Top = 34
          Width = 110
          Height = 21
          EditLabel.Width = 57
          EditLabel.Height = 13
          EditLabel.Caption = 'Binding port'
          TabOrder = 2
          Text = '45045'
        end
      end
    end
    object HttpServerTabSheet: TTabSheet
      Caption = 'Http Server'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox5: TGroupBox
        Left = 0
        Top = 0
        Width = 746
        Height = 233
        Align = alTop
        Caption = ' Main options '
        TabOrder = 0
        DesignSize = (
          746
          233)
        object HttpServerIndicationLabel: TLabel
          Left = 47
          Top = 159
          Width = 128
          Height = 13
          Caption = 'Http server indication label'
        end
        object HttpIPAddressLabeledEdit: TLabeledEdit
          Left = 20
          Top = 34
          Width = 183
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          EditLabel.Width = 73
          EditLabel.Height = 13
          EditLabel.Caption = 'BindingAddress'
          TabOrder = 0
          Text = '127.0.0.1'
        end
        object HttpServerIndicationPanel: TPanel
          Left = 18
          Top = 153
          Width = 23
          Height = 25
          Color = clRed
          ParentBackground = False
          TabOrder = 1
        end
        object HttpLoginLabeledEdit: TLabeledEdit
          Left = 18
          Top = 74
          Width = 374
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          EditLabel.Width = 61
          EditLabel.Height = 13
          EditLabel.Caption = 'Server login:'
          TabOrder = 2
        end
        object HttpPasswordLabeledEdit: TLabeledEdit
          Left = 18
          Top = 114
          Width = 374
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          EditLabel.Width = 85
          EditLabel.Height = 13
          EditLabel.Caption = 'Server password:'
          TabOrder = 3
        end
        object HttpPortLabeledEdit: TLabeledEdit
          Left = 209
          Top = 34
          Width = 183
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          EditLabel.Width = 141
          EditLabel.Height = 13
          EditLabel.Caption = 'Http address (http://ip:port/)'
          TabOrder = 4
          Text = '40004'
        end
      end
      object Panel5: TPanel
        Left = 0
        Top = 416
        Width = 746
        Height = 41
        Align = alBottom
        TabOrder = 1
        object HttpStopButton: TButton
          Left = 271
          Top = 6
          Width = 79
          Height = 27
          Caption = 'Stop'
          TabOrder = 0
          OnClick = ServerStopButtonClick
        end
        object HttpStartButton: TButton
          Left = 186
          Top = 6
          Width = 79
          Height = 28
          Caption = 'Start'
          TabOrder = 1
          OnClick = HttpStartButtonClick
        end
        object HttpApplyButton: TButton
          Left = 16
          Top = 5
          Width = 79
          Height = 28
          Caption = 'Apply'
          TabOrder = 2
          OnClick = HttpApplyButtonClick
        end
        object HttpReturnButton: TButton
          Left = 101
          Top = 6
          Width = 79
          Height = 27
          Caption = 'Return'
          TabOrder = 3
          OnClick = HttpReturnButtonClick
        end
      end
    end
  end
  object CommandTimer: TTimer
    Interval = 10
    OnTimer = CommandTimerTimer
    Left = 24
    Top = 312
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
    Left = 96
    Top = 296
  end
  object ServerRestartTimer: TTimer
    Enabled = False
    OnTimer = ServerRestartTimerTimer
    Left = 168
    Top = 320
  end
  object HttpCommandTimer: TTimer
    Interval = 10
    OnTimer = HttpCommandTimerTimer
    Left = 32
    Top = 384
  end
  object HttpServerRestartTimer: TTimer
    Enabled = False
    OnTimer = HttpServerRestartTimerTimer
    Left = 192
    Top = 384
  end
  object IdHTTPServer: TIdHTTPServer
    Bindings = <
      item
        Port = 80
      end>
    OnConnect = IdHTTPServerConnect
    OnDisconnect = IdHTTPServerDisconnect
    OnCommandGet = IdHTTPServerCommandGet
    Left = 100
    Top = 368
  end
end
