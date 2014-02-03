inherited VideoOutputFrame: TVideoOutputFrame
  Width = 916
  Height = 714
  Constraints.MinHeight = 100
  Constraints.MinWidth = 200
  PopupMenu = PopupMenu
  TabStop = True
  ExplicitWidth = 916
  ExplicitHeight = 714
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 916
    Height = 714
    ActivePage = VideoSourceTabSheet
    Align = alClient
    TabOrder = 0
    object VideoSourceTabSheet: TTabSheet
      Caption = 'Video Source'
      object GroupBox: TGroupBox
        Left = 0
        Top = 0
        Width = 908
        Height = 564
        Align = alClient
        DoubleBuffered = True
        ParentDoubleBuffered = False
        TabOrder = 0
        object Image: TImage
          Left = 2
          Top = 15
          Width = 904
          Height = 547
          Cursor = crCross
          Align = alClient
          Stretch = True
          OnMouseDown = ImageMouseDown
          OnMouseMove = ImageMouseMove
          OnMouseUp = ImageMouseUp
          ExplicitTop = 14
          ExplicitWidth = 698
          ExplicitHeight = 400
        end
        object GroupBox1: TGroupBox
          Left = 2
          Top = 15
          Width = 904
          Height = 547
          Align = alClient
          Caption = 'GroupBox1'
          TabOrder = 0
          Visible = False
        end
      end
      object Panel1: TPanel
        Left = 0
        Top = 564
        Width = 908
        Height = 41
        Align = alBottom
        TabOrder = 1
        DesignSize = (
          908
          41)
        object StopButton: TButton
          Left = 871
          Top = 6
          Width = 30
          Height = 25
          Anchors = [akTop, akRight]
          Caption = 'Stop'
          TabOrder = 0
          OnClick = StopButtonClick
        end
        object StartButton: TButton
          Left = 835
          Top = 6
          Width = 30
          Height = 25
          Anchors = [akTop, akRight]
          Caption = 'Start'
          TabOrder = 1
          OnClick = StartButtonClick
        end
        object TimeEdit: TMaskEdit
          Left = 750
          Top = 8
          Width = 79
          Height = 21
          Anchors = [akTop, akRight]
          EditMask = '000\:00\:00\:00;1;_'
          MaxLength = 12
          TabOrder = 2
          Text = '000:00:00:00'
          OnChange = TimeEditChange
        end
        object TrackBar: TTrackBar
          Left = 2
          Top = 9
          Width = 742
          Height = 22
          Anchors = [akLeft, akTop, akRight]
          Max = 1
          Min = 1
          PageSize = 1
          Position = 1
          TabOrder = 3
          OnChange = TrackBarChange
        end
      end
      object Panel2: TPanel
        Left = 0
        Top = 605
        Width = 908
        Height = 46
        Align = alBottom
        TabOrder = 2
        DesignSize = (
          908
          46)
        object SendToEdit: TEdit
          Left = 16
          Top = 4
          Width = 643
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          Color = clBtnFace
          ReadOnly = True
          TabOrder = 0
        end
        object Button1: TButton
          Left = 750
          Top = 2
          Width = 79
          Height = 25
          Anchors = [akTop, akRight]
          Caption = 'Send points'
          TabOrder = 1
          OnClick = Button1Click
        end
        object SelectSendToButton: TButton
          Left = 835
          Top = 2
          Width = 66
          Height = 25
          Anchors = [akTop, akRight]
          Caption = 'Select To'
          DropDownMenu = SelectPopupMenu
          Style = bsSplitButton
          TabOrder = 2
        end
        object SendAsMatrixButton: TButton
          Left = 665
          Top = 2
          Width = 79
          Height = 25
          Anchors = [akTop, akRight]
          Caption = 'Send as Matrix'
          TabOrder = 3
          OnClick = SendAsMatrixButtonClick
        end
        object SendPointsByStepCheckBox: TCheckBox
          Left = 5
          Top = 27
          Width = 220
          Height = 17
          Caption = 'Send points before even calculation step'
          TabOrder = 4
        end
        object DeletePointsAfterSendCheckBox: TCheckBox
          Left = 230
          Top = 27
          Width = 147
          Height = 17
          Caption = 'Delete points after send'
          TabOrder = 5
        end
      end
      object Panel3: TPanel
        Left = 0
        Top = 651
        Width = 908
        Height = 35
        Align = alBottom
        TabOrder = 3
        object ShowCentralPointCheckBox: TCheckBox
          Left = 1
          Top = 1
          Width = 120
          Height = 33
          Align = alLeft
          Caption = ' Show central point'
          TabOrder = 0
        end
        object PointXEdit: TEdit
          Left = 127
          Top = 6
          Width = 58
          Height = 21
          TabOrder = 1
          Text = '0'
        end
        object PointYEdit: TEdit
          Left = 191
          Top = 6
          Width = 58
          Height = 21
          TabOrder = 2
          Text = '0'
        end
      end
    end
    object VideoRegistratorTabSheet: TTabSheet
      Caption = 'Video Registrator'
      ImageIndex = 1
      inline TVideoRegistratorFrame: TTVideoRegistratorFrame
        Left = 0
        Top = 0
        Width = 908
        Height = 686
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        inherited ControlPanel: TPanel
          inherited ModePageControl: TPageControl
            inherited NetworkStreamingTabSheet: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 24
              ExplicitWidth = 370
              ExplicitHeight = 257
            end
          end
          inherited SourcePageControl: TPageControl
            inherited SourceFrameTabSheet: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 24
              ExplicitWidth = 370
              ExplicitHeight = 165
            end
          end
        end
      end
    end
  end
  object Timer: TTimer
    Interval = 30
    OnTimer = TimerTimer
    Left = 8
    Top = 408
  end
  object PopupMenu: TPopupMenu
    Left = 48
    Top = 408
    object SendToComponent: TMenuItem
      Caption = 'Send To Component Parameter...'
      OnClick = SendToComponentClick
    end
    object SendToComponentState1: TMenuItem
      Caption = 'Send To Component State...'
      OnClick = SendToComponentState1Click
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object SendImageToComponentInput1: TMenuItem
      Caption = 'Send Image To Component Input...'
      OnClick = SendImageToComponentInput1Click
    end
    object SendImageToComponentOutput1: TMenuItem
      Caption = 'Send Image To Component Output...'
      OnClick = SendImageToComponentOutput1Click
    end
    object SendImageToComponentProperty1: TMenuItem
      Caption = 'Send Image To Component Property ...'
      OnClick = SendImageToComponentProperty1Click
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object SaveImage1: TMenuItem
      Caption = 'Save Image'
      OnClick = SaveImage1Click
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object N1: TMenuItem
      Caption = 'Toolbar'
      OnClick = N1Click
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object SourceControl1: TMenuItem
      Caption = 'Source Control'
      OnClick = SourceControl1Click
    end
  end
  object SelectPopupMenu: TPopupMenu
    Left = 88
    Top = 408
    object Parameter1: TMenuItem
      Caption = 'Parameter'
      OnClick = Parameter1Click
    end
    object State1: TMenuItem
      Caption = 'State'
      OnClick = State1Click
    end
    object PropertyMatrix1: TMenuItem
      Caption = 'Property Matrix'
      OnClick = PropertyMatrix1Click
    end
  end
  object SavePictureDialog: TSavePictureDialog
    DefaultExt = 'bmp'
    Filter = 'Bitmaps (*.bmp)|*.bmp|JPEG Image File (*.jpg)|*.jpg'
    Left = 128
    Top = 408
  end
end
