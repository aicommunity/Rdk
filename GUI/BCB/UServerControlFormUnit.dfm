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
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 721
    Height = 490
    ActivePage = ControlTabSheet
    Align = alClient
    TabOrder = 0
    object ControlTabSheet: TTabSheet
      Caption = 'Control'
      object GroupBox3: TGroupBox
        Left = 0
        Top = 0
        Width = 713
        Height = 201
        Align = alTop
        Caption = ' Channels performance '
        TabOrder = 0
        object PerformanceChart: TChart
          Left = 2
          Top = 15
          Width = 709
          Height = 153
          BackWall.Brush.Gradient.Direction = gdBottomTop
          BackWall.Brush.Gradient.EndColor = clWhite
          BackWall.Brush.Gradient.MidColor = clNone
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
          ExplicitLeft = 256
          ExplicitTop = 40
          ExplicitWidth = 400
          ExplicitHeight = 250
          ColorPaletteIndex = 13
          object Series1: TBarSeries
            Marks.Arrow.Visible = True
            Marks.Callout.Brush.Color = clBlack
            Marks.Callout.Arrow.Visible = True
            Marks.Visible = True
            Title = 'Performance'
            XValues.Name = 'X'
            XValues.Order = loAscending
            YValues.Name = 'Bar'
            YValues.Order = loNone
          end
        end
        object Panel: TPanel
          Left = 2
          Top = 168
          Width = 709
          Height = 31
          Align = alBottom
          TabOrder = 1
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
    end
    object OptionsTabSheet: TTabSheet
      Caption = 'Options'
      ImageIndex = 1
      ExplicitLeft = 8
      ExplicitTop = 28
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 317
        Height = 462
        Align = alLeft
        TabOrder = 0
        object GroupBox1: TGroupBox
          Left = 1
          Top = 1
          Width = 315
          Height = 110
          Align = alTop
          Caption = ' Main options '
          TabOrder = 0
          ExplicitLeft = 79
          ExplicitTop = 3
          ExplicitWidth = 238
          DesignSize = (
            315
            110)
          object ServerControlPortLabeledEdit: TLabeledEdit
            Left = 6
            Top = 31
            Width = 299
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 91
            EditLabel.Height = 13
            EditLabel.Caption = 'Server control port'
            TabOrder = 0
            Text = '80'
            ExplicitWidth = 222
          end
          object NumberOfChannelsLabeledEdit: TLabeledEdit
            Left = 6
            Top = 79
            Width = 218
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 83
            EditLabel.Height = 13
            EditLabel.Caption = 'Channels number'
            TabOrder = 1
            Text = '1'
          end
          object ApplyChannelsNumberButton: TButton
            Left = 230
            Top = 77
            Width = 75
            Height = 25
            Caption = 'Apply'
            TabOrder = 2
          end
        end
        object GroupBox2: TGroupBox
          Left = 1
          Top = 111
          Width = 315
          Height = 350
          Align = alClient
          Caption = ' Channel names '
          TabOrder = 1
          ExplicitLeft = 76
          ExplicitTop = 119
          ExplicitWidth = 241
          ExplicitHeight = 313
          object ChannelNamesValueListEditor: TValueListEditor
            Left = 2
            Top = 15
            Width = 311
            Height = 333
            Align = alClient
            TabOrder = 0
            ExplicitWidth = 237
            ExplicitHeight = 296
            ColWidths = (
              150
              155)
          end
        end
      end
      object Panel2: TPanel
        Left = 317
        Top = 0
        Width = 396
        Height = 462
        Align = alClient
        TabOrder = 1
        ExplicitLeft = 432
        ExplicitTop = 136
        ExplicitWidth = 185
        ExplicitHeight = 41
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
          end
          inherited IdHTTPServer: TIdHTTPServer
            OnCommandGet = UHttpServerFrameIdHTTPServerCommandGet
          end
        end
      end
    end
  end
end
