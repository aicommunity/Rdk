inherited TVideoRegistratorFrame: TTVideoRegistratorFrame
  Width = 996
  Height = 686
  ExplicitWidth = 996
  ExplicitHeight = 686
  object Splitter1: TSplitter
    Left = 613
    Top = 0
    Height = 686
    Align = alRight
    ExplicitLeft = 570
    ExplicitHeight = 425
  end
  object ControlPanel: TPanel
    Left = 616
    Top = 0
    Width = 380
    Height = 686
    Align = alRight
    Constraints.MinHeight = 480
    Constraints.MinWidth = 380
    TabOrder = 0
    object ModePageControl: TPageControl
      Left = 1
      Top = 194
      Width = 378
      Height = 491
      ActivePage = NetworkStreamingTabSheet
      Align = alClient
      TabOrder = 0
      object NetworkStreamingTabSheet: TTabSheet
        Caption = 'Network Streaming'
        object StreamingSettingsGroupBox: TGroupBox
          Left = 0
          Top = 0
          Width = 370
          Height = 408
          Align = alClient
          Caption = 'Network streaming settings'
          TabOrder = 0
          DesignSize = (
            370
            408)
          object BitRateLabeledEdit: TLabeledEdit
            Left = 3
            Top = 72
            Width = 173
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 100
            EditLabel.Height = 13
            EditLabel.Caption = 'Bit Rate, bits per sec'
            TabOrder = 0
            Text = '2000000'
          end
          object BufferWindowLabeledEdit: TLabeledEdit
            Left = 182
            Top = 32
            Width = 185
            Height = 21
            Anchors = [akTop, akRight]
            EditLabel.Width = 91
            EditLabel.Height = 13
            EditLabel.Caption = 'Buffer Window, ms'
            TabOrder = 1
            Text = '100'
          end
          object FrameRateLabeledEdit: TLabeledEdit
            Left = 3
            Top = 112
            Width = 173
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 56
            EditLabel.Height = 13
            EditLabel.Caption = 'Frame Rate'
            TabOrder = 2
            Text = '24'
          end
          object MaxKeyFrameSpacingLabeledEdit: TLabeledEdit
            Left = 182
            Top = 72
            Width = 185
            Height = 21
            Anchors = [akTop, akRight]
            EditLabel.Width = 114
            EditLabel.Height = 13
            EditLabel.Caption = 'Max Key Frame Spacing'
            TabOrder = 3
            Text = '100'
          end
          object PortLabeledEdit: TLabeledEdit
            Left = 3
            Top = 32
            Width = 173
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 20
            EditLabel.Height = 13
            EditLabel.Caption = 'Port'
            TabOrder = 4
            Text = '45040'
          end
          object VideoHeightLabeledEdit: TLabeledEdit
            Left = 182
            Top = 149
            Width = 185
            Height = 21
            Anchors = [akTop, akRight]
            EditLabel.Width = 60
            EditLabel.Height = 13
            EditLabel.Caption = 'Video Height'
            TabOrder = 5
            Text = '288'
          end
          object VideoWidthLabeledEdit: TLabeledEdit
            Left = 3
            Top = 149
            Width = 173
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 57
            EditLabel.Height = 13
            EditLabel.Caption = 'Video Width'
            TabOrder = 6
            Text = '384'
          end
          object MaxUsersLabeledEdit: TLabeledEdit
            Left = 182
            Top = 112
            Width = 185
            Height = 21
            Anchors = [akTop, akRight]
            EditLabel.Width = 50
            EditLabel.Height = 13
            EditLabel.Caption = 'Max Users'
            TabOrder = 7
            Text = '4'
          end
        end
        object NetworkStreamingControlGroupBox: TGroupBox
          Left = 0
          Top = 408
          Width = 370
          Height = 55
          Align = alBottom
          Caption = 'Network Streaming Control'
          TabOrder = 1
          DesignSize = (
            370
            55)
          object GetStreamingHostButton: TButton
            Left = 125
            Top = 24
            Width = 123
            Height = 25
            Anchors = [akTop]
            Caption = 'Get Streaming Host'
            TabOrder = 0
            OnClick = GetStreamingHostButtonClick
          end
          object NetworkStreamingButton: TButton
            Left = 3
            Top = 24
            Width = 116
            Height = 25
            Caption = 'Network Streaming'
            TabOrder = 1
            OnClick = NetworkStreamingButtonClick
          end
          object StopNetworkStreamingButton: TButton
            Left = 254
            Top = 24
            Width = 113
            Height = 25
            Anchors = [akTop, akRight]
            Caption = 'Stop Streaming'
            TabOrder = 2
            OnClick = StopNetworkStreamingButtonClick
          end
        end
      end
      object RecordingTabSheet: TTabSheet
        Caption = 'RecordingTabSheet'
        ImageIndex = 1
        object RecordingSettingsGroupBox: TGroupBox
          Left = 0
          Top = 0
          Width = 370
          Height = 408
          Align = alClient
          Caption = 'Recording Settings'
          TabOrder = 0
          DesignSize = (
            370
            408)
          object RecordingMethodLabel: TLabel
            Left = 3
            Top = 21
            Width = 87
            Height = 13
            Anchors = [akLeft, akTop, akRight]
            Caption = 'Recording Method'
            ExplicitWidth = 81
          end
          object VideoCompressorLabel: TLabel
            Left = 3
            Top = 67
            Width = 86
            Height = 13
            Anchors = [akLeft, akTop, akRight]
            Caption = 'Video Compressor'
            ExplicitWidth = 80
          end
          object RecordingFrameRateLabeledEdit: TLabeledEdit
            Left = 3
            Top = 218
            Width = 364
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 107
            EditLabel.Height = 13
            EditLabel.Caption = 'Recording Frame Rate'
            TabOrder = 0
          end
          object RecordingFileNameLabeledEdit: TLabeledEdit
            Left = 3
            Top = 259
            Width = 364
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 97
            EditLabel.Height = 13
            EditLabel.Caption = 'Recording File Name'
            TabOrder = 1
          end
          object VideoCompressorComboBox: TComboBox
            Left = 3
            Top = 89
            Width = 364
            Height = 21
            Anchors = [akLeft, akRight]
            TabOrder = 2
          end
          object RecordHeightLabeledEdit: TLabeledEdit
            Left = 3
            Top = 176
            Width = 364
            Height = 21
            Anchors = [akLeft, akRight]
            EditLabel.Width = 68
            EditLabel.Height = 13
            EditLabel.Caption = 'Record Height'
            TabOrder = 3
            Text = '480'
          end
          object RecordWidthLabeledEdit: TLabeledEdit
            Left = 3
            Top = 131
            Width = 364
            Height = 21
            Anchors = [akLeft, akRight]
            EditLabel.Width = 65
            EditLabel.Height = 13
            EditLabel.Caption = 'Record Width'
            TabOrder = 4
            Text = '640'
          end
          object RecordingMethodComboBox: TComboBox
            Left = 3
            Top = 40
            Width = 364
            Height = 21
            Anchors = [akLeft, akRight]
            TabOrder = 5
          end
          object BrowseFileNameButton: TButton
            Left = 3
            Top = 286
            Width = 75
            Height = 25
            Caption = 'Browse'
            TabOrder = 6
            OnClick = BrowseFileNameButtonClick
          end
        end
        object RecordingControlGroupBox: TGroupBox
          Left = 0
          Top = 408
          Width = 370
          Height = 55
          Align = alBottom
          Caption = 'Recording Control'
          TabOrder = 1
          object StartRecordingButton: TButton
            Left = 3
            Top = 20
            Width = 95
            Height = 25
            Caption = 'Start Recording'
            TabOrder = 0
            OnClick = StartRecordingButtonClick
          end
          object StopRecordingButton: TButton
            Left = 104
            Top = 20
            Width = 89
            Height = 25
            Caption = 'Stop Recording'
            TabOrder = 1
            OnClick = StopRecordingButtonClick
          end
        end
      end
    end
    object SourcePageControl: TPageControl
      Left = 1
      Top = 1
      Width = 378
      Height = 193
      ActivePage = SourceFrameTabSheet
      Align = alTop
      TabOrder = 1
      object SourceComponentTabSheet: TTabSheet
        Caption = 'Source Component'
        object ComponentSourceControlGroupBox: TGroupBox
          Left = 0
          Top = 0
          Width = 370
          Height = 165
          Align = alClient
          Caption = 'Component Source Control'
          TabOrder = 0
          DesignSize = (
            370
            165)
          object ComponentNameLabeledEdit: TLabeledEdit
            Left = 3
            Top = 32
            Width = 364
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 85
            EditLabel.Height = 13
            EditLabel.Caption = 'Component Name'
            TabOrder = 0
          end
          object ComponentPropertyNameLabeledEdit: TLabeledEdit
            Left = 3
            Top = 80
            Width = 364
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 100
            EditLabel.Height = 13
            EditLabel.Caption = 'Component Property'
            TabOrder = 1
          end
          object BrowseComponentButton: TButton
            Left = 3
            Top = 122
            Width = 142
            Height = 25
            Caption = 'Browse Component'
            TabOrder = 2
            OnClick = BrowseComponentButtonClick
          end
        end
      end
      object SourceFrameTabSheet: TTabSheet
        Caption = 'Source Frame'
        ImageIndex = 1
        object FrameSourceControlGroupBox: TGroupBox
          Left = 0
          Top = 0
          Width = 370
          Height = 165
          Align = alClient
          Caption = 'Frame Source Control'
          TabOrder = 0
          DesignSize = (
            370
            165)
          object ChannelIndexLabeledEdit: TLabeledEdit
            Left = 3
            Top = 75
            Width = 364
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 70
            EditLabel.Height = 13
            EditLabel.Caption = 'Channel Index'
            TabOrder = 0
            Text = '-1'
          end
          object FrameIndexLabeledEdit: TLabeledEdit
            Left = 3
            Top = 35
            Width = 364
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            EditLabel.Width = 61
            EditLabel.Height = 13
            EditLabel.Caption = 'Frame Index'
            TabOrder = 1
            Text = '0'
          end
        end
      end
    end
  end
  object VideoPanel: TPanel
    Left = 0
    Top = 0
    Width = 613
    Height = 686
    Align = alClient
    Constraints.MinWidth = 525
    TabOrder = 1
    object PreviewImage: TImage
      Left = 1
      Top = 1
      Width = 611
      Height = 631
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
      Width = 611
      Height = 631
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
      Reencoding_Method = rm_AVI
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
      VideoSource = vs_JPEGsOrBitmaps
      VideoSource_FileOrURL_StartTime = -1
      VideoSource_FileOrURL_StopTime = -1
      OnAVIDurationUpdated = VideoGrabberAVIDurationUpdated
      OnFrameCaptureCompleted = VideoGrabberFrameCaptureCompleted
      OnVideoFromBitmapsNextFrameNeeded = VideoGrabberVideoFromBitmapsNextFrameNeeded
    end
    object VideoControlGroupBox: TGroupBox
      Left = 1
      Top = 632
      Width = 611
      Height = 53
      Align = alBottom
      Caption = 'Video Control'
      TabOrder = 1
      object InitButton: TButton
        Left = 160
        Top = 16
        Width = 73
        Height = 25
        Caption = 'Init'
        TabOrder = 0
        Visible = False
        OnClick = InitButtonClick
      end
      object StartPreviewButton: TButton
        Left = 0
        Top = 16
        Width = 75
        Height = 25
        Caption = 'Preview'
        TabOrder = 1
        OnClick = StartPreviewButtonClick
      end
      object StopButton: TButton
        Left = 80
        Top = 16
        Width = 75
        Height = 25
        Caption = 'Stop Preview'
        TabOrder = 2
        OnClick = StopButtonClick
      end
    end
  end
  object RecordingFileOpen: TFileOpenDialog
    FavoriteLinks = <>
    FileTypes = <>
    Options = []
    Left = 720
    Top = 504
  end
end
