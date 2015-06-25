object VideoCaptureOptionsIPCameraForm: TVideoCaptureOptionsIPCameraForm
  Left = 0
  Top = 0
  ClientHeight = 310
  ClientWidth = 592
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 592
    Height = 310
    Align = alClient
    TabOrder = 0
    DesignSize = (
      592
      310)
    object Label4: TLabel
      Left = 8
      Top = 3
      Width = 324
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'IP Camera URL:'
    end
    object Label5: TLabel
      Left = 8
      Top = 47
      Width = 576
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'User Name:'
    end
    object Label6: TLabel
      Left = 8
      Top = 89
      Width = 576
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'User Password:'
    end
    object Label8: TLabel
      Left = 216
      Top = 1
      Width = 299
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Camera control URL:'
      Visible = False
    end
    object IPCameraUrlEdit: TEdit
      Left = 8
      Top = 20
      Width = 576
      Height = 25
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      Text = 'http://127.0.0.1/video/mjpg.cgi'
    end
    object IPCameraUserNameEdit: TEdit
      Left = 8
      Top = 62
      Width = 576
      Height = 25
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 1
      Text = 'Admin'
    end
    object IPCameraUserPasswordEdit: TEdit
      Left = 8
      Top = 108
      Width = 576
      Height = 25
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
    end
    object IpMoveUpButton: TButton
      Left = 329
      Top = 193
      Width = 75
      Height = 26
      Caption = 'Up'
      TabOrder = 3
      Visible = False
    end
    object IpMoveLeftButton: TButton
      Left = 252
      Top = 226
      Width = 75
      Height = 25
      Caption = 'Left'
      TabOrder = 4
      Visible = False
    end
    object IpMoveRightButton: TButton
      Left = 333
      Top = 225
      Width = 75
      Height = 25
      Caption = 'Right'
      TabOrder = 5
      Visible = False
    end
    object IpMoveDownButton: TButton
      Left = 329
      Top = 262
      Width = 75
      Height = 26
      Caption = 'Down'
      TabOrder = 6
      Visible = False
    end
    object ZoomInButton: TButton
      Left = 414
      Top = 194
      Width = 75
      Height = 25
      Caption = 'Zoom Im'
      TabOrder = 7
      Visible = False
    end
    object ZoomOutButton: TButton
      Left = 414
      Top = 256
      Width = 75
      Height = 25
      Caption = 'Zoom Out'
      TabOrder = 8
      Visible = False
    end
    object ResetButton: TButton
      Left = 509
      Top = 225
      Width = 75
      Height = 25
      Caption = 'Reset'
      TabOrder = 9
      Visible = False
    end
    object IPCameraControlPostfixEdit: TEdit
      Left = 216
      Top = 20
      Width = 368
      Height = 25
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 10
      Text = 'http://127.0.0.1/video/mjpg.cgi'
      Visible = False
    end
    object FpsLabeledEdit: TLabeledEdit
      Left = 8
      Top = 151
      Width = 576
      Height = 25
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 17
      EditLabel.Height = 13
      EditLabel.Caption = 'Fps'
      TabOrder = 11
    end
    object DesiredResFlagCheckBox: TCheckBox
      Left = 8
      Top = 177
      Width = 113
      Height = 13
      Margins.Left = 2
      Margins.Top = 2
      Margins.Right = 2
      Margins.Bottom = 2
      Caption = 'Use user resolution'
      TabOrder = 12
    end
    object DesiredWidthLabeledEdit: TLabeledEdit
      Left = 125
      Top = 193
      Width = 36
      Height = 25
      Margins.Left = 2
      Margins.Top = 2
      Margins.Right = 2
      Margins.Bottom = 2
      EditLabel.Width = 28
      EditLabel.Height = 13
      EditLabel.Margins.Left = 2
      EditLabel.Margins.Top = 2
      EditLabel.Margins.Right = 2
      EditLabel.Margins.Bottom = 2
      EditLabel.Caption = 'Width'
      TabOrder = 13
      Text = '640'
    end
    object DesiredHeightLabeledEdit: TLabeledEdit
      Left = 166
      Top = 193
      Width = 37
      Height = 25
      Margins.Left = 2
      Margins.Top = 2
      Margins.Right = 2
      Margins.Bottom = 2
      EditLabel.Width = 31
      EditLabel.Height = 13
      EditLabel.Margins.Left = 2
      EditLabel.Margins.Top = 2
      EditLabel.Margins.Right = 2
      EditLabel.Margins.Bottom = 2
      EditLabel.Caption = 'Height'
      TabOrder = 14
      Text = '480'
    end
    object ConnectionTimeoutLabeledEdit: TLabeledEdit
      Left = 8
      Top = 231
      Width = 113
      Height = 25
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 113
      EditLabel.Height = 13
      EditLabel.Caption = 'Connection timeout, ms'
      TabOrder = 15
    end
    object CaptureTimeoutLabeledEdit: TLabeledEdit
      Left = 133
      Top = 231
      Width = 113
      Height = 25
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 98
      EditLabel.Height = 13
      EditLabel.Caption = 'Capture timeout, ms'
      TabOrder = 16
    end
  end
end
