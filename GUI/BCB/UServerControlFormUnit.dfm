object UServerControlForm: TUServerControlForm
  Left = 0
  Top = 0
  Caption = 'Server Control'
  ClientHeight = 641
  ClientWidth = 943
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 120
  TextHeight = 17
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 943
    Height = 641
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    ActivePage = ControlTabSheet
    Align = alClient
    TabOrder = 0
    OnChange = PageControlChange
    object ControlTabSheet: TTabSheet
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Control'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object GroupBox3: TGroupBox
        Left = 0
        Top = 0
        Width = 933
        Height = 554
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alClient
        Caption = ' Channels performance '
        TabOrder = 0
        object PerformanceChart: TChart
          Left = 3
          Top = 19
          Width = 927
          Height = 491
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
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
          Left = 3
          Top = 510
          Width = 927
          Height = 41
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alBottom
          TabOrder = 1
          Visible = False
          object FpsRadioButton: TRadioButton
            Left = 10
            Top = 8
            Width = 75
            Height = 22
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Caption = 'Fps'
            Checked = True
            TabOrder = 0
            TabStop = True
          end
          object RadioButton2: TRadioButton
            Left = 158
            Top = 8
            Width = 147
            Height = 22
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Caption = 'Frame duration (ms)'
            TabOrder = 1
          end
        end
      end
      object Panel4: TPanel
        Left = 0
        Top = 554
        Width = 933
        Height = 52
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alBottom
        TabOrder = 1
        object ServerStartButton: TButton
          Left = 13
          Top = 8
          Width = 98
          Height = 35
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Start'
          TabOrder = 0
          OnClick = ServerStartButtonClick
        end
        object ServerStopButton: TButton
          Left = 119
          Top = 10
          Width = 99
          Height = 33
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Stop'
          TabOrder = 1
          OnClick = ServerStopButtonClick
        end
      end
    end
    object OptionsTabSheet: TTabSheet
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Options'
      ImageIndex = 1
      ExplicitWidth = 746
      ExplicitHeight = 481
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 415
        Height = 558
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alLeft
        TabOrder = 0
        ExplicitHeight = 555
        object GroupBox1: TGroupBox
          Left = 1
          Top = 1
          Width = 413
          Height = 252
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alTop
          Caption = ' Main options '
          TabOrder = 0
          DesignSize = (
            413
            252)
          object ServerControlPortLabeledEdit: TLabeledEdit
            Left = 263
            Top = 153
            Width = 137
            Height = 21
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 74
            EditLabel.Height = 17
            EditLabel.Margins.Left = 4
            EditLabel.Margins.Top = 4
            EditLabel.Margins.Right = 4
            EditLabel.Margins.Bottom = 4
            EditLabel.Caption = 'Binding port'
            TabOrder = 0
            Text = '45045'
          end
          object NumberOfChannelsLabeledEdit: TLabeledEdit
            Left = 8
            Top = 214
            Width = 392
            Height = 21
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 107
            EditLabel.Height = 17
            EditLabel.Margins.Left = 4
            EditLabel.Margins.Top = 4
            EditLabel.Margins.Right = 4
            EditLabel.Margins.Bottom = 4
            EditLabel.Caption = 'Channels number'
            TabOrder = 1
            Text = '1'
          end
          object ServerNameLabeledEdit: TLabeledEdit
            Left = 8
            Top = 44
            Width = 392
            Height = 21
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 79
            EditLabel.Height = 17
            EditLabel.Margins.Left = 4
            EditLabel.Margins.Top = 4
            EditLabel.Margins.Right = 4
            EditLabel.Margins.Bottom = 4
            EditLabel.Caption = 'Server Name'
            TabOrder = 2
            Text = 'Server'
          end
          object ServerIdLabeledEdit: TLabeledEdit
            Left = 8
            Top = 95
            Width = 392
            Height = 21
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 56
            EditLabel.Height = 17
            EditLabel.Margins.Left = 4
            EditLabel.Margins.Top = 4
            EditLabel.Margins.Right = 4
            EditLabel.Margins.Bottom = 4
            EditLabel.Caption = 'Server Id'
            TabOrder = 3
            Text = 'Server'
          end
          object BindingAddressLabeledEdit: TLabeledEdit
            Left = 8
            Top = 153
            Width = 247
            Height = 21
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 95
            EditLabel.Height = 17
            EditLabel.Margins.Left = 4
            EditLabel.Margins.Top = 4
            EditLabel.Margins.Right = 4
            EditLabel.Margins.Bottom = 4
            EditLabel.Caption = 'Binging address'
            TabOrder = 4
            Text = '127.0.0.1'
          end
        end
        object GroupBox2: TGroupBox
          Left = 1
          Top = 253
          Width = 413
          Height = 304
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alClient
          Caption = ' Channel names '
          TabOrder = 1
          ExplicitHeight = 301
          object ChannelNamesStringGrid: TStringGrid
            Left = 3
            Top = 19
            Width = 407
            Height = 279
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
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
        Left = 415
        Top = 0
        Width = 520
        Height = 558
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alClient
        TabOrder = 1
        ExplicitWidth = 518
        ExplicitHeight = 555
        inline UHttpServerFrame: TUHttpServerFrame
          Left = 1
          Top = 488
          Width = 518
          Height = 69
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alBottom
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -15
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          ExplicitLeft = 1
          ExplicitTop = 485
          ExplicitWidth = 515
          ExplicitHeight = 69
          inherited Image1: TImage
            Width = 518
            Height = 69
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            ExplicitLeft = 0
            ExplicitTop = 0
            ExplicitWidth = 515
            ExplicitHeight = 69
          end
          inherited IdHTTPServer: TIdHTTPServer
            OnCommandGet = UHttpServerFrameIdHTTPServerCommandGet
          end
        end
        object GroupBox4: TGroupBox
          Left = 1
          Top = 1
          Width = 518
          Height = 138
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alTop
          Caption = ' Metadata options '
          TabOrder = 1
          ExplicitWidth = 515
          DesignSize = (
            518
            138)
          object MetadataComponentNameLabeledEdit: TLabeledEdit
            Left = 5
            Top = 44
            Width = 505
            Height = 21
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 112
            EditLabel.Height = 17
            EditLabel.Margins.Left = 4
            EditLabel.Margins.Top = 4
            EditLabel.Margins.Right = 4
            EditLabel.Margins.Bottom = 4
            EditLabel.Caption = 'Component Name'
            TabOrder = 0
          end
          object MetadataComponentStateNameLabeledEdit: TLabeledEdit
            Left = 8
            Top = 95
            Width = 503
            Height = 21
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 148
            EditLabel.Height = 17
            EditLabel.Margins.Left = 4
            EditLabel.Margins.Top = 4
            EditLabel.Margins.Right = 4
            EditLabel.Margins.Bottom = 4
            EditLabel.Caption = 'Component State Name'
            TabOrder = 1
          end
        end
      end
      object Panel3: TPanel
        Left = 0
        Top = 558
        Width = 935
        Height = 51
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alBottom
        TabOrder = 2
        ExplicitTop = 555
        ExplicitWidth = 933
        object ApplyOptionsButton: TButton
          Left = 9
          Top = 8
          Width = 99
          Height = 35
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Apply'
          TabOrder = 0
          OnClick = ApplyOptionsButtonClick
        end
        object ReturnOptionsButton: TButton
          Left = 115
          Top = 8
          Width = 98
          Height = 35
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
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
    Left = 256
    Top = 40
  end
end
