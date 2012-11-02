inherited VideoGrabberControlFrame: TVideoGrabberControlFrame
  Width = 453
  Height = 269
  DoubleBuffered = True
  ParentDoubleBuffered = False
  ExplicitWidth = 453
  ExplicitHeight = 269
  object VCapturePageControl: TPageControl
    Left = 0
    Top = 0
    Width = 453
    Height = 269
    ActivePage = DeviceTabSheet
    Align = alClient
    DoubleBuffered = True
    ParentDoubleBuffered = False
    Style = tsFlatButtons
    TabOrder = 0
    OnChange = VCapturePageControlChange
    object DeviceTabSheet: TTabSheet
      Caption = 'Device'
      object VDCapturePanel: TPanel
        Left = 0
        Top = 0
        Width = 445
        Height = 238
        Align = alClient
        TabOrder = 0
        DesignSize = (
          445
          238)
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
      end
    end
    object IPCameraTabSheet: TTabSheet
      Caption = 'IP Camera'
      ImageIndex = 3
      object Panel1: TPanel
        Left = 0
        Top = 0
        Width = 445
        Height = 238
        Align = alClient
        TabOrder = 0
        DesignSize = (
          445
          238)
        object Label4: TLabel
          Left = 8
          Top = 3
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'IP Camera URL:'
          ExplicitWidth = 369
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
      end
    end
    object VideoFileTabSheet: TTabSheet
      Caption = 'Video File'
      ImageIndex = 1
      object VFCapturePanel: TPanel
        Left = 0
        Top = 0
        Width = 445
        Height = 238
        Align = alClient
        TabOrder = 0
        DesignSize = (
          445
          238)
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
      DesignSize = (
        445
        238)
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
