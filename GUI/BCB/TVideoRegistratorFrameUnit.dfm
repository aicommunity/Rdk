inherited TVideoRegistratorFrame: TTVideoRegistratorFrame
  Width = 948
  Height = 631
  ExplicitWidth = 948
  ExplicitHeight = 631
  object Splitter1: TSplitter
    Left = 565
    Top = 0
    Height = 425
    Align = alRight
    ExplicitLeft = 570
  end
  object Splitter2: TSplitter
    Left = 0
    Top = 425
    Width = 948
    Height = 6
    Cursor = crVSplit
    Align = alBottom
    ExplicitTop = 1392
    ExplicitWidth = 1035
  end
  object VideoPanel: TPanel
    Left = 0
    Top = 0
    Width = 565
    Height = 425
    Align = alClient
    Constraints.MinWidth = 525
    TabOrder = 0
    object PreviewImage: TImage
      Left = 1
      Top = 1
      Width = 563
      Height = 370
      Align = alClient
      Stretch = True
      ExplicitLeft = 8
      ExplicitTop = 8
      ExplicitWidth = 519
      ExplicitHeight = 429
    end
    object VideoGrabber: TVideoGrabber
      Left = 1
      Top = 1
      Width = 563
      Height = 370
      Align = alClient
      Caption = 'VideoGrabber'
      Color = clBlack
      Visible = False
      AspectRatioToUse = -1.000000000000000000
      AudioCompressor = 0
      AutoFileNameDateTimeFormat = 'yymmdd_hhmmss_zzz'
      AutoFilePrefix = 'vg'
      BurstCount = 0
      BurstMode = True
      Display_Active = False
      Cropping_Zoom = 1.000000000000000000
      FrameGrabber = fg_PreviewStream
      LicenseString = 'N/A'
      MotionDetector_Grid = 
        '5555555555 5555555555 5555555555 5555555555 5555555555 555555555' +
        '5 5555555555 5555555555 5555555555 5555555555'
      NetworkStreamingType = nst_VideoStreaming
      PlayerSpeedRatio = 1.000000000000000000
      Reencoding_StartTime = -1
      Reencoding_StartFrame = -1
      Reencoding_StopTime = -1
      Reencoding_StopFrame = -1
      Reencoding_UseVideoCompressor = True
      TextOverlay_Font.Charset = DEFAULT_CHARSET
      TextOverlay_Font.Color = clAqua
      TextOverlay_Font.Height = -16
      TextOverlay_Font.Name = 'Arial'
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
      VideoSource_FileOrURL_StartTime = -1
      VideoSource_FileOrURL_StopTime = -1
      OnAVIDurationUpdated = VideoGrabberAVIDurationUpdated
      OnFrameCaptureCompleted = VideoGrabberFrameCaptureCompleted
      OnVideoFromBitmapsNextFrameNeeded = VideoGrabberVideoFromBitmapsNextFrameNeeded
    end
    object VideoControlGroupBox: TGroupBox
      Left = 1
      Top = 371
      Width = 563
      Height = 53
      Align = alBottom
      Caption = 'Video Control'
      TabOrder = 1
      object ClearMemoButton: TButton
        Left = 359
        Top = 16
        Width = 159
        Height = 25
        Caption = 'Clear Memo'
        TabOrder = 0
        OnClick = ClearMemoButtonClick
      end
      object InitButton: TButton
        Left = 160
        Top = 16
        Width = 73
        Height = 25
        Caption = 'Init'
        TabOrder = 1
        Visible = False
        OnClick = InitButtonClick
      end
      object SaveToIniButton: TButton
        Left = 240
        Top = 16
        Width = 113
        Height = 25
        Caption = 'Save to ini'
        TabOrder = 2
        Visible = False
        OnClick = SaveToIniButtonClick
      end
      object StartPreviewButton: TButton
        Left = 0
        Top = 16
        Width = 75
        Height = 25
        Caption = 'Preview'
        TabOrder = 3
        OnClick = StartPreviewButtonClick
      end
      object StopButton: TButton
        Left = 80
        Top = 16
        Width = 75
        Height = 25
        Caption = 'Stop Preview'
        TabOrder = 4
        OnClick = StopButtonClick
      end
    end
  end
  object ControlPanel: TPanel
    Left = 568
    Top = 0
    Width = 380
    Height = 425
    Align = alRight
    Constraints.MinHeight = 425
    Constraints.MinWidth = 380
    TabOrder = 1
    object PageControl: TPageControl
      Left = 1
      Top = 154
      Width = 378
      Height = 270
      ActivePage = RecordTabSheet
      Align = alClient
      TabOrder = 0
      object NetworkStreamingTabSheet: TTabSheet
        Caption = 'Network Streaming'
        object StreamingSettingsGroupBox: TGroupBox
          Left = 3
          Top = 3
          Width = 358
          Height = 205
          Caption = 'Network streaming settings'
          TabOrder = 0
          object BitRateLabeledEdit: TLabeledEdit
            Left = 8
            Top = 72
            Width = 140
            Height = 21
            EditLabel.Width = 100
            EditLabel.Height = 13
            EditLabel.Caption = 'Bit Rate, bits per sec'
            TabOrder = 0
            Text = '2000000'
          end
          object BufferWindowLabeledEdit: TLabeledEdit
            Left = 160
            Top = 32
            Width = 140
            Height = 21
            EditLabel.Width = 91
            EditLabel.Height = 13
            EditLabel.Caption = 'Buffer Window, ms'
            TabOrder = 1
            Text = '100'
          end
          object FrameRateLabeledEdit: TLabeledEdit
            Left = 8
            Top = 112
            Width = 140
            Height = 21
            EditLabel.Width = 56
            EditLabel.Height = 13
            EditLabel.Caption = 'Frame Rate'
            TabOrder = 2
            Text = '24'
          end
          object MaxKeyFrameSpacingLabeledEdit: TLabeledEdit
            Left = 160
            Top = 72
            Width = 140
            Height = 21
            EditLabel.Width = 114
            EditLabel.Height = 13
            EditLabel.Caption = 'Max Key Frame Spacing'
            TabOrder = 3
            Text = '100'
          end
          object PortLabeledEdit: TLabeledEdit
            Left = 8
            Top = 32
            Width = 140
            Height = 21
            EditLabel.Width = 20
            EditLabel.Height = 13
            EditLabel.Caption = 'Port'
            TabOrder = 4
            Text = '45040'
          end
          object VideoHeightLabeledEdit: TLabeledEdit
            Left = 8
            Top = 152
            Width = 140
            Height = 21
            EditLabel.Width = 60
            EditLabel.Height = 13
            EditLabel.Caption = 'Video Height'
            TabOrder = 5
            Text = '288'
          end
          object VideoWidthLabeledEdit: TLabeledEdit
            Left = 160
            Top = 152
            Width = 140
            Height = 21
            EditLabel.Width = 57
            EditLabel.Height = 13
            EditLabel.Caption = 'Video Width'
            TabOrder = 6
            Text = '384'
          end
          object MaxUsersLabeledEdit: TLabeledEdit
            Left = 160
            Top = 112
            Width = 140
            Height = 21
            EditLabel.Width = 50
            EditLabel.Height = 13
            EditLabel.Caption = 'Max Users'
            TabOrder = 7
            Text = '4'
          end
        end
        object GetStreamingHostButton: TButton
          Left = 119
          Top = 214
          Width = 123
          Height = 25
          Caption = 'Get Streaming Host'
          TabOrder = 1
          OnClick = GetStreamingHostButtonClick
        end
        object NetworkStreamingButton: TButton
          Left = 3
          Top = 214
          Width = 110
          Height = 25
          Caption = 'Network Streaming'
          TabOrder = 2
          OnClick = NetworkStreamingButtonClick
        end
        object StopNetworkStreamingButton: TButton
          Left = 248
          Top = 214
          Width = 113
          Height = 25
          Caption = 'Stop Streaming'
          TabOrder = 3
          OnClick = StopNetworkStreamingButtonClick
        end
      end
      object RecordTabSheet: TTabSheet
        Caption = 'RecordTabSheet'
        ImageIndex = 1
        object RecordingMethodLabel: TLabel
          Left = 3
          Top = 5
          Width = 87
          Height = 13
          Caption = 'Recording Method'
        end
        object StartRecordingButton: TButton
          Left = 3
          Top = 139
          Width = 95
          Height = 25
          Caption = 'Start Recording'
          TabOrder = 0
          OnClick = StartRecordingButtonClick
        end
        object StopRecordingButton: TButton
          Left = 104
          Top = 139
          Width = 89
          Height = 25
          Caption = 'Stop Recording'
          TabOrder = 1
          OnClick = StopRecordingButtonClick
        end
        object RecordingMethodComboBox: TComboBox
          Left = 3
          Top = 24
          Width = 190
          Height = 21
          TabOrder = 2
        end
        object RecordingFileNameLabeledEdit: TLabeledEdit
          Left = 3
          Top = 72
          Width = 190
          Height = 21
          EditLabel.Width = 97
          EditLabel.Height = 13
          EditLabel.Caption = 'Recording File Name'
          TabOrder = 3
        end
        object RecordingFrameRateLabeledEdit: TLabeledEdit
          Left = 3
          Top = 112
          Width = 190
          Height = 21
          EditLabel.Width = 107
          EditLabel.Height = 13
          EditLabel.Caption = 'Recording Frame Rate'
          TabOrder = 4
        end
      end
    end
    object SourceControlGroupBox: TGroupBox
      Left = 1
      Top = 1
      Width = 378
      Height = 153
      Align = alTop
      Caption = 'Source Control'
      TabOrder = 1
      object SourceModeLabel: TLabel
        Left = 3
        Top = 107
        Width = 59
        Height = 13
        Caption = 'SourceMode'
      end
      object ComponentNameLabeledEdit: TLabeledEdit
        Left = 3
        Top = 32
        Width = 158
        Height = 21
        EditLabel.Width = 85
        EditLabel.Height = 13
        EditLabel.Caption = 'Component Name'
        TabOrder = 0
      end
      object ComponentPropertyNameLabeledEdit: TLabeledEdit
        Left = 211
        Top = 32
        Width = 158
        Height = 21
        EditLabel.Width = 100
        EditLabel.Height = 13
        EditLabel.Caption = 'Component Property'
        TabOrder = 1
      end
      object FrameIndexLabeledEdit: TLabeledEdit
        Left = 3
        Top = 75
        Width = 158
        Height = 21
        EditLabel.Width = 61
        EditLabel.Height = 13
        EditLabel.Caption = 'Frame Index'
        TabOrder = 2
        Text = '0'
      end
      object ChannelIndexLabeledEdit: TLabeledEdit
        Left = 211
        Top = 75
        Width = 158
        Height = 21
        EditLabel.Width = 70
        EditLabel.Height = 13
        EditLabel.Caption = 'Channel Index'
        TabOrder = 3
        Text = '-1'
      end
      object SorceModeComboBox: TComboBox
        Left = 3
        Top = 126
        Width = 158
        Height = 21
        ItemIndex = 1
        TabOrder = 4
        Text = 'Frame'
        Items.Strings = (
          'Component'
          'Frame')
      end
    end
  end
  object LogPanel: TPanel
    Left = 0
    Top = 431
    Width = 948
    Height = 200
    Align = alBottom
    TabOrder = 2
    object LogMemo: TMemo
      Left = 1
      Top = 1
      Width = 946
      Height = 198
      Align = alClient
      ScrollBars = ssBoth
      TabOrder = 0
    end
  end
end
