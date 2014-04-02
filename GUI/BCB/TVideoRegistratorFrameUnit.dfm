inherited TVideoRegistratorFrame: TTVideoRegistratorFrame
  Width = 1003
  Height = 789
  ExplicitWidth = 1003
  ExplicitHeight = 789
  object Splitter1: TSplitter
    Left = 620
    Top = 0
    Height = 789
    Align = alRight
    ExplicitLeft = 570
    ExplicitHeight = 425
  end
  object ControlPanel: TPanel
    Left = 623
    Top = 0
    Width = 380
    Height = 789
    Align = alRight
    Constraints.MinHeight = 480
    Constraints.MinWidth = 380
    TabOrder = 0
    object ModePageControl: TPageControl
      Left = 1
      Top = 166
      Width = 378
      Height = 622
      ActivePage = RecordingTabSheet
      Align = alClient
      TabOrder = 0
      object NetworkStreamingTabSheet: TTabSheet
        Caption = 'Network Streaming'
        object StreamingSettingsGroupBox: TGroupBox
          Left = 0
          Top = 0
          Width = 370
          Height = 539
          Align = alClient
          Caption = 'Network streaming settings'
          TabOrder = 0
          DesignSize = (
            370
            539)
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
          Top = 539
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
        object RecordingSettingsScrollBox: TScrollBox
          Left = 0
          Top = 0
          Width = 370
          Height = 594
          Align = alClient
          TabOrder = 0
          object GroupBox1: TGroupBox
            Left = 0
            Top = 0
            Width = 366
            Height = 561
            Align = alTop
            TabOrder = 0
            object RecordingControlGroupBox: TGroupBox
              Left = 2
              Top = 505
              Width = 362
              Height = 54
              Align = alClient
              Caption = 'Recording Control'
              TabOrder = 0
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
            object RecordingSettingsGroupBox: TGroupBox
              Left = 2
              Top = 15
              Width = 362
              Height = 490
              Align = alTop
              Caption = 'Recording Settings'
              TabOrder = 1
              DesignSize = (
                362
                490)
              object RecordingMethodLabel: TLabel
                Left = 3
                Top = 21
                Width = 87
                Height = 13
                Anchors = [akLeft, akTop, akRight]
                Caption = 'Recording Method'
              end
              object VideoCompressorLabel: TLabel
                Left = 3
                Top = 71
                Width = 86
                Height = 13
                Anchors = [akLeft, akTop, akRight]
                Caption = 'Video Compressor'
              end
              object VideoCompressionModeLabel: TLabel
                Left = 3
                Top = 117
                Width = 119
                Height = 13
                Caption = 'Video Compression Mode'
              end
              object RecordingFrameRateLabeledEdit: TLabeledEdit
                Left = 3
                Top = 218
                Width = 166
                Height = 21
                Anchors = [akLeft, akTop, akRight]
                EditLabel.Width = 107
                EditLabel.Height = 13
                EditLabel.Caption = 'Recording Frame Rate'
                TabOrder = 0
              end
              object RecordingFileNameLabeledEdit: TLabeledEdit
                Left = 175
                Top = 218
                Width = 184
                Height = 21
                Anchors = [akTop, akRight]
                EditLabel.Width = 97
                EditLabel.Height = 13
                EditLabel.Caption = 'Recording File Name'
                TabOrder = 1
              end
              object VideoCompressorComboBox: TComboBox
                Left = 3
                Top = 90
                Width = 356
                Height = 21
                Style = csDropDownList
                Anchors = [akLeft, akTop, akRight]
                TabOrder = 2
                OnChange = VideoCompressorComboBoxChange
              end
              object RecordHeightLabeledEdit: TLabeledEdit
                Left = 175
                Top = 179
                Width = 184
                Height = 21
                Anchors = [akTop, akRight]
                EditLabel.Width = 68
                EditLabel.Height = 13
                EditLabel.Caption = 'Record Height'
                TabOrder = 3
                Text = '480'
              end
              object RecordWidthLabeledEdit: TLabeledEdit
                Left = 3
                Top = 179
                Width = 166
                Height = 21
                Anchors = [akLeft, akTop, akRight]
                EditLabel.Width = 65
                EditLabel.Height = 13
                EditLabel.Caption = 'Record Width'
                TabOrder = 4
                Text = '640'
              end
              object RecordingMethodComboBox: TComboBox
                Left = 3
                Top = 40
                Width = 356
                Height = 21
                Style = csDropDownList
                Anchors = [akLeft, akTop, akRight]
                ItemIndex = 0
                TabOrder = 5
                Text = 'rm_AVI'
                OnChange = RecordingMethodComboBoxChange
                Items.Strings = (
                  'rm_AVI'
                  'rm_ASF'
                  'rm_FLV'
                  'rm_MKV'
                  'rm_MOV'
                  'rm_MP4'
                  'rm_MPG'
                  'rm_Multiplexer'
                  'rm_SendToDV'
                  'rm_WebM')
              end
              object BrowseFileNameButton: TButton
                Left = 175
                Top = 245
                Width = 184
                Height = 25
                Anchors = [akTop, akRight]
                Caption = 'Browse File Name'
                TabOrder = 6
                OnClick = BrowseFileNameButtonClick
              end
              object VideoCompressorSettingsButton: TButton
                Left = 3
                Top = 245
                Width = 166
                Height = 25
                Anchors = [akLeft, akTop, akRight]
                Caption = 'Video Compressor Settings'
                TabOrder = 7
                OnClick = VideoCompressorSettingsButtonClick
              end
              object VideoCompressionModeComboBox: TComboBox
                Left = 3
                Top = 136
                Width = 356
                Height = 21
                Style = csDropDownList
                Anchors = [akLeft, akTop, akRight]
                ItemIndex = 0
                TabOrder = 8
                Text = 'no'
                Items.Strings = (
                  'no'
                  'on the fly'
                  'after capture')
              end
              object RecordingTimerGroupBox: TGroupBox
                Left = 3
                Top = 342
                Width = 356
                Height = 53
                Anchors = [akLeft, akTop, akRight]
                Caption = 'Recording Timer'
                TabOrder = 9
                DesignSize = (
                  356
                  53)
                object RecordingModeComboBox: TComboBox
                  Left = 3
                  Top = 24
                  Width = 166
                  Height = 21
                  Style = csDropDownList
                  Anchors = [akLeft, akTop, akRight]
                  ItemIndex = 0
                  TabOrder = 0
                  Text = 'disabled'
                  Items.Strings = (
                    'disabled'
                    'new file every...'
                    'stop after ...'
                    'start after ...')
                end
                object RecordingTimerLabeledEdit: TLabeledEdit
                  Left = 175
                  Top = 24
                  Width = 178
                  Height = 21
                  Anchors = [akTop, akRight]
                  EditLabel.Width = 77
                  EditLabel.Height = 13
                  EditLabel.Caption = 'Recording Timer'
                  TabOrder = 1
                  Text = '10'
                end
              end
              object PreAllocatedRecordingGroupBox: TGroupBox
                Left = 3
                Top = 401
                Width = 373
                Height = 83
                Anchors = [akLeft, akTop, akRight, akBottom]
                Caption = 'Preallocated Recording'
                TabOrder = 10
                DesignSize = (
                  373
                  83)
                object UsePreallocatedFileCheckBox: TCheckBox
                  Left = 3
                  Top = 19
                  Width = 186
                  Height = 17
                  Caption = 'Use PreallocatedFileCheckBox'
                  TabOrder = 0
                end
                object PreallocatedFileSizeLabeledEdit: TLabeledEdit
                  Left = 3
                  Top = 57
                  Width = 166
                  Height = 21
                  Anchors = [akLeft, akTop, akRight]
                  EditLabel.Width = 100
                  EditLabel.Height = 13
                  EditLabel.Caption = 'Preallocated File Size'
                  TabOrder = 1
                  Text = '100'
                end
                object PreallocatedFileCreateButton: TButton
                  Left = 175
                  Top = 54
                  Width = 182
                  Height = 25
                  Anchors = [akTop, akRight]
                  Caption = 'Create Preallocated File Now'
                  TabOrder = 2
                  OnClick = PreallocatedFileCreateButtonClick
                end
              end
              object StoragePathRadioGroup: TRadioGroup
                Left = 3
                Top = 270
                Width = 166
                Height = 66
                Anchors = [akLeft, akTop, akRight]
                Caption = 'Storage path'
                ItemIndex = 0
                Items.Strings = (
                  'to projects folder'
                  'to custom folder')
                TabOrder = 11
                OnClick = StoragePathRadioGroupClick
              end
              object StoragePathLabeledEdit: TLabeledEdit
                Left = 175
                Top = 284
                Width = 184
                Height = 21
                Anchors = [akTop, akRight]
                EditLabel.Width = 63
                EditLabel.Height = 13
                EditLabel.Caption = 'Storage path'
                TabOrder = 12
              end
              object BrowseStoragePathButton: TButton
                Left = 175
                Top = 311
                Width = 184
                Height = 25
                Anchors = [akTop, akRight]
                Caption = 'Browse Storage Path'
                TabOrder = 13
                OnClick = BrowseStoragePathButtonClick
              end
            end
          end
        end
      end
    end
    object SourcePageControl: TPageControl
      Left = 1
      Top = 1
      Width = 378
      Height = 165
      ActivePage = SourceComponentTabSheet
      Align = alTop
      Constraints.MaxHeight = 165
      TabOrder = 1
      object SourceComponentTabSheet: TTabSheet
        Caption = 'Source Component'
        object ComponentSourceControlGroupBox: TGroupBox
          Left = 0
          Top = 0
          Width = 370
          Height = 137
          Align = alClient
          Caption = 'Component Source Control'
          TabOrder = 0
          DesignSize = (
            370
            137)
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
            Top = 72
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
            Top = 99
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
          Height = 137
          Align = alClient
          Caption = 'Frame Source Control'
          TabOrder = 0
          DesignSize = (
            370
            137)
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
    Width = 620
    Height = 789
    Align = alClient
    Constraints.MinWidth = 525
    TabOrder = 1
    object PreviewImage: TImage
      Left = 1
      Top = 1
      Width = 618
      Height = 734
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
      Width = 618
      Height = 734
      Align = alClient
      Caption = 'VideoGrabber'
      Color = clBlack
      Visible = False
      ASFVideoWidth = -1
      AspectRatioToUse = -1.000000000000000000
      AudioCompressor = 0
      AutoFileNameDateTimeFormat = 'yymmdd_hhmmss_zzz'
      AutoFilePrefix = 'vg'
      BurstCount = 0
      BurstMode = True
      Display_Active = False
      DualDisplay_Active = False
      DualDisplay_Embedded = False
      DualDisplay_Left = 20
      DualDisplay_Top = 400
      DualDisplay_VideoPortEnabled = False
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
      Top = 735
      Width = 618
      Height = 53
      Align = alBottom
      Caption = 'Video Control'
      TabOrder = 1
      Visible = False
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
    Left = 424
    Top = 640
  end
  object StoragePathOpenDialog: TOpenDialog
    Left = 496
    Top = 600
  end
end
