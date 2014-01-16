inherited TVideoRegistratorFrame: TTVideoRegistratorFrame
  Width = 348
  Height = 762
  ExplicitWidth = 348
  ExplicitHeight = 762
  object PageControl: TPageControl
    Left = 3
    Top = 495
    Width = 335
    Height = 253
    ActivePage = NetworkStreamingTabSheet
    TabOrder = 0
    object NetworkStreamingTabSheet: TTabSheet
      Caption = 'Network Streaming'
      object StreamingSettingsGroupBox: TGroupBox
        Left = 3
        Top = 3
        Width = 316
        Height = 179
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
        Top = 188
        Width = 106
        Height = 25
        Caption = 'Get Streaming Host'
        TabOrder = 1
        OnClick = GetStreamingHostButtonClick
      end
      object NetworkStreamingButton: TButton
        Left = 3
        Top = 188
        Width = 110
        Height = 25
        Caption = 'Network Streaming'
        TabOrder = 2
        OnClick = NetworkStreamingButtonClick
      end
      object StopNetworkStreamingButton: TButton
        Left = 231
        Top = 188
        Width = 88
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
        Width = 95
        Height = 21
        EditLabel.Width = 97
        EditLabel.Height = 13
        EditLabel.Caption = 'Recording File Name'
        TabOrder = 3
      end
      object RecordingFrameRateLabeledEdit: TLabeledEdit
        Left = 3
        Top = 112
        Width = 95
        Height = 21
        EditLabel.Width = 107
        EditLabel.Height = 13
        EditLabel.Caption = 'Recording Frame Rate'
        TabOrder = 4
      end
    end
  end
  object BasicPanel: TPanel
    Left = 1
    Top = 3
    Width = 337
    Height = 486
    TabOrder = 1
    object PreviewImage: TImage
      Left = 8
      Top = 8
      Width = 321
      Height = 240
      Stretch = True
    end
    object LogMemo: TMemo
      Left = 8
      Top = 248
      Width = 321
      Height = 113
      ScrollBars = ssBoth
      TabOrder = 0
    end
    object VideoGrabber: TVideoGrabber
      Left = 9
      Top = 2
      Width = 321
      Height = 240
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
    object InitButton: TButton
      Left = 5
      Top = 367
      Width = 102
      Height = 25
      Caption = 'Init'
      TabOrder = 2
      OnClick = InitButtonClick
    end
    object SaveToIniButton: TButton
      Left = 113
      Top = 367
      Width = 96
      Height = 25
      Caption = 'Save to ini'
      TabOrder = 3
      OnClick = SaveToIniButtonClick
    end
    object ClearMemoButton: TButton
      Left = 215
      Top = 367
      Width = 114
      Height = 25
      Caption = 'Clear Memo'
      TabOrder = 4
      OnClick = ClearMemoButtonClick
    end
    object ComponentNameLabeledEdit: TLabeledEdit
      Left = 6
      Top = 408
      Width = 157
      Height = 21
      EditLabel.Width = 85
      EditLabel.Height = 13
      EditLabel.Caption = 'Component Name'
      TabOrder = 5
    end
    object ComponentPropertyNameLabeledEdit: TLabeledEdit
      Left = 169
      Top = 408
      Width = 160
      Height = 21
      EditLabel.Width = 100
      EditLabel.Height = 13
      EditLabel.Caption = 'Component Property'
      TabOrder = 6
    end
    object FrameIndexLabeledEdit: TLabeledEdit
      Left = 9
      Top = 456
      Width = 154
      Height = 21
      EditLabel.Width = 61
      EditLabel.Height = 13
      EditLabel.Caption = 'Frame Index'
      TabOrder = 7
    end
    object StartPreviewButton: TButton
      Left = 169
      Top = 435
      Width = 75
      Height = 25
      Caption = 'Preview'
      TabOrder = 8
      OnClick = StartPreviewButtonClick
    end
    object StopButton: TButton
      Left = 254
      Top = 435
      Width = 75
      Height = 25
      Caption = 'Stop'
      TabOrder = 9
      OnClick = StopButtonClick
    end
  end
end
