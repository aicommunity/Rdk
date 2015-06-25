object VideoCaptureOptionsIPCameraForm: TVideoCaptureOptionsIPCameraForm
  Left = 0
  Top = 0
  ClientHeight = 388
  ClientWidth = 740
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 17
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 740
    Height = 388
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    TabOrder = 0
    DesignSize = (
      740
      388)
    object Label4: TLabel
      Left = 10
      Top = 4
      Width = 405
      Height = 16
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'IP Camera URL:'
    end
    object Label5: TLabel
      Left = 10
      Top = 59
      Width = 720
      Height = 16
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'User Name:'
    end
    object Label6: TLabel
      Left = 10
      Top = 111
      Width = 720
      Height = 17
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'User Password:'
    end
    object Label8: TLabel
      Left = 270
      Top = 1
      Width = 374
      Height = 17
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Camera control URL:'
      Visible = False
    end
    object IPCameraUrlEdit: TEdit
      Left = 10
      Top = 25
      Width = 720
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      Text = 'http://127.0.0.1/video/mjpg.cgi'
    end
    object IPCameraUserNameEdit: TEdit
      Left = 10
      Top = 78
      Width = 720
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 1
      Text = 'Admin'
    end
    object IPCameraUserPasswordEdit: TEdit
      Left = 10
      Top = 135
      Width = 720
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
    end
    object IpMoveUpButton: TButton
      Left = 411
      Top = 241
      Width = 94
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Up'
      TabOrder = 3
      Visible = False
    end
    object IpMoveLeftButton: TButton
      Left = 315
      Top = 283
      Width = 94
      Height = 31
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Left'
      TabOrder = 4
      Visible = False
    end
    object IpMoveRightButton: TButton
      Left = 416
      Top = 281
      Width = 94
      Height = 32
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Right'
      TabOrder = 5
      Visible = False
    end
    object IpMoveDownButton: TButton
      Left = 411
      Top = 328
      Width = 94
      Height = 32
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Down'
      TabOrder = 6
      Visible = False
    end
    object ZoomInButton: TButton
      Left = 518
      Top = 243
      Width = 93
      Height = 31
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Zoom Im'
      TabOrder = 7
      Visible = False
    end
    object ZoomOutButton: TButton
      Left = 518
      Top = 320
      Width = 93
      Height = 31
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Zoom Out'
      TabOrder = 8
      Visible = False
    end
    object ResetButton: TButton
      Left = 636
      Top = 281
      Width = 94
      Height = 32
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Reset'
      TabOrder = 9
      Visible = False
    end
    object IPCameraControlPostfixEdit: TEdit
      Left = 270
      Top = 25
      Width = 460
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 10
      Text = 'http://127.0.0.1/video/mjpg.cgi'
      Visible = False
    end
    object FpsLabeledEdit: TLabeledEdit
      Left = 10
      Top = 189
      Width = 720
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 21
      EditLabel.Height = 17
      EditLabel.Margins.Left = 4
      EditLabel.Margins.Top = 4
      EditLabel.Margins.Right = 4
      EditLabel.Margins.Bottom = 4
      EditLabel.Caption = 'Fps'
      TabOrder = 11
    end
    object DesiredResFlagCheckBox: TCheckBox
      Left = 10
      Top = 221
      Width = 141
      Height = 17
      Caption = 'Use user resolution'
      TabOrder = 12
    end
    object DesiredWidthLabeledEdit: TLabeledEdit
      Left = 156
      Top = 241
      Width = 45
      Height = 25
      EditLabel.Width = 37
      EditLabel.Height = 17
      EditLabel.Caption = 'Width'
      TabOrder = 13
      Text = '640'
    end
    object DesiredHeightLabeledEdit: TLabeledEdit
      Left = 208
      Top = 241
      Width = 46
      Height = 25
      EditLabel.Width = 39
      EditLabel.Height = 17
      EditLabel.Caption = 'Height'
      TabOrder = 14
      Text = '480'
    end
    object ConnectionTimeoutLabeledEdit: TLabeledEdit
      Left = 10
      Top = 289
      Width = 141
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 147
      EditLabel.Height = 17
      EditLabel.Margins.Left = 4
      EditLabel.Margins.Top = 4
      EditLabel.Margins.Right = 4
      EditLabel.Margins.Bottom = 4
      EditLabel.Caption = 'Connection timeout, ms'
      TabOrder = 15
    end
    object CaptureTimeoutLabeledEdit: TLabeledEdit
      Left = 166
      Top = 289
      Width = 142
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 126
      EditLabel.Height = 17
      EditLabel.Margins.Left = 4
      EditLabel.Margins.Top = 4
      EditLabel.Margins.Right = 4
      EditLabel.Margins.Bottom = 4
      EditLabel.Caption = 'Capture timeout, ms'
      TabOrder = 16
    end
  end
end
