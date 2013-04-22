inherited VideoGrabberControlFrame: TVideoGrabberControlFrame
  Width = 453
  Height = 297
  DoubleBuffered = True
  ParentDoubleBuffered = False
  ExplicitWidth = 453
  ExplicitHeight = 297
  object VCapturePageControl: TPageControl
    Left = 0
    Top = 0
    Width = 453
    Height = 297
    ActivePage = IPCameraTabSheet
    Align = alClient
    DoubleBuffered = True
    ParentDoubleBuffered = False
    Style = tsFlatButtons
    TabOrder = 0
    OnChange = VCapturePageControlChange
    object DeviceTabSheet: TTabSheet
      Caption = 'Device'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object VDCapturePanel: TPanel
        Left = 0
        Top = 0
        Width = 445
        Height = 266
        Align = alClient
        TabOrder = 0
        DesignSize = (
          445
          266)
        object ModeLabel: TLabel
          Left = 8
          Top = 89
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Video Size:'
          ExplicitWidth = 390
        end
        object InputLabel: TLabel
          Left = 8
          Top = 47
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Video Input:'
          ExplicitWidth = 390
        end
        object DeviceLabel: TLabel
          Left = 8
          Top = 3
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Video device:'
          ExplicitWidth = 369
        end
        object Label1: TLabel
          Left = 8
          Top = 137
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Subtype:'
        end
        object Label2: TLabel
          Left = 8
          Top = 185
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Analog video standard:'
        end
        object VideoSizeComboBox: TComboBox
          Left = 8
          Top = 104
          Width = 429
          Height = 21
          Style = csDropDownList
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 2
          OnSelect = VideoSizeComboBoxSelect
        end
        object InputComboBox: TComboBox
          Left = 8
          Top = 62
          Width = 429
          Height = 21
          Style = csDropDownList
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 1
          OnSelect = InputComboBoxSelect
        end
        object DeviceComboBox: TComboBox
          Left = 8
          Top = 19
          Width = 429
          Height = 21
          Style = csDropDownList
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 0
          OnSelect = DeviceComboBoxSelect
        end
        object VideoSubTypeComboBox: TComboBox
          Left = 8
          Top = 152
          Width = 429
          Height = 21
          Style = csDropDownList
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 3
          OnSelect = VideoSubTypeComboBoxSelect
        end
        object AnalogVideoStandardComboBox: TComboBox
          Left = 8
          Top = 200
          Width = 429
          Height = 21
          Style = csDropDownList
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 4
          OnSelect = AnalogVideoStandardComboBoxSelect
        end
        object StreamButton: TButton
          Left = 8
          Top = 231
          Width = 75
          Height = 25
          Caption = 'Stream'
          TabOrder = 5
          OnClick = StreamButtonClick
        end
      end
    end
    object IPCameraTabSheet: TTabSheet
      Caption = 'IP Camera'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 445
        Height = 266
        Align = alClient
        TabOrder = 0
        DesignSize = (
          445
          266)
        object Label4: TLabel
          Left = 8
          Top = 3
          Width = 177
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'IP Camera URL:'
        end
        object Label5: TLabel
          Left = 8
          Top = 47
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'User Name:'
          ExplicitWidth = 390
        end
        object Label6: TLabel
          Left = 8
          Top = 89
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'User Password:'
          ExplicitWidth = 390
        end
        object Label8: TLabel
          Left = 216
          Top = 1
          Width = 152
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Camera control URL:'
          Visible = False
        end
        object IPCameraUrlEdit: TEdit
          Left = 8
          Top = 20
          Width = 429
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 0
          Text = 'http://127.0.0.1/video/mjpg.cgi'
        end
        object IPCameraUserNameEdit: TEdit
          Left = 8
          Top = 62
          Width = 429
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 1
          Text = 'Admin'
        end
        object IPCameraUserPasswordEdit: TEdit
          Left = 8
          Top = 108
          Width = 429
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 2
        end
        object IpMoveUpButton: TButton
          Left = 96
          Top = 144
          Width = 75
          Height = 25
          Caption = 'Up'
          TabOrder = 3
          Visible = False
          OnClick = IpMoveUpButtonClick
        end
        object IpMoveLeftButton: TButton
          Left = 24
          Top = 175
          Width = 75
          Height = 25
          Caption = 'Left'
          TabOrder = 4
          Visible = False
          OnClick = IpMoveLeftButtonClick
        end
        object IpMoveRightButton: TButton
          Left = 168
          Top = 175
          Width = 75
          Height = 25
          Caption = 'Right'
          TabOrder = 5
          Visible = False
          OnClick = IpMoveRightButtonClick
        end
        object IpMoveDownButton: TButton
          Left = 96
          Top = 206
          Width = 75
          Height = 25
          Caption = 'Down'
          TabOrder = 6
          Visible = False
          OnClick = IpMoveDownButtonClick
        end
        object ZoomInButton: TButton
          Left = 249
          Top = 144
          Width = 75
          Height = 25
          Caption = 'Zoom Im'
          TabOrder = 7
          Visible = False
          OnClick = ZoomInButtonClick
        end
        object ZoomOutButton: TButton
          Left = 249
          Top = 206
          Width = 75
          Height = 25
          Caption = 'Zoom Out'
          TabOrder = 8
          Visible = False
          OnClick = ZoomOutButtonClick
        end
        object ResetButton: TButton
          Left = 344
          Top = 175
          Width = 75
          Height = 25
          Caption = 'Reset'
          TabOrder = 9
          Visible = False
          OnClick = ResetButtonClick
        end
        object IPCameraControlPostfixEdit: TEdit
          Left = 216
          Top = 20
          Width = 221
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 10
          Text = 'http://127.0.0.1/video/mjpg.cgi'
          Visible = False
        end
      end
    end
    object VideoFileTabSheet: TTabSheet
      Caption = 'Video File'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object VFCapturePanel: TPanel
        Left = 0
        Top = 0
        Width = 445
        Height = 266
        Align = alClient
        TabOrder = 0
        DesignSize = (
          445
          266)
        object VideoFileNameLabel: TLabel
          Left = 8
          Top = 3
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Video file name:'
          ExplicitWidth = 369
        end
        object VFNameEdit: TEdit
          Left = 8
          Top = 22
          Width = 344
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 0
        end
        object VFBrowseButton: TButton
          Left = 358
          Top = 20
          Width = 79
          Height = 25
          Anchors = [akTop, akRight]
          Caption = 'Browse'
          TabOrder = 1
          OnClick = VFBrowseButtonClick
        end
        object VideoTruncPathCheckBox: TCheckBox
          Left = 8
          Top = 49
          Width = 153
          Height = 17
          Caption = 'Truncate full path'
          TabOrder = 2
        end
      end
    end
    object PictureFileTabSheet: TTabSheet
      Caption = 'Picture File'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        445
        266)
      object Label3: TLabel
        Left = 8
        Top = 3
        Width = 429
        Height = 13
        Anchors = [akLeft, akTop, akRight]
        AutoSize = False
        Caption = 'Image file name:'
        ExplicitWidth = 369
      end
      object ImageFileNameEdit: TEdit
        Left = 8
        Top = 22
        Width = 344
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
      end
      object OpenImageFileButton: TButton
        Left = 358
        Top = 20
        Width = 79
        Height = 25
        Anchors = [akTop, akRight]
        Caption = 'Browse'
        TabOrder = 1
        OnClick = OpenImageFileButtonClick
      end
      object PictureTruncPathCheckBox: TCheckBox
        Left = 8
        Top = 49
        Width = 153
        Height = 17
        Caption = 'Truncate full path'
        TabOrder = 2
      end
    end
    object ImageSequenceTabSheet: TTabSheet
      Caption = 'Image Sequence'
      ImageIndex = 4
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        445
        266)
      object Label7: TLabel
        Left = 8
        Top = 3
        Width = 429
        Height = 13
        Anchors = [akLeft, akTop, akRight]
        AutoSize = False
        Caption = 'Image file name:'
        ExplicitWidth = 369
      end
      object ImageSequencePathEdit: TEdit
        Left = 8
        Top = 22
        Width = 344
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        TabOrder = 0
      end
      object ImageSequencePathCheckBox: TCheckBox
        Left = 8
        Top = 49
        Width = 153
        Height = 17
        Caption = 'Truncate full path'
        TabOrder = 1
      end
      object ImageSequencePathBrowseButton: TButton
        Left = 358
        Top = 20
        Width = 79
        Height = 25
        Anchors = [akTop, akRight]
        Caption = 'Browse'
        TabOrder = 2
        OnClick = ImageSequencePathBrowseButtonClick
      end
    end
  end
  object VideoOpenDialog: TOpenDialog
    DefaultExt = 'avi'
    Filter = #1042#1080#1076#1077#1086'|*.avi|'#1042#1089#1077' '#1092#1072#1081#1083#1099'|*.*'
    FilterIndex = 0
    Left = 200
    Top = 112
  end
  object PicturesOpenDialog: TOpenDialog
    DefaultExt = 'bmp'
    Filter = #1060#1086#1090#1086'|*.bmp|'#1042#1089#1077' '#1092#1072#1081#1083#1099'|*.*'
    FilterIndex = 0
    Left = 288
    Top = 112
  end
end
