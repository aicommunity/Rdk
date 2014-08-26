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
    ExplicitWidth = 558
    ExplicitHeight = 218
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
      ExplicitWidth = 177
    end
    object Label5: TLabel
      Left = 8
      Top = 47
      Width = 576
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'User Name:'
      ExplicitWidth = 390
    end
    object Label6: TLabel
      Left = 8
      Top = 89
      Width = 576
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'User Password:'
      ExplicitWidth = 390
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
      ExplicitWidth = 152
    end
    object IPCameraUrlEdit: TEdit
      Left = 8
      Top = 20
      Width = 576
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      Text = 'http://127.0.0.1/video/mjpg.cgi'
      ExplicitWidth = 542
    end
    object IPCameraUserNameEdit: TEdit
      Left = 8
      Top = 62
      Width = 576
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 1
      Text = 'Admin'
      ExplicitWidth = 542
    end
    object IPCameraUserPasswordEdit: TEdit
      Left = 8
      Top = 108
      Width = 576
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
      ExplicitWidth = 542
    end
    object IpMoveUpButton: TButton
      Left = 96
      Top = 188
      Width = 75
      Height = 25
      Caption = 'Up'
      TabOrder = 3
      Visible = False
    end
    object IpMoveLeftButton: TButton
      Left = 24
      Top = 219
      Width = 75
      Height = 25
      Caption = 'Left'
      TabOrder = 4
      Visible = False
    end
    object IpMoveRightButton: TButton
      Left = 168
      Top = 219
      Width = 75
      Height = 25
      Caption = 'Right'
      TabOrder = 5
      Visible = False
    end
    object IpMoveDownButton: TButton
      Left = 96
      Top = 250
      Width = 75
      Height = 25
      Caption = 'Down'
      TabOrder = 6
      Visible = False
    end
    object ZoomInButton: TButton
      Left = 249
      Top = 188
      Width = 75
      Height = 25
      Caption = 'Zoom Im'
      TabOrder = 7
      Visible = False
    end
    object ZoomOutButton: TButton
      Left = 249
      Top = 250
      Width = 75
      Height = 25
      Caption = 'Zoom Out'
      TabOrder = 8
      Visible = False
    end
    object ResetButton: TButton
      Left = 344
      Top = 219
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
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 10
      Text = 'http://127.0.0.1/video/mjpg.cgi'
      Visible = False
      ExplicitWidth = 334
    end
    object FpsLabeledEdit: TLabeledEdit
      Left = 8
      Top = 151
      Width = 576
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 17
      EditLabel.Height = 13
      EditLabel.Caption = 'Fps'
      TabOrder = 11
      ExplicitWidth = 550
    end
  end
end
