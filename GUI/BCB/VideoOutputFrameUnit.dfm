inherited VideoOutputFrame: TVideoOutputFrame
  Width = 702
  Height = 458
  Constraints.MinHeight = 100
  Constraints.MinWidth = 200
  PopupMenu = PopupMenu
  TabStop = True
  ExplicitWidth = 702
  ExplicitHeight = 458
  object GroupBox: TGroupBox
    Left = 0
    Top = 0
    Width = 702
    Height = 336
    Align = alClient
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 0
    object Image: TImage
      Left = 2
      Top = 15
      Width = 698
      Height = 319
      Cursor = crCross
      Align = alClient
      Stretch = True
      OnMouseDown = ImageMouseDown
      OnMouseMove = ImageMouseMove
      OnMouseUp = ImageMouseUp
      ExplicitTop = 14
      ExplicitHeight = 400
    end
    object VideoGrabber: TVideoGrabber
      Left = 2
      Top = 15
      Width = 698
      Height = 319
      Align = alClient
      Caption = 'VideoGrabber'
      Color = clBlack
      Visible = False
      ASFVideoWidth = -1
      AspectRatioToUse = -1.000000000000000000
      AudioCompressor = 0
      AutoFilePrefix = 'vg'
      DoubleBuffered = True
      Cropping_Zoom = 1.000000000000000000
      LicenseString = 'N/A'
      MotionDetector_Grid = 
        '5555555555 5555555555 5555555555 5555555555 5555555555 555555555' +
        '5 5555555555 5555555555 5555555555 5555555555'
      PlayerSpeedRatio = 1.000000000000000000
      PlayerTrackBar = TrackBar
      Reencoding_StartTime = -1
      Reencoding_StartFrame = -1
      Reencoding_StopTime = -1
      Reencoding_StopFrame = -1
      TextOverlay_Font.Charset = DEFAULT_CHARSET
      TextOverlay_Font.Color = clAqua
      TextOverlay_Font.Height = -16
      TextOverlay_Font.Name = 'Tahoma'
      TextOverlay_Font.Style = []
      TextOverlay_String = 
        'Note: the date/time formats '#13#10'can be easily modified.'#13#10#13#10'system ' +
        'date/time: %sys_time[dd/mm/yy hh:nn:ss]%'#13#10'DV time code: %time_co' +
        'de%'#13#10'DV date/time: %dv_time[dd/mm/yy hh:nn:ss]%'#13#10'frame number: %' +
        'frame_count%'#13#10'time (full): %time_full%'#13#10'time (sec): %time_sec%'#13#10 +
        'time (ns): %time_100ns%'
      VideoCompression_Quality = 1.000000000000000000
      VideoCompressor = 0
      VideoFromImages_TemporaryFile = 'SetOfBitmaps01.dat'
      VideoProcessing_RotationCustomAngle = 45.500000000000000000
      VideoSource = vs_VideoFileOrURL
      VideoSource_FileOrURL_StartTime = -1
      VideoSource_FileOrURL_StopTime = -1
      OnFrameCaptureCompleted = VideoGrabberFrameCaptureCompleted
    end
    object GroupBox1: TGroupBox
      Left = 2
      Top = 15
      Width = 698
      Height = 319
      Align = alClient
      Caption = 'GroupBox1'
      TabOrder = 1
      Visible = False
      inline UHttpServerFrame: TUHttpServerFrame
        Left = 2
        Top = 15
        Width = 694
        Height = 302
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        ExplicitLeft = 2
        ExplicitTop = 15
        ExplicitWidth = 694
        ExplicitHeight = 302
        inherited Image1: TImage
          Width = 694
          Height = 302
          Proportional = True
          Stretch = True
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 694
          ExplicitHeight = 302
        end
        inherited IdHTTPServer: TIdHTTPServer
          OnCommandGet = UHttpServerFrameIdHTTPServerCommandGet
        end
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 382
    Width = 702
    Height = 41
    Align = alBottom
    TabOrder = 1
    DesignSize = (
      702
      41)
    object StopButton: TButton
      Left = 665
      Top = 6
      Width = 30
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Stop'
      TabOrder = 0
      OnClick = StopButtonClick
    end
    object StartButton: TButton
      Left = 629
      Top = 6
      Width = 30
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Start'
      TabOrder = 1
      OnClick = StartButtonClick
    end
    object TimeEdit: TMaskEdit
      Left = 544
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
      Width = 536
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
    Top = 336
    Width = 702
    Height = 46
    Align = alBottom
    TabOrder = 2
    DesignSize = (
      702
      46)
    object SendToEdit: TEdit
      Left = 16
      Top = 4
      Width = 437
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      Color = clBtnFace
      ReadOnly = True
      TabOrder = 0
    end
    object Button1: TButton
      Left = 544
      Top = 2
      Width = 79
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Send points'
      TabOrder = 1
      OnClick = Button1Click
    end
    object SelectSendToButton: TButton
      Left = 629
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
      Left = 459
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
    Top = 423
    Width = 702
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
  object Timer: TTimer
    Enabled = False
    Interval = 10
    OnTimer = TimerTimer
    Left = 8
    Top = 264
  end
  object PopupMenu: TPopupMenu
    Left = 48
    Top = 264
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
    Left = 120
    Top = 272
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
end
