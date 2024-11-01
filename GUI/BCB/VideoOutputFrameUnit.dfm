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
    Width = 779
    Height = 714
    ActivePage = VideoSourceTabSheet
    Align = alClient
    TabOrder = 0
    object VideoSourceTabSheet: TTabSheet
      Caption = 'Video Source'
      object GroupBox: TGroupBox
        Left = 0
        Top = 0
        Width = 771
        Height = 570
        Align = alClient
        DoubleBuffered = True
        ParentDoubleBuffered = False
        TabOrder = 0
        object Image: TImage
          Left = 2
          Top = 15
          Width = 767
          Height = 553
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
          Width = 767
          Height = 553
          Align = alClient
          Caption = 'GroupBox1'
          TabOrder = 0
          Visible = False
        end
      end
      object Panel1: TPanel
        Left = 0
        Top = 570
        Width = 771
        Height = 35
        Align = alBottom
        TabOrder = 1
        DesignSize = (
          771
          35)
        object StopButton: TButton
          Left = 662
          Top = 6
          Width = 30
          Height = 25
          Anchors = [akTop, akRight]
          Caption = 'Stop'
          TabOrder = 0
          OnClick = StopButtonClick
        end
        object StartButton: TButton
          Left = 626
          Top = 6
          Width = 30
          Height = 25
          Anchors = [akTop, akRight]
          Caption = 'Start'
          TabOrder = 1
          OnClick = StartButtonClick
        end
        object TimeEdit: TMaskEdit
          Left = 541
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
          Left = 5
          Top = 9
          Width = 530
          Height = 22
          Anchors = [akLeft, akTop, akRight]
          Max = 1
          Min = 1
          PageSize = 1
          Position = 1
          TabOrder = 3
          OnChange = TrackBarChange
        end
        object FrameLeftButton: TButton
          Left = 698
          Top = 6
          Width = 30
          Height = 25
          Anchors = [akTop, akRight]
          Caption = '<'
          Enabled = False
          TabOrder = 4
          OnClick = FrameLeftButtonClick
        end
        object FrameRightButton: TButton
          Left = 734
          Top = 6
          Width = 30
          Height = 25
          Anchors = [akTop, akRight]
          Caption = '>'
          Enabled = False
          TabOrder = 5
          OnClick = FrameRightButtonClick
        end
      end
      object Panel2: TPanel
        Left = 0
        Top = 605
        Width = 771
        Height = 46
        Align = alBottom
        TabOrder = 2
        DesignSize = (
          771
          46)
        object SendToEdit: TEdit
          Left = 5
          Top = 4
          Width = 530
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          Color = clBtnFace
          ReadOnly = True
          TabOrder = 0
        end
        object Button1: TButton
          Left = 626
          Top = 2
          Width = 66
          Height = 25
          Anchors = [akTop, akRight]
          Caption = 'Send points'
          TabOrder = 1
          OnClick = Button1Click
        end
        object SelectSendToButton: TButton
          Left = 698
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
          Left = 541
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
        Width = 771
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
    object NetworkStreamingTabSheet: TTabSheet
      Caption = 'Network Streaming'
      ImageIndex = 1
      inline NetworkStreamingFrame: TTVideoRegistratorFrame
        Left = 0
        Top = 0
        Width = 771
        Height = 686
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        ExplicitWidth = 771
        ExplicitHeight = 686
        inherited Splitter1: TSplitter
          Left = 388
          Height = 686
          ExplicitLeft = 525
          ExplicitHeight = 659
        end
        inherited ControlPanel: TPanel
          Left = 391
          Height = 686
          ExplicitLeft = 391
          ExplicitHeight = 686
          inherited ModePageControl: TPageControl
            Height = 519
            ExplicitHeight = 519
            inherited NetworkStreamingTabSheet: TTabSheet
              inherited NetworkStreamingControlGroupBox: TGroupBox
                inherited NetworkStreamingButton: TButton
                  OnClick = NetworkStreamingFrameNetworkStreamingButtonClick
                end
              end
            end
            inherited RecordingTabSheet: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 24
              ExplicitWidth = 370
              ExplicitHeight = 491
              inherited RecordingSettingsScrollBox: TScrollBox
                Height = 491
                ExplicitHeight = 491
                inherited GroupBox1: TGroupBox
                  inherited RecordingControlGroupBox: TGroupBox
                    ExplicitWidth = 345
                    ExplicitHeight = 58
                  end
                  inherited RecordingSettingsGroupBox: TGroupBox
                    inherited RecordingFrameRateLabeledEdit: TLabeledEdit
                      ExplicitWidth = 149
                    end
                    inherited RecordingFileNameLabeledEdit: TLabeledEdit
                      ExplicitLeft = 158
                    end
                    inherited RecordHeightLabeledEdit: TLabeledEdit
                      ExplicitLeft = 158
                    end
                    inherited RecordWidthLabeledEdit: TLabeledEdit
                      ExplicitWidth = 149
                    end
                    inherited BrowseFileNameButton: TButton
                      ExplicitLeft = 158
                    end
                    inherited VideoCompressorSettingsButton: TButton
                      ExplicitWidth = 149
                    end
                    inherited RecordingTimerGroupBox: TGroupBox
                      inherited RecordingTimerLabeledEdit: TLabeledEdit
                        ExplicitLeft = 155
                      end
                    end
                    inherited PreAllocatedRecordingGroupBox: TGroupBox
                      inherited PreallocatedFileSizeLabeledEdit: TLabeledEdit
                        ExplicitWidth = 146
                      end
                      inherited PreallocatedFileCreateButton: TButton
                        ExplicitLeft = 155
                      end
                    end
                    inherited StoragePathRadioGroup: TRadioGroup
                      ExplicitWidth = 149
                    end
                    inherited StoragePathLabeledEdit: TLabeledEdit
                      ExplicitLeft = 158
                    end
                    inherited BrowseStoragePathButton: TButton
                      ExplicitLeft = 158
                    end
                  end
                end
              end
            end
          end
          inherited SourcePageControl: TPageControl
            ActivePage = NetworkStreamingFrame.SourceFrameTabSheet
            inherited SourceComponentTabSheet: TTabSheet
              TabVisible = False
            end
          end
        end
        inherited VideoPanel: TPanel
          Width = 388
          Height = 686
          Constraints.MinWidth = 388
          ExplicitWidth = 388
          ExplicitHeight = 686
          inherited PreviewImage: TImage
            Width = 386
            Height = 631
            ExplicitWidth = 386
            ExplicitHeight = 666
          end
          inherited VideoGrabber: TVideoGrabber
            Width = 386
            Height = 631
            ExplicitWidth = 386
            ExplicitHeight = 631
          end
          inherited VideoControlGroupBox: TGroupBox
            Top = 632
            Width = 386
            ExplicitTop = 632
            ExplicitWidth = 386
          end
        end
      end
    end
    object RecordingTabSheet: TTabSheet
      Caption = 'Recording'
      ImageIndex = 2
      inline RecordingFrame: TTVideoRegistratorFrame
        Left = 0
        Top = 0
        Width = 771
        Height = 686
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        ExplicitWidth = 771
        ExplicitHeight = 686
        inherited Splitter1: TSplitter
          Left = 388
          Height = 686
          ExplicitLeft = 525
          ExplicitHeight = 659
        end
        inherited ControlPanel: TPanel
          Left = 391
          Height = 686
          ExplicitLeft = 391
          ExplicitHeight = 686
          inherited ModePageControl: TPageControl
            Height = 519
            ActivePage = RecordingFrame.NetworkStreamingTabSheet
            ExplicitHeight = 519
            inherited NetworkStreamingTabSheet: TTabSheet
              ExplicitHeight = 491
              inherited StreamingSettingsGroupBox: TGroupBox
                Height = 436
                ExplicitHeight = 436
                DesignSize = (
                  370
                  436)
              end
              inherited NetworkStreamingControlGroupBox: TGroupBox
                Top = 436
                ExplicitTop = 436
              end
            end
            inherited RecordingTabSheet: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 24
              ExplicitWidth = 370
              ExplicitHeight = 500
              inherited RecordingSettingsScrollBox: TScrollBox
                VertScrollBar.Position = 54
                inherited GroupBox1: TGroupBox
                  Top = -54
                  Height = 550
                  ExplicitTop = -54
                  ExplicitHeight = 550
                  inherited RecordingControlGroupBox: TGroupBox
                    Height = 43
                    ExplicitWidth = 345
                    ExplicitHeight = 43
                  end
                  inherited RecordingSettingsGroupBox: TGroupBox
                    inherited RecordingFrameRateLabeledEdit: TLabeledEdit
                      ExplicitWidth = 149
                    end
                    inherited RecordingFileNameLabeledEdit: TLabeledEdit
                      ExplicitLeft = 158
                    end
                    inherited RecordHeightLabeledEdit: TLabeledEdit
                      ExplicitLeft = 158
                    end
                    inherited RecordWidthLabeledEdit: TLabeledEdit
                      ExplicitWidth = 149
                    end
                    inherited BrowseFileNameButton: TButton
                      ExplicitLeft = 158
                    end
                    inherited VideoCompressorSettingsButton: TButton
                      ExplicitWidth = 149
                    end
                    inherited RecordingTimerGroupBox: TGroupBox
                      inherited RecordingTimerLabeledEdit: TLabeledEdit
                        ExplicitLeft = 155
                      end
                    end
                    inherited PreAllocatedRecordingGroupBox: TGroupBox
                      inherited PreallocatedFileSizeLabeledEdit: TLabeledEdit
                        ExplicitWidth = 146
                      end
                      inherited PreallocatedFileCreateButton: TButton
                        ExplicitLeft = 155
                      end
                    end
                    inherited StoragePathRadioGroup: TRadioGroup
                      ExplicitWidth = 149
                    end
                    inherited StoragePathLabeledEdit: TLabeledEdit
                      ExplicitLeft = 158
                    end
                    inherited BrowseStoragePathButton: TButton
                      ExplicitLeft = 158
                    end
                  end
                end
              end
            end
          end
          inherited SourcePageControl: TPageControl
            ActivePage = RecordingFrame.SourceFrameTabSheet
            inherited SourceComponentTabSheet: TTabSheet
              TabVisible = False
            end
          end
        end
        inherited VideoPanel: TPanel
          Width = 388
          Height = 686
          Constraints.MinWidth = 388
          ExplicitWidth = 388
          ExplicitHeight = 686
          inherited PreviewImage: TImage
            Width = 386
            Height = 631
            ExplicitWidth = 523
            ExplicitHeight = 604
          end
          inherited VideoGrabber: TVideoGrabber
            Width = 386
            Height = 631
            ExplicitWidth = 386
            ExplicitHeight = 631
          end
          inherited VideoControlGroupBox: TGroupBox
            Top = 632
            Width = 386
            ExplicitTop = 632
            ExplicitWidth = 386
          end
        end
      end
    end
  end
  object ToolBar: TToolBar
    Left = 779
    Top = 0
    Width = 137
    Height = 714
    Align = alRight
    AutoSize = True
    ButtonHeight = 30
    ButtonWidth = 122
    Caption = 'ToolBar'
    DoubleBuffered = True
    Images = ImageList
    List = True
    ParentDoubleBuffered = False
    ShowCaptions = True
    TabOrder = 1
    object CaptureStartToolButton: TToolButton
      Left = 0
      Top = 0
      Caption = 'Start Capture'
      ImageIndex = 19
      Wrap = True
      OnClick = CaptureStartToolButtonClick
    end
    object CaptureStopToolButton: TToolButton
      Left = 0
      Top = 30
      Caption = 'Stop Capture'
      ImageIndex = 18
      OnClick = CaptureStopToolButtonClick
    end
    object ToolButton3: TToolButton
      Left = 0
      Top = 30
      Width = 8
      Caption = 'ToolButton3'
      ImageIndex = 25
      Wrap = True
      Style = tbsSeparator
    end
    object StartRecordingToolButton: TToolButton
      Left = 0
      Top = 68
      Caption = 'Start Record'
      DropdownMenu = RecordingPopupMenu
      ImageIndex = 2
      Wrap = True
      Style = tbsDropDown
      OnClick = StartRecordingToolButtonClick
    end
    object StopRecordingToolButton: TToolButton
      Left = 0
      Top = 98
      Caption = 'Stop Record'
      ImageIndex = 3
      OnClick = StopRecordingToolButtonClick
    end
    object ToolButton1: TToolButton
      Left = 0
      Top = 98
      Width = 8
      Caption = 'ToolButton1'
      ImageIndex = 25
      Wrap = True
      Style = tbsSeparator
    end
    object StartStreamingToolButton: TToolButton
      Left = 0
      Top = 136
      Caption = 'Start Streaming'
      ImageIndex = 4
      Wrap = True
      OnClick = StartStreamingToolButtonClick
    end
    object StopStreamingToolButton: TToolButton
      Left = 0
      Top = 166
      Caption = 'Stop Streaming'
      ImageIndex = 5
      Wrap = True
      OnClick = StopStreamingToolButtonClick
    end
    object SavePictureToolButton: TToolButton
      Left = 0
      Top = 196
      Action = SavePictureAction
      Caption = 'Save Picture (F12)'
      ImageIndex = 22
      Wrap = True
    end
    object SelectSourceToolButton: TToolButton
      Left = 0
      Top = 226
      Caption = 'Select Source'
      ImageIndex = 15
      OnClick = SelectSourceToolButtonClick
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
      Visible = False
      OnClick = SourceControl1Click
    end
    object SourceControl21: TMenuItem
      Caption = 'Source Control'
      OnClick = SourceControl21Click
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
  object ImageList: TImageList
    Left = 688
    Top = 456
    Bitmap = {
      494C010120006000100110001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000009000000001002000000000000090
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000080808000808080008080
      8000808080008080800080808000808080008080800080808000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000008080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF0000FFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF0000FFFF000000000000FFFF00FFFFFF000000
      0000FFFFFF000000000000000000000000000000000000000000000000008080
      8000000000000000000000000000808080000000000000FFFF00808080000000
      0000000000000000000000000000000000000000000000000000000000008080
      8000000000000000000000000000808080000000000000FFFF00808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FF000000FF000000FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000000000000000
      000000000000FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000000000FFFF
      FF0000FFFF000000000000000000000000000000000000000000000000008080
      800080808000808080008080800080808000000000000000000000FFFF000000
      0000000000000000000000000000000000000000000000000000000000008080
      800080808000808080008080800080808000000000000000000000FFFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FF000000FFFFFF00FFFFFF00FF000000FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000000000000000
      00000000000000FFFF00FFFFFF0000FFFF00FFFFFF0000000000FFFFFF0000FF
      FF0000000000000000000000000000000000000000000000000080808000FFFF
      FF00C0C0C000FFFFFF00C0C0C000FFFFFF008080800000000000000000000000
      000000000000000000000000000000000000000000000000000080808000C0C0
      C000FFFFFF00C0C0C000FFFFFF00C0C0C0008080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FF000000FFFFFF00FFFFFF00FF000000FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000000000000000
      000000000000FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF000000
      0000008080000000000000000000000000008080800080808000FFFFFF00C0C0
      C000FFFFFF000000FF00FFFFFF00C0C0C000FFFFFF0080808000808080000000
      0000000000000000000000000000000000008080800080808000C0C0C000FFFF
      FF00C0C0C000FFFFFF00C0C0C000FFFFFF00C0C0C00080808000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FF000000FF000000FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF000000000000000000000000000000000000000000FFFF
      00000000000000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00000000000000
      0000008080000000000000000000000000000000000080808000C0C0C000FFFF
      FF00C0C0C0000000FF00C0C0C000FFFFFF00C0C0C00080808000000000000000
      0000000000000000000000000000000000000000000080808000FFFFFF00C0C0
      C000FFFFFF00C0C0C000FFFFFF00C0C0C000FFFFFF0080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000FFFF0000FFFF
      0000FFFF00000000000000FFFF00FFFFFF0000FFFF000000000000FFFF000000
      0000008080000000000000000000000000000000000080808000FFFFFF000000
      FF000000FF000000FF000000FF000000FF00FFFFFF0080808000000000000000
      0000000000000000000000000000000000000000000080808000C0C0C0000000
      FF000000FF000000FF000000FF000000FF00C0C0C00080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF000000000000000000000000000000000000000000FFFF
      0000FFFF0000FFFF00000000000000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000008080000000000000000000000000000000000080808000C0C0C000FFFF
      FF00C0C0C0000000FF00C0C0C000FFFFFF00C0C0C00080808000000000000000
      0000000000000000000000000000000000000000000080808000FFFFFF00C0C0
      C000FFFFFF00C0C0C000FFFFFF00C0C0C000FFFFFF0080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF0000000000000000000000000000000000000000000000
      0000FFFF0000FFFF0000FFFF0000000000000000000000000000000000000000
      0000008080000000000000000000000000008080800080808000FFFFFF00C0C0
      C000FFFFFF000000FF00FFFFFF00C0C0C000FFFFFF0080808000808080000000
      0000000000000000000000000000000000008080800080808000C0C0C000FFFF
      FF00C0C0C000FFFFFF00C0C0C000FFFFFF00C0C0C00080808000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFF0000000000000000000000000000000000000000000000FF
      FF0000FFFF00000000000000000000000000000000000000000080808000FFFF
      FF00C0C0C000FFFFFF00C0C0C000FFFFFF008080800000000000000000000000
      000000000000000000000000000000000000000000000000000080808000C0C0
      C000FFFFFF00C0C0C000FFFFFF00C0C0C0008080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000008080000000000000000000000000000000000000000000000000008080
      8000808080008080800080808000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008080
      8000808080008080800080808000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF0000000000000000000000000000000000000000008080
      8000000000000000000000000000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008080
      8000000000000000000000000000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008080
      8000808080008080800000000000000000000000000000000000808080008080
      8000808080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000800000000000000000000000000000000000
      0000000000000000000080000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000008000
      0000000000000000000000000000000000008000000000000000000000000000
      00008000000000000000000000000000000000000000000000000000000000FF
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000080000000800000000000
      0000000000000000000080000000000000000000000080000000000000000000
      0000000000008000000000000000000000000000000000000000000000000000
      000000FF00000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000008000
      0000800000000000000000000000000000000000000000000000800000000000
      00000000000080000000800000000000000000000000000000000000000000FF
      00000000000000FF000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000008000
      0000000000000000000080000000800000000000000000000000000000000000
      0000800000008000000000000000000000000000000000000000000000000000
      000000FF00000000000000FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000008000
      0000000000000000000000000000000000000000000000000000000000008000
      00008000000000000000000000000000000000000000000000000000000000FF
      00000000000000FF00000000000000FF00000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000080
      8000008080000080800000808000008080000080800000808000008080000080
      8000008080000080800000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000800000000000
      0000000000000000000080000000800000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FF00000000000000FF00000000000000FF000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000008000000080000000000000000000
      0000000000000000000000000000000000000000000080000000000000000000
      00000000000000000000000000000000000000000000000000000000000000FF
      00000000000000FF00000000000000FF00000000000000FF0000000000000000
      0000000000000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000008000
      0000000000000000000000000000000000008000000000000000000000008000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FF00000000000000FF00000000000000FF000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000000000000000000080000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000000000FF
      00000000000000FF00000000000000FF00000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000080000000000000000000
      0000000000000000000000000000000000000000000080000000000000000000
      0000000000008000000000000000000000000000000000000000000000000000
      000000FF00000000000000FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000800000000000
      0000000000008000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000000000FF
      00000000000000FF000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000800000008000
      0000800000008000000000000000000000000000000080000000800000008000
      0000800000000000000000000000000000000000000080000000000000008000
      0000800000000000000000000000000000000000000000000000000000008000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FF00000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000800000008000
      0000800000000000000000000000000000000000000080000000800000008000
      0000000000000000000000000000000000000000000000000000000000008000
      0000000000000000000000000000000000008000000000000000000000000000
      00000000000080000000000000000000000000000000000000000000000000FF
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF000000000000000000000000000000000000000000000000000000000000FF
      FF00FFFFFF0000FFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      FF000000FF000000FF0000000000FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000000000000000000000000000000000000000000000000000000000FFFF
      FF0000FFFF00FFFFFF000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      FF000000FF000000FF0000000000FFFFFF000000000000000000008080000080
      8000008080000080800000808000008080000080800000808000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      FF000000FF000000FF0000000000FFFFFF000000000000FFFF00000000000080
      8000008080000080800000808000008080000080800000808000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000FFFFFF0000000000FFFFFF0000FFFF000000
      0000008080000080800000808000008080000080800000808000008080000080
      8000008080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFF00000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000FFFF00FFFFFF0000FF
      FF00000000000080800000808000008080000080800000808000008080000080
      8000008080000080800000000000000000000000000000000000000000000000
      00000000000000000000FFFF000000000000FFFF000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFF00000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF000000
      00000000000000000000FFFFFF00FFFFFF000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      0000000000000000000000000000FFFFFF0000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFF0000FFFF
      0000FFFF00000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF0000FFFF00FFFFFF0000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF000000
      0000000000000000000000000000000000000000000000000000FFFF0000FFFF
      0000FFFF00000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000FFFF00FFFFFF0000FFFF0000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFF0000FFFF
      0000FFFF00000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000C0C0C000C0C0C000808080008080800080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000C0C0C000C0C0C000808080008080800080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00000000000000000000000000000000000000000000000000C0C0
      C000C0C0C0008080800000000000000000000000000080808000808080008080
      800000000000000000000000000000000000000000000000000000000000C0C0
      C000C0C0C0008080800000000000000000000000000080808000808080008080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF000000000000000000FFFFFF000000
      00000000000000000000FFFFFF0000000000FFFFFF0000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000C0C0C000C0C0
      C00000000000FFFFFF000000FF00FFFFFF000000FF00FFFFFF00000000008080
      8000808080000000000000000000000000000000000000000000C0C0C000C0C0
      C00000000000FFFFFF0000FF0000FFFFFF0000FF0000FFFFFF00000000008080
      8000808080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000C0C0C0000000
      0000FFFFFF000000000000000000000000000000000000000000FFFFFF000000
      0000808080000000000000000000000000000000000000000000C0C0C0000000
      0000FFFFFF000000000000000000000000000000000000000000FFFFFF000000
      0000808080000000000000000000000000000000000000000000000000000000
      FF00000000000000000000000000FFFFFF0000000000C0C0C000000000000000
      0000FFFFFF0000000000FFFFFF0000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0000000000000000000000000000000000C0C0C00080808000FFFF
      FF0000000000000000000000FF0000008000000080000000000000000000FFFF
      FF008080800080808000000000000000000000000000C0C0C00080808000FFFF
      FF00000000000000000000FF000000800000008000000000000000000000FFFF
      FF00808080008080800000000000000000000000000000000000000000000000
      FF000000FF000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF0000000000FFFFFF00FFFF
      FF0080000000800000008000000080000000FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0000000000000000000000000000000000C0C0C000000000000000
      FF00000000000000FF00000080000000FF000000800000008000000000000000
      FF000000000080808000000000000000000000000000C0C0C0000000000000FF
      00000000000000FF00000080000000FF000000800000008000000000000000FF
      000000000000808080000000000000000000000000000000FF000000FF000000
      FF000000FF000000FF0000000000FFFFFF000000000000000000FFFFFF000000
      000000000000000000000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0000000000000000000000000000000000FFFFFF0000000000FFFF
      FF00000000000000FF000000FF000000FF000000FF000000800000000000FFFF
      FF000000000080808000000000000000000000000000FFFFFF0000000000FFFF
      FF000000000000FF000000FF000000FF000000FF00000080000000000000FFFF
      FF0000000000808080000000000000000000000000000000FF000000FF000000
      FF000000FF000000FF000000FF0000000000FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF0080000000800000008000000080000000FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0000000000000000000000000000000000FFFFFF00000000000000
      FF0000000000FFFFFF000000FF000000FF00000080000000FF00000000000000
      FF0000000000C0C0C000000000000000000000000000FFFFFF000000000000FF
      000000000000FFFFFF0000FF000000FF00000080000000FF00000000000000FF
      000000000000C0C0C0000000000000000000000000000000FF000000FF000000
      FF000000FF000000FF000000FF000000FF0000000000C0C0C000FFFFFF000000
      0000FFFFFF00000000000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0000000000000000000000000000000000FFFFFF0080808000FFFF
      FF000000000000000000FFFFFF00FFFFFF000000FF000000000000000000FFFF
      FF0080808000C0C0C000000000000000000000000000FFFFFF0080808000FFFF
      FF000000000000000000FFFFFF00FFFFFF0000FF00000000000000000000FFFF
      FF0080808000C0C0C0000000000000000000000000000000FF000000FF000000
      FF000000FF000000FF000000FF0000000000FFFFFF00FFFFFF00FFFFFF000000
      000000000000000000000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF00800000008000000080000000FFFFFF00000000000000000000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000C0C0C0000000
      0000FFFFFF000000000000000000000000000000000000000000FFFFFF000000
      0000C0C0C0000000000000000000000000000000000000000000C0C0C0000000
      0000FFFFFF000000000000000000000000000000000000000000FFFFFF000000
      0000C0C0C000000000000000000000000000000000000000FF000000FF000000
      FF000000FF000000FF0000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000C0C0C00000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000FFFFFF00C0C0
      C00000000000FFFFFF000000FF00FFFFFF000000FF00FFFFFF0000000000C0C0
      C000C0C0C0000000000000000000000000000000000000000000FFFFFF00C0C0
      C00000000000FFFFFF0000FF0000FFFFFF0000FF0000FFFFFF0000000000C0C0
      C000C0C0C0000000000000000000000000000000000000000000000000000000
      FF000000FF000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000FFFFFF00FFFF
      FF00FFFFFF00000000000000000000000000000000000000000000000000FFFF
      FF00C0C0C0008080800000000000000000000000000080808000C0C0C000C0C0
      C00000000000000000000000000000000000000000000000000000000000FFFF
      FF00C0C0C0008080800000000000000000000000000080808000C0C0C000C0C0
      C000000000000000000000000000000000000000000000000000000000000000
      FF00000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF0000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF00FFFFFF00FFFFFF00C0C0C000C0C0C000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF00FFFFFF00FFFFFF00C0C0C000C0C0C000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008080800080808000808080008080800080808000808080008080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000080808000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C00000000000000000000000000000000000000000000000000000FFFF00C0C0
      C00000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FFFF000000
      0000000000000000000000000000000000008080800080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF000080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000080808000C0C0C0000000000000000000C0C0C000C0C0C000C0C0
      C000000000000000000000000000000000000000000000FFFF000000000000FF
      FF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FF
      FF0000000000FFFFFF00FFFFFF00000000000000000000000000000000000000
      00000000FF000000FF000000FF000000000000FFFF0000FFFF0000FFFF000000
      0000FF000000FF000000FF000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF000080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000080808000C0C0C0000000000000000000C0C0C000C0C0C000C0C0
      C0000000000000000000000000000000000000000000FFFFFF0000FFFF000000
      000000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0
      C00000FFFF0000000000FFFFFF00000000000000000080808000000000000000
      00000000FF000000FF000000FF000000000000FFFF0000FFFF0000FFFF000000
      0000FF000000FF000000FF000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000080808000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF00000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00000000000000000000000000000000000000
      00000000FF000000FF000000FF000000000000FFFF0000FFFF0000FFFF000000
      0000FF000000FF000000FF000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF000080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000008080800080808000000000008080
      80000000FF000000FF000000FF008080800000FFFF0000FFFF0000FFFF008080
      8000FF000000FF000000FF000000808080000000000000000000000000000000
      000000000000000000000000000000FFFF000080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF0000000000FFFFFF000000000000000000000000000000
      00000000000000000000FFFFFF00000000000000000000000000000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000FF000000FF000000FF000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF000080800000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF000000000000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000080808000000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000FF000000FF000000FF000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF0000FFFF0000808000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000FFFFFF0000FF
      FF00FFFFFF0000FFFF0000000000FFFFFF000000000000000000000000000000
      00000000000000000000FFFFFF00000000000000000000000000000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000FF000000FF000000FF000000000000000000000000000000000000000000
      00000000000000000000000000000000000000FFFF0000FFFF00008080000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000080808000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000008080800080808000000000008080
      8000000000000000000000000000808080008080800080808000808080008080
      8000FF000000FF000000FF000000808080000000000000000000000000000000
      000000FFFF000080800000000000000000000000000000FFFF0000FFFF000080
      800000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF000000000000000000FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FF000000FF000000FF000000000000000000000000000000000000000000
      000000FFFF00008080000000000000000000000000000000000000FFFF000080
      800000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FFFF0000FFFF0000808000000000000000000000FFFF0000FFFF000080
      800000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000008080800000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF0000FFFF0000FFFF0000FFFF00008080000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000008080800080808000000000008080
      8000808080008080800080808000808080008080800080808000808080008080
      8000808080008080800080808000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF000000
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000808080000000
      0000808080008080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000FF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000C0C0C0000000
      0000C0C0C000C0C0C00000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000FF000000
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000C0C0C000C0C0
      C000C0C0C000C0C0C0000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF0000FFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000FF000000FF000000FF000000FF000000FF000000
      FF000000FF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF00000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00000000000000FF000000FF000000FF000000FF000000FF000000
      FF000000FF000000FF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00000000000000FF000000FF000000FF000000FF000000FF000000
      FF000000FF000000FF000000FF000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF000000000000000000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      0000FFFFFF00000000000000FF000000FF000000FF000000FF000000FF000000
      FF000000FF000000FF00000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00000000000000FF000000FF000000FF000000FF000000FF000000
      FF000000FF0000000000000000000000000000000000FFFFFF00000000000000
      0000FFFFFF000000000000000000C0C0C00000000000FF000000FF000000FF00
      00000000FF00FF000000FF0000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF0000FFFF000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000FF000000
      FF000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000000000000000
      FF000000FF000000FF0000000000000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF00000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000FF000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      00000000000000000000FFFFFF0000000000FFFFFF00000000000000FF000000
      FF000000FF000000FF000000FF000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000FF000000FF000000
      FF000000FF000000FF000000FF000000FF000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0000000000FFFFFF00FFFFFF000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF0000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000000000C0C0
      C000FFFFFF0000000000FFFFFF00000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0000000000FFFFFF00FFFFFF000000000000000000000000000000
      FF000000FF000000FF000000000000000000FFFFFF000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000FFFF0000FFFF0000FF
      FF00000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000000000C0C0
      C000FFFFFF0000000000FFFFFF00000000000000000000000000808080000000
      FF000000FF000000FF000000000000000000FFFFFF00FFFFFF00000000000000
      00000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000FFFF0000FFFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000FF000000FF000000FF000000
      FF000000FF00000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000112A1251245B27B72B6F2EE42C742FF52C742FF5296D2CE61D622EE01767
      34F7186A36FF176533F20D391D88000000000000000000000000000000000000
      0000112A1251245B27B72B6F2EE42C742FF52C742FF5296D2CE61F586FDC0541
      A3EB0441BAFE033DA4E30121587A000000000000000000000000000000000000
      8000000000000000000000000000000000000000000000000000000000000000
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000307040D2B65
      2FB638873DFB9AC49CFFB2C19CFFBCC09BFFBDC29CFF619C71FF278B52FF63B9
      8CFF94D2B1FF63B98CFF278B52FF0D3A1E8C00000000000000000307040D2B65
      2FB638873DFB9AC49CFFB2C19CFFBCC09BFFBDC29CFF6493B9FF2564C8FF2076
      E6FF0578EAFF0163DDFF044BBAFC0121587A0000000000000000000080000000
      000000000000000000000000FF00000000000000FF0000000000000000000000
      0000000080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000408040D397E3ED16FB6
      85FFADE8C7FFBFD29FFFC4B280FFCAB37CFFCCAE77FF1F6D3AFF61B98AFF5FB9
      86FFFFFFFFFF5FB886FF66BB8EFF176734F7000000000408040D397E3ED16FB6
      85FFADE8C7FFBFD29FFFC4B280FFCAB37CFFCCAE77FF164FB1FF629CF4FF177E
      FFFF0075F8FF0075EEFF0367E1FF033DA5E40000000000000000000000000000
      00000000FF00C0C0C000FFFFFF0000000000FFFFFF00C0C0C0000000FF000000
      0000000000000000000000000000000000000000000000000000800000000000
      0000800000000000000000000000000000000000000080000000000000008000
      0000000000000000000000000000000000000000000035743CB175BF8CFF98D7
      B2FF78BF7FFFBBC17DFFC5A869FFD7B068FFD6AC64FF307A4BFF9BD4B5FFFFFF
      FFFFFFFFFFFFFFFFFFFF94D2B1FF186A36FF0000000035743CB175BF8CFF98D7
      B2FF78BF7FFFBBC17DFFC5A869FFD7B068FFD6AC64FF0541BBFFADCDFEFFFFFF
      FFFFFFFFFFFFFFFFFFFF177EEFFF0441BAFE0000000080808000000000000000
      FF00C0C0C00000000000FFFFFF00FFFFFF00FFFFFF0000000000C0C0C0000000
      FF00000000008080800000000000000000000000000000000000000000008000
      0000000000008000000000000000000000000000000000000000800000000000
      00008000000000000000000000000000000017311A4651AB5BFBB4EAD3FF68BB
      73FF6DBC70FFBDB56CFFC9A25AFFDFAA5AFFDDA757FF47865CFF8FD3B0FF91D6
      B0FFFFFFFFFF64BB8BFF66BB8EFF186835FA17311A4651AB5BFBB4EAD3FF68BB
      73FF6DBC70FFBDB56CFFC9A25AFFDFAA5AFFDDA757FF1B4DAAFF8CB4F6FF4C91
      FFFF1176FFFF2085FFFF3F89EBFF0540A2EA00000000000000000000FF000000
      FF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      FF000000FF000000000000000000000000000000000000000000800000000000
      0000800000000000000000000000000000000000000080000000000000008000
      0000000000000000000000000000000000003E8147B291D7AEFF9FDEB3FF83C5
      6FFFA7D07FFFC4A45BFFD0A656FFE0A955FFDAA550FF8B9258FF60AA80FF94D4
      B3FFB9E6D0FF69BA8EFF2C8E56FF1C632EE23E8147B291D7AEFF9FDEB3FF83C5
      6FFFA7D07FFFC4A45BFFD0A656FFE0A955FFDAA550FF777982FF3A73CEFF8CB4
      F7FFB7D6FEFF71A7F5FF2D6BCAFF155064DA00000000000000000000FF000000
      0000FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00000000000000
      00000000FF000000000000000000000000000000000000000000000000008000
      0000000000008000000000000000000000000000000000000000800000000000
      00008000000000000000000000000000000053AA5EE4AFE9CFFF82D48FFFBEDC
      89FFC2CB81FFCCA155FFDAAE5BFFDCAE5AFFC9994AFFBE8542FF888B57FF5C93
      6CFF4E8D65FF418559FF69A884FF2A6E2DE853AA5EE4AFE9CFFF82D48FFFBEDC
      89FFC2CB81FFCCA155FFDAAE5BFFDCAE5AFFC9994AFFBE8542FF6E6E7FFF2155
      B2FF0541BBFF0E4FB0FF5692BAFF2A6E2DE800000000000000000000FF000000
      FF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF000000
      FF000000FF000000000000000000000000000000000000000000800000000000
      0000800000000000000000000000000000000000000080000000000000008000
      0000000000000000000000000000000000005CBB68F6BDEFDDFF72D17CFF8FD1
      6BFFBBE09DFFC7A65DFFD3AF5DFFC59852FFC5BB6DFFAED179FFB3C36CFFAEA8
      5EFF7AA856FF59A266FFB0E3CEFF2E7932F75CBB68F6BDEFDDFF72D17CFF8FD1
      6BFFBBE09DFFC7A65DFFD3AF5DFFC59852FFC5BB6DFFAED179FFB3C36CFFAEA8
      5EFF7AA856FF59A266FFB0E3CEFF2E7932F70000000080808000000000000000
      FF00C0C0C00000000000FFFFFF0000000000FFFFFF0000000000C0C0C0000000
      FF00000000008080800000000000000000000000000000000000000000008000
      0000000000008000000000000000000000000000000000000000800000000000
      0000800000000000000000000000000000005CBB69F5BDF0DCFF80D882FF76DB
      6CFFBEE599FFCCDFA6FFCAA75CFFC1BC6BFFB7DA8AFFA5D85FFF76D13EFF69D0
      45FF58BB4FFF62AA6BFFB1E4CEFF327C36F65CBB69F5BDF0DCFF80D882FF76DB
      6CFFBEE599FFCCDFA6FFCAA75CFFC1BC6BFFB7DA8AFFA5D85FFF76D13EFF69D0
      45FF58BB4FFF62AA6BFFB1E4CEFF327C36F60000000000000000000000000000
      00000000FF00C0C0C000FFFFFF0000000000FFFFFF00C0C0C0000000FF000000
      0000000000000000000000000000000000000000000000000000800000000000
      0000800000000000000000000000000000000000000080000000000000008000
      00000000000000000000000000000000000055AD61E2B2ECD2FF9AE2A1FF9CEA
      8CFFD4EDB6FFD0EAC7FFCFB86DFFCCB065FFCBC974FF75DB66FF65D94CFF64D7
      4CFF6BD35CFF72BA7DFFA4DBC1FF337A37E655AD61E2B2ECD2FF9AE2A1FF9CEA
      8CFFD4EDB6FFD0EAC7FFCFB86DFFCCB065FFCBC974FF75DB66FF65D94CFF64D7
      4CFF6BD35CFF72BA7DFFA4DBC1FF337A37E60000000000000000000000000000
      0000000000000000FF000000FF000000FF000000FF000000FF00000000000000
      0000000000000000000000000000000000000000000000000000000000008000
      0000000000008000000000000000000000000000000000000000800000000000
      00008000000000000000000000000000000041844AAD97DEB4FFB4EBCCFFB0EF
      A6FFC9EEA8FFD1EAC9FFD5CF8CFFD9CB8AFFCDB365FFBBBB66FF99D670FF81DE
      72FF79DC70FF90D0A2FF87C8A3FF2C6730B941844AAD97DEB4FFB4EBCCFFB0EF
      A6FFC9EEA8FFD1EAC9FFD5CF8CFFD9CB8AFFCDB365FFBBBB66FF99D670FF81DE
      72FF79DC70FF90D0A2FF87C8A3FF2C6730B900000000C0C0C000C0C0C0008080
      800000000000000000000000000000000000000000000000000000000000C0C0
      C000808080008080800000000000000000000000000000000000800000000000
      0000800000000000000000000000000000000000000080000000000000008000
      000000000000000000000000000000000000172E1A3C5EBF6BFABFF3E2FFB4EF
      B3FFB4F0ABFFC0EDB6FFD4E3B6FFD9D89BFFDAD394FFCDB46CFFC7B26CFFB4CB
      83FF93DF99FFAEE7CDFF44984BFC142E164F172E1A3C5EBF6BFABFF3E2FFB4EF
      B3FFB4F0ABFFC0EDB6FFD4E3B6FFD9D89BFFDAD394FFCDB46CFFC7B26CFFB4CB
      83FF93DF99FFAEE7CDFF44984BFC142E164F00000000C0C0C000C0C0C000C0C0
      C000C0C0C00080808000808080000000000080808000C0C0C000C0C0C000C0C0
      C000C0C0C0008080800000000000000000000000000000000000000000008000
      0000000000008000000000000000000000000000000000000000800000000000
      000080000000000000000000000000000000000000003E7E47A586D79FFFBFF2
      DEFFC7F2D6FFD5EFD5FFD0E9CFFFD5DBA5FFDCDEAAFFDBCD8FFFD7C88AFFC9C0
      8DFFBCD5AEFF79C790FF34743BB600000000000000003E7E47A586D79FFFBFF2
      DEFFC7F2D6FFD5EFD5FFD0E9CFFFD5DBA5FFDCDEAAFFDBCD8FFFD7C88AFFC9C0
      8DFFBCD5AEFF79C790FF34743BB600000000000000000000000080808000C0C0
      C000C0C0C0000000000080808000000000008080800000000000C0C0C000C0C0
      C000808080000000000000000000000000000000000000000000800000000000
      0000800000000000000000000000000000000000000080000000000000008000
      00000000000000000000000000000000000000000000020402054A9654C48BD8
      A1FFCDF5E8FFD4EDDAFFCEEDD3FFCFDFAEFFD6DEB4FFD4D4A1FFCED0A0FFC3D0
      A9FF86C990FF438F4BD1050A050F0000000000000000020402054A9654C48BD8
      A1FFCDF5E8FFD4EDDAFFCEEDD3FFCFDFAEFFD6DEB4FFD4D4A1FFCED0A0FFC3D0
      A9FF86C990FF438F4BD1050A050F000000000000000000000000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000808080000000000000000000000000000000000000000000000000008000
      0000000000008000000000000000000000000000000000000000800000000000
      0000800000000000000000000000000000000000000000000000020302043E7D
      46A45DBE6AF8A4E1BAFFB9EACCFFC4E0BDFFC4DAB3FFBCD7AFFFA5D7ABFF5BBA
      66FB408448B60409050C00000000000000000000000000000000020302043E7D
      46A45DBE6AF8A4E1BAFFB9EACCFFC4E0BDFFC4DAB3FFBCD7AFFFA5D7ABFF5BBA
      66FB408448B60409050C00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000800000000000
      0000800000000000000000000000000000000000000080000000000000008000
      0000000000000000000000000000000000000000000000000000000000000000
      0000172F1A3D408249AA54AB5FDF5CBB68F45DBE6AF858B264E943884CB21B36
      1E48000000000000000000000000000000000000000000000000000000000000
      0000172F1A3D408249AA54AB5FDF5CBB68F45DBE6AF858B264E943884CB21B36
      1E48000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000D391D881765
      33F2186A36FF176533F20D391D88000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000D285C78033B
      A1DF0441BAFE033DA4E30121587A000000000000000000000000000000000000
      0000000000000000FF000000FF000000FF000000FF000000FF00000000000000
      00000000000000000000000000000000000000000000787878FF545454FF4D4D
      4DFF464646FF3E3E3EFF363636FF2E2E2EFF272727FF202020FF1A1A1AFF1616
      16FF141414FF141414FF141414FF0000000000000000787878FF545454FF4D4D
      4DFF464646FF3E3E3EFF363636FF2E2E2EFF272727FF1C462BFF278B52FF63B9
      8CFF94D2B1FF63B98CFF278B52FF0D3A1E8C00000000787878FF545454FF4D4D
      4DFF464646FF3E3E3EFF363636FF2E2E2EFF272727FF1C3B71FF2462C5FF2076
      E6FF0578EAFF0163DDFF044BBAFF0121587A0000000000000000000000000000
      FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF000000
      FF000000000000000000000000000000000000000000999999FFD1D1D1FFCBCB
      CBFFC3C3C3FFBBBBBBFFB3B3B3FFAAAAAAFFA2A2A2FF9A9A9AFF939393FF8D8D
      8DFF888888FF868686FF181818FF0000000000000000999999FFD1D1D1FFCBCB
      CBFFC3C3C3FFBBBBBBFFB3B3B3FFAAAAAAFFA2A2A2FF1E6C3BFF61B98AFF5FB9
      86FFFFFFFFFF5FB886FF66BB8EFF176734F700000000999999FFD1D1D1FFCBCB
      CBFFC3C3C3FFBBBBBBFFB3B3B3FFAAAAAAFFA2A2A2FF134DB5FF629CF4FF177E
      FFFF0075F8FF0075EEFF0367E1FF033DA5E400000000000000000000FF000000
      FF000000000000000000808080000000000080808000000000000000FF000000
      FF000000FF0000000000000000000000000000000000A1A1A1FFC7C7C7FFAEAE
      AEFF8E8E8EFF898989FF939393FF797979FF6F6F6FFF747474FF535353FF4A4A
      4AFF606060FF838383FF1C1C1CFF0000000000000000A1A1A1FFC7C7C7FFAEAE
      AEFF8E8E8EFF898989FF939393FF797979FF6F6F6FFF307A4BFF9BD4B5FFFFFF
      FFFFFFFFFFFFFFFFFFFF94D2B1FF186A36FF00000000A1A1A1FFC7C7C7FFAEAE
      AEFF8E8E8EFF898989FF939393FF797979FF6F6F6FFF0441BBFFADCDFEFFFFFF
      FFFFFFFFFFFFFFFFFFFF177EEFFF0441BAFE000000000000FF000000FF000000
      FF000000FF000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF00000000000000000000000000363636549F9F9FF99E9E
      9EFF0D0D0D1D585858FF5A5A5AFF474747FF444444FF4A4A4AFF363636FF0707
      071D4E4E4EFF616161FF1515157E0000000000000000363636549F9F9FF99E9E
      9EFF0D0D0D1D585858FF5A5A5AFF474747FF444444FF43845BFF8FD3B0FF91D6
      B0FFFFFFFFFF64BB8BFF66BB8EFF176734F700000000363636549F9F9FF99E9E
      9EFF0D0D0D1D585858FF5A5A5AFF474747FF444444FF0D44A9FF8CB4F6FF4C91
      FFFF1176FFFF2085FFFF3F89EBFF033AA0DE000000000000FF00000000000000
      FF000000FF000000FF0080808000000000008080800000000000000000000000
      00000000FF000000FF0000000000000000000000000000000000363636547F7F
      7FCF585858B0B2B2B2FFCACACAFFBEBEBEFFBDBDBDFFC8C8C8FF888888FF2C2C
      2CB04D4D4DD32626267E00000000000000000000000000000000363636547F7F
      7FCF585858B0B2B2B2FFCACACAFFBEBEBEFFBDBDBDFF8BAA97FF60AA80FF94D4
      B3FFB9E6D0FF69BA8EFF2C8E56FF0D3A1E8C0000000000000000363636547F7F
      7FCF585858B0B2B2B2FFCACACAFFBEBEBEFFBDBDBDFF7892C5FF3973CFFF8CB4
      F7FFB7D6FEFF71A7F5FF2B69C9FC021C4F6D0000FF000000FF00000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000000000000000FF000000FF000000000000000000000000001212121D6969
      69B8BCBCBCFFB2B2B2FFA0A0A0FF9B9B9BFF999999FF9B9B9BFFA4A4A4FF9191
      91FF373737BC0808081D000000000000000000000000000000001212121D6969
      69B8BCBCBCFFB2B2B2FFA0A0A0FF9B9B9BFF999999FF9B9B9BFF7F9C8AFF5A93
      6FFF4E8D65FF3C7D54F3163A237C0000000000000000000000001212121D6969
      69B8BCBCBCFFB2B2B2FFA0A0A0FF9B9B9BFF999999FF9B9B9BFF6480B3FF1D54
      BAFF0441BBFF043DA4E501205778000000000000FF000000FF00000000000000
      0000000000000000FF000000FF00000000000000000000000000000000000000
      0000000000000000FF000000FF000000000000000000000000004C4C4C70C2C2
      C2FFDADADAFFAEAEAEFFD1D1D1FFF0F0F0FFEFEFEFFFCECECEFFA5A5A5FFC7C7
      C7FF7B7B7BFF1F1F1F70000000000000000000000000000000004C4C4C70C2C2
      C2FFDADADAFFAEAEAEFFD1D1D1FFF0F0F0FFEFEFEFFFCECECEFFA5A5A5FFC7C7
      C7FF7B7B7BFF1F1F1F70000000000000000000000000000000004C4C4C70C2C2
      C2FFDADADAFFAEAEAEFFD1D1D1FFF0F0F0FFEFEFEFFFCECECEFFA5A5A5FFC7C7
      C7FF7B7B7BFF1F1F1F7000000000000000000000FF000000FF00000000000000
      0000000000000000000080808000000000008080800000000000000000000000
      0000000000000000FF000000FF00000000000000000000000000888888B7DBDB
      DBFFDADADAFFEAEAEAFFDAD1CCFFD5AB90FFE3AD89FFE6D9D3FFE4E4E4FFCBCB
      CBFFACACACFF373737B700000000000000000000000000000000888888B7DBDB
      DBFFDADADAFFEAEAEAFFDAD1CCFFD5AB90FFE3AD89FFE6D9D3FFE4E4E4FFCBCB
      CBFFACACACFF373737B700000000000000000000000000000000888888B7DBDB
      DBFFDADADAFFEAEAEAFFDAD1CCFFD5AB90FFE3AD89FFE6D9D3FFE4E4E4FFCBCB
      CBFFACACACFF373737B700000000000000000000FF000000FF00000000000000
      000000000000000000000000800000000000000080000000FF00000000000000
      0000000000000000FF000000FF00000000000000000000000000B8B8B8EAE8E8
      E8FFDDDDDDFFF9F9F9FFA39891FFE4B89BFFEBB798FFE7B08FFFF7F7F7FFCBCB
      CBFFCCCCCCFF4F4F4FEA00000000000000000000000000000000B8B8B8EAE8E8
      E8FFDDDDDDFFF9F9F9FFA39891FFE4B89BFFEBB798FFE7B08FFFF7F7F7FFCBCB
      CBFFCCCCCCFF4F4F4FEA00000000000000000000000000000000B8B8B8EAE8E8
      E8FFDDDDDDFFF9F9F9FFA39891FFE4B89BFFEBB798FFE7B08FFFF7F7F7FFCBCB
      CBFFCCCCCCFF4F4F4FEA00000000000000000000FF000000FF00000000000000
      000000000000000000000000000000000000000000000000FF000000FF000000
      0000000000000000FF000000FF00000000000000000000000000C1C1C1EAECEC
      ECFFE3E3E3FFFAFAF9FFA39A93FFDAB8A1FFE5BAA0FFD8AF95FFF7F7F7FFCECE
      CEFFDADADAFF595959EA00000000000000000000000000000000C1C1C1EAECEC
      ECFFE3E3E3FFFAFAF9FFA39A93FFDAB8A1FFE5BAA0FFD8AF95FFF7F7F7FFCECE
      CEFFDADADAFF595959EA00000000000000000000000000000000C1C1C1EAECEC
      ECFFE3E3E3FFFAFAF9FFA39A93FFDAB8A1FFE5BAA0FFD8AF95FFF7F7F7FFCECE
      CEFFDADADAFF595959EA0000000000000000000000000000FF000000FF000000
      000000000000000000000000000000000000000000000000FF000000FF000000
      FF00000000000000FF00000000000000000000000000000000009C9C9CB7EDED
      EDFFEAEAEAFFF3F3F3FFDFD4CCFFA49A93FFA39992FFDCD1CAFFE9E9E9FFD3D3
      D3FFCFCFCFFF4E4E4EB7000000000000000000000000000000009C9C9CB7EDED
      EDFFEAEAEAFFF3F3F3FFDFD4CCFFA49A93FFA39992FFDCD1CAFFE9E9E9FFD3D3
      D3FFCFCFCFFF4E4E4EB7000000000000000000000000000000009C9C9CB7EDED
      EDFFEAEAEAFFF3F3F3FFDFD4CCFFA49A93FFA39992FFDCD1CAFFE9E9E9FFD3D3
      D3FFCFCFCFFF4E4E4EB70000000000000000000000000000FF000000FF000000
      FF000000000000000000000000000000000000000000000000000000FF000000
      FF000000FF000000FF000000000000000000000000000000000063636370EAEA
      EAFFF2F2F2FFEFEFEFFFF5F5F5FFFBFAFAFFFAFAF9FFEDEDEDFFDADADAFFDFDF
      DFFFB8B8B8FF373737700000000000000000000000000000000063636370EAEA
      EAFFF2F2F2FFEFEFEFFFF5F5F5FFFBFAFAFFFAFAF9FFEDEDEDFFDADADAFFDFDF
      DFFFB8B8B8FF373737700000000000000000000000000000000063636370EAEA
      EAFFF2F2F2FFEFEFEFFFF5F5F5FFFBFAFAFFFAFAF9FFEDEDEDFFDADADAFFDFDF
      DFFFB8B8B8FF37373770000000000000000000000000000000000000FF000000
      FF000000FF000000000080808000000000008080800000000000000000000000
      FF000000FF0000000000000000000000000000000000000000001A1A1A1D9F9F
      9FB0F1F1F1FFF5F5F5FFEEEEEEFFBDBDBDFFB9B9B9FFE0E0E0FFE3E3E3FFDADA
      DAFF6C6C6CB01111111D000000000000000000000000000000001A1A1A1D9F9F
      9FB0F1F1F1FFF5F5F5FFEEEEEEFFBDBDBDFFB9B9B9FFE0E0E0FFE3E3E3FFDADA
      DAFF6C6C6CB01111111D000000000000000000000000000000001A1A1A1D9F9F
      9FB0F1F1F1FFF5F5F5FFEEEEEEFFBDBDBDFFB9B9B9FFE0E0E0FFE3E3E3FFDADA
      DAFF6C6C6CB01111111D00000000000000000000000000000000000000000000
      FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF000000
      FF00000000000000000000000000000000000000000000000000000000003434
      3439A1A1A1B0EEEEEEFFEFEFEFFFF1F1F1FFEDEDEDFFE6E6E6FFDADADAFF8181
      81B0272727390000000000000000000000000000000000000000000000003434
      3439A1A1A1B0EEEEEEFFEFEFEFFFF1F1F1FFEDEDEDFFE6E6E6FFDADADAFF8181
      81B0272727390000000000000000000000000000000000000000000000003434
      3439A1A1A1B0EEEEEEFFEFEFEFFFF1F1F1FFEDEDEDFFE6E6E6FFDADADAFF8181
      81B0272727390000000000000000000000000000000000000000000000000000
      0000000000000000FF000000FF000000FF000000FF000000FF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00001A1A1A1D67676770A7A7A7B7D2D2D2EACECECEEA9E9E9EB75C5C5C701616
      161D000000000000000000000000000000000000000000000000000000000000
      00001A1A1A1D67676770A7A7A7B7D2D2D2EACECECEEA9E9E9EB75C5C5C701616
      161D000000000000000000000000000000000000000000000000000000000000
      00001A1A1A1D67676770A7A7A7B7D2D2D2EACECECEEA9E9E9EB75C5C5C701616
      161D00000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000900000000100010000000000800400000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000FFFFFFFFFFFDFFFDFFFF8017FFF8FFF8
      C003FFF7FFF1FFF1CFFBFE63FFE3FFE3C00BFC03FFC7FFC7C00BF803E08FE08F
      C00BF003C01FC01FC00BF003803F803FC00BE003001F001FC00BC003001F001F
      C00B8003001F001FC00B0003001F001FC0030003001F001FC00301E3803F803F
      FFFF83E3C07FC07FFFFFC7E3E0FFE0FFFFFFFFFFFFFFFFFFE3C7FEFDE7FFC001
      C183EF77EBFF8031C1839DBBF5FF8031C183E7D9EAFF8031C183ECF3F57F8001
      C183EFE7EABF8001C183DCFFF55F8001C1833FBFEABF8FF1C183EF6FF57F8FF1
      C183FDFFEAFF8FF1C183BFBBF5FF8FF1C183DBFFEBFF8FF1C387A7EFF7FF8FF5
      C78FEF7BEFFF8001FFFFFFEFFFFFFFFFFFFFFFFF8000FFFF07C1FFFF8000FFFF
      07C1C007C000001F07C1C007E000000F06C1C007F0000007DD77C007F8000003
      DAB7C007FC000001C547C007FE000000FABFC007FF00001FFD7FC007FF80001F
      FEFFC0078380001FF83FC00783E08FF1F83FC00F83E0FFF9F83FC01F83E0FF75
      F83FC03F8384FF8FFFFFFFFFFFFEFFFFFFFFFFFFFFFFFFFFFC00FFFFF83FF83F
      FC00FFFFE00FE00FFC000007C007C007FC007FE780038003EC00402780038003
      E400402700010001E00040270001000100004027000100010001402700010001
      0003402700010001000740E780038003000F40A780038003E3FF40C7C007C007
      E7FF7F87E00FE00FEFFF0007F83FF83FFFFFFFFFF111FFFF8003801FFFFFFE7F
      000100000000FC3F00010000D111FC3F000100009111FE7F00010000D111FC3F
      000100000000FC3F00010000D111FC3F0001000091F1FC1F00018000D1F1F20F
      000180000000E1070001FC00DFF1E1870001FC019FF1E0070001FC03DFFFF00F
      0001FC070000F81F8003FFFFFFFFFFFFFFFFBFFFFFDFFF00FFFFBFFFFFCFFF00
      FFFFB049FFC7FF000007807F0003FF000007B07F000100007FF7B9FF00000000
      8017BFFF000100008017B049000300008017807F000700238017B07F000F0001
      8017B9FF001F00008017BFFF007F0023FFE7048F00FF0063420707FF01FF00C3
      3C0707FF03FF010700079FFFFFFF03FF000000008823FFFF00000000C007EBD7
      00000000C007D5AB000000008003EBD7000000008003D5AB000000008003EBD7
      000000008003D5AB000000008003EBD700000000C007D5AB000000000001EBD7
      000000000001D5AB000000000001EBD7000000008003D5AB00000000C6C7EBD7
      00000000FC7FD7AF00000000FFFFFFFFFFFF000000000000F83F000000000000
      E00F000000000000CC470000000000008463000000000000A073000000000000
      31F900000000000038F90000000000003C790000000000003C39000000000000
      3C190000000000009C0B0000000000008C43000000000000C467000000000000
      E00F000000000000F83F00000000000000000000000000000000000000000000
      000000000000}
  end
  object RecordingPopupMenu: TPopupMenu
    Left = 328
    Top = 368
    object SelectFile1: TMenuItem
      Caption = 'Select File'
      OnClick = SelectFile1Click
    end
    object RecordingMethod: TMenuItem
      Caption = 'Recording Method'
    end
    object VideoCodec: TMenuItem
      Caption = 'Video Codec'
    end
  end
  object ActionList: TActionList
    Left = 528
    Top = 176
    object SavePictureAction: TAction
      Caption = 'Save Picture'
      ShortCut = 123
      OnExecute = SavePictureActionExecute
    end
  end
end
