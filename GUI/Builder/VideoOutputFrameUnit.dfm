object VideoOutputFrame: TVideoOutputFrame
  Left = 0
  Top = 0
  Width = 702
  Height = 458
  Constraints.MinHeight = 100
  Constraints.MinWidth = 200
  PopupMenu = PopupMenu
  TabOrder = 0
  TabStop = True
  object GroupBox: TGroupBox
    Left = 0
    Top = 0
    Width = 702
    Height = 417
    Align = alClient
    TabOrder = 0
    object Image: TImage
      Left = 2
      Top = 15
      Width = 698
      Height = 400
      Align = alClient
      Stretch = True
      OnMouseDown = ImageMouseDown
      OnMouseMove = ImageMouseMove
      OnMouseUp = ImageMouseUp
      ExplicitTop = 17
      ExplicitWidth = 378
      ExplicitHeight = 291
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 417
    Width = 702
    Height = 41
    Align = alBottom
    TabOrder = 1
    Visible = False
    DesignSize = (
      702
      41)
    object ImageTrackBar: TTrackBar
      Left = 0
      Top = 8
      Width = 546
      Height = 29
      Anchors = [akLeft, akTop, akRight]
      Enabled = False
      TabOrder = 0
      OnChange = ImageTrackBarChange
    end
    object StopButton: TButton
      Left = 665
      Top = 6
      Width = 30
      Height = 25
      Anchors = [akTop, akRight]
      Caption = #1057#1090#1086#1087
      Enabled = False
      TabOrder = 1
      OnClick = StopButtonClick
    end
    object StartButton: TButton
      Left = 629
      Top = 6
      Width = 30
      Height = 25
      Anchors = [akTop, akRight]
      Caption = #1055#1091#1089#1082
      Enabled = False
      TabOrder = 2
      OnClick = StartButtonClick
    end
    object TimeEdit: TMaskEdit
      Left = 544
      Top = 8
      Width = 79
      Height = 21
      Anchors = [akTop, akRight]
      EditMask = '000\:00\:00\:00;1;_'
      MaxLength = 12
      TabOrder = 3
      Text = '000:00:00:00'
      OnChange = TimeEditChange
    end
  end
  object Timer: TTimer
    Enabled = False
    Interval = 39
    OnTimer = TimerTimer
    Left = 8
    Top = 264
  end
  object PopupMenu: TPopupMenu
    Left = 48
    Top = 264
    object N1: TMenuItem
      Caption = #1055#1072#1085#1077#1083#1100' '#1080#1085#1089#1090#1088#1091#1084#1077#1085#1090#1086#1074
      OnClick = N1Click
    end
  end
end
