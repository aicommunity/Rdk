object VideoCaptureOptionsIPCameraForm: TVideoCaptureOptionsIPCameraForm
  Left = 0
  Top = 0
  ClientHeight = 405
  ClientWidth = 774
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
    Width = 774
    Height = 405
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    TabOrder = 0
    DesignSize = (
      774
      405)
    object Label4: TLabel
      Left = 10
      Top = 4
      Width = 424
      Height = 17
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
      Top = 61
      Width = 754
      Height = 17
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
      Top = 116
      Width = 754
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
      Left = 282
      Top = 1
      Width = 391
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
      Top = 26
      Width = 754
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
      Top = 81
      Width = 754
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
      Top = 141
      Width = 754
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
    end
    object IpMoveUpButton: TButton
      Left = 342
      Top = 254
      Width = 98
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
      Left = 247
      Top = 294
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Left'
      TabOrder = 4
      Visible = False
    end
    object IpMoveRightButton: TButton
      Left = 436
      Top = 294
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Right'
      TabOrder = 5
      Visible = False
    end
    object IpMoveDownButton: TButton
      Left = 342
      Top = 335
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Down'
      TabOrder = 6
      Visible = False
    end
    object ZoomInButton: TButton
      Left = 542
      Top = 254
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Zoom Im'
      TabOrder = 7
      Visible = False
    end
    object ZoomOutButton: TButton
      Left = 542
      Top = 335
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Zoom Out'
      TabOrder = 8
      Visible = False
    end
    object ResetButton: TButton
      Left = 666
      Top = 294
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Reset'
      TabOrder = 9
      Visible = False
    end
    object IPCameraControlPostfixEdit: TEdit
      Left = 282
      Top = 26
      Width = 482
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
      Top = 197
      Width = 754
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
      Top = 232
      Width = 148
      Height = 17
      Caption = 'Use user resolution'
      TabOrder = 12
    end
    object DesiredWidthLabeledEdit: TLabeledEdit
      Left = 164
      Top = 252
      Width = 47
      Height = 25
      EditLabel.Width = 37
      EditLabel.Height = 17
      EditLabel.Caption = 'Width'
      TabOrder = 13
      Text = '640'
    end
    object DesiredHeightLabeledEdit: TLabeledEdit
      Left = 217
      Top = 252
      Width = 48
      Height = 25
      EditLabel.Width = 39
      EditLabel.Height = 17
      EditLabel.Caption = 'Height'
      TabOrder = 14
      Text = '480'
    end
  end
end
