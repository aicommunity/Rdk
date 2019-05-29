inherited DebugControlFrame: TDebugControlFrame
  Width = 465
  Height = 389
  ExplicitWidth = 465
  ExplicitHeight = 389
  object DebugImagesGroupBox: TGroupBox
    Left = 0
    Top = 128
    Width = 465
    Height = 261
    Align = alBottom
    Caption = #1054#1090#1083#1072#1076#1086#1095#1085#1099#1077' '#1080#1079#1086#1073#1088#1072#1078#1077#1085#1080#1103
    TabOrder = 0
    DesignSize = (
      465
      261)
    object ImgTLLabel: TLabel
      Left = 16
      Top = 63
      Width = 80
      Height = 13
      Caption = #1051#1077#1074#1086#1077' '#1074#1077#1088#1093#1085#1077#1077':'
    end
    object ImgTRLabel: TLabel
      Left = 205
      Top = 63
      Width = 86
      Height = 13
      Caption = #1055#1088#1072#1074#1086#1077' '#1074#1077#1088#1093#1085#1077#1077':'
    end
    object ImgRBLabel: TLabel
      Left = 205
      Top = 103
      Width = 82
      Height = 13
      Caption = #1055#1088#1072#1074#1086#1077' '#1085#1080#1078#1085#1077#1077':'
    end
    object ImgLBLabel: TLabel
      Left = 16
      Top = 103
      Width = 76
      Height = 13
      Caption = #1051#1077#1074#1086#1077' '#1085#1080#1078#1085#1077#1077':'
    end
    object EnableDebugImgSaveCheckBox: TCheckBox
      Left = 16
      Top = 16
      Width = 433
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = #1042#1082#1083#1102#1095#1080#1090#1100' '#1089#1086#1093#1088#1072#1085#1077#1085#1080#1077' '#1086#1090#1083#1072#1076#1086#1095#1085#1099#1093' '#1080#1079#1086#1073#1088#1072#1078#1077#1085#1080#1081
      TabOrder = 0
    end
    object ImgTLEdit: TEdit
      Left = 16
      Top = 82
      Width = 143
      Height = 21
      TabOrder = 1
    end
    object ImgTLButton: TButton
      Left = 158
      Top = 81
      Width = 41
      Height = 25
      Caption = '...'
      TabOrder = 2
      OnClick = ImgTLButtonClick
    end
    object ImgTREdit: TEdit
      Left = 205
      Top = 82
      Width = 143
      Height = 21
      TabOrder = 3
    end
    object ImgTRButton: TButton
      Left = 347
      Top = 81
      Width = 41
      Height = 25
      Caption = '...'
      TabOrder = 4
      OnClick = ImgTRButtonClick
    end
    object ImgRBEdit: TEdit
      Left = 205
      Top = 122
      Width = 143
      Height = 21
      TabOrder = 5
    end
    object ImgRBButton: TButton
      Left = 347
      Top = 121
      Width = 41
      Height = 25
      Caption = '...'
      TabOrder = 6
      OnClick = ImgRBButtonClick
    end
    object ImgLBButton: TButton
      Left = 158
      Top = 121
      Width = 41
      Height = 25
      Caption = '...'
      TabOrder = 7
      OnClick = ImgLBButtonClick
    end
    object ImgLBEdit: TEdit
      Left = 16
      Top = 122
      Width = 143
      Height = 21
      TabOrder = 8
    end
    object ApplyButton: TButton
      Left = 16
      Top = 172
      Width = 164
      Height = 36
      Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
      TabOrder = 9
      OnClick = ApplyButtonClick
    end
    object SaveOneImageButton: TButton
      Left = 205
      Top = 171
      Width = 164
      Height = 36
      Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1086#1076#1085#1086
      TabOrder = 10
      OnClick = SaveOneImageButtonClick
    end
    object SaveImgCurrentChannelRadioButton: TRadioButton
      Left = 16
      Top = 149
      Width = 113
      Height = 17
      Caption = #1058#1077#1082#1091#1097#1080#1081' '#1082#1072#1085#1072#1083
      Checked = True
      TabOrder = 11
      TabStop = True
    end
    object SaveImgActiveChannelsRadioButton: TRadioButton
      Left = 135
      Top = 148
      Width = 138
      Height = 17
      Caption = #1042#1089#1077' '#1072#1082#1090#1080#1074#1085#1099#1077' '#1082#1072#1085#1072#1083#1099
      TabOrder = 12
    end
    object SaveOnlyOnceCheckBox: TCheckBox
      Left = 16
      Top = 40
      Width = 433
      Height = 17
      Anchors = [akLeft, akTop, akRight]
      Caption = #1057#1086#1093#1088#1072#1085#1103#1090#1100' '#1090#1086#1083#1100#1082#1086' '#1086#1076#1080#1085' '#1088#1072#1079
      Checked = True
      Enabled = False
      State = cbChecked
      TabOrder = 13
    end
  end
  object SnapshotSettingsGroupBox: TGroupBox
    Left = 0
    Top = 0
    Width = 465
    Height = 128
    Align = alClient
    Caption = #1057#1086#1093#1088#1072#1085#1077#1085#1080#1077' '#1089#1085#1080#1084#1082#1072' '#1089#1086#1089#1090#1086#1103#1085#1080#1103
    TabOrder = 1
    object MakeSnapshotButton: TButton
      Left = 16
      Top = 86
      Width = 164
      Height = 36
      Caption = #1057#1076#1077#1083#1072#1090#1100' '#1089#1085#1080#1084#1086#1082
      TabOrder = 0
      OnClick = MakeSnapshotButtonClick
    end
    object SaveSnapshotCheckBox: TCheckBox
      Left = 16
      Top = 40
      Width = 225
      Height = 17
      Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1089#1086#1089#1090#1086#1103#1085#1080#1077' '#1084#1086#1076#1077#1083#1080
      TabOrder = 1
    end
    object SaveImageCheckBox: TCheckBox
      Left = 16
      Top = 63
      Width = 372
      Height = 17
      Caption = #1057#1086#1093#1088#1072#1085#1080#1090#1100' '#1080#1079#1086#1073#1088#1072#1078#1077#1085#1080#1077
      TabOrder = 2
    end
    object SnapshotOneRadioButton: TRadioButton
      Left = 16
      Top = 17
      Width = 113
      Height = 17
      Caption = #1058#1077#1082#1091#1097#1080#1081' '#1082#1072#1085#1072#1083
      Checked = True
      TabOrder = 3
      TabStop = True
    end
    object SnapshotActiveRadioButton: TRadioButton
      Left = 135
      Top = 17
      Width = 138
      Height = 17
      Caption = #1042#1089#1077' '#1072#1082#1090#1080#1074#1085#1099#1077' '#1082#1072#1085#1072#1083#1099
      TabOrder = 4
    end
  end
end
