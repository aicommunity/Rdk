inherited ApplicationOptionsFrame: TApplicationOptionsFrame
  Width = 367
  Height = 301
  ExplicitWidth = 367
  ExplicitHeight = 301
  DesignSize = (
    367
    301)
  object Label1: TLabel
    Left = 3
    Top = 225
    Width = 124
    Height = 13
    Caption = 'Open Project History Limit'
  end
  object AutoStartProjectCheckBox: TCheckBox
    Left = 3
    Top = 156
    Width = 352
    Height = 17
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Run Application After Loading'
    TabOrder = 0
  end
  object ApplicationNameLabeledEdit: TLabeledEdit
    Left = 3
    Top = 23
    Width = 352
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    EditLabel.Width = 82
    EditLabel.Height = 13
    EditLabel.Caption = 'Application Name'
    TabOrder = 1
  end
  object MainFormInternalNameLabeledEdit: TLabeledEdit
    Left = 3
    Top = 67
    Width = 271
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    EditLabel.Width = 120
    EditLabel.Height = 13
    EditLabel.Caption = 'Main Form Internal Name'
    TabOrder = 2
  end
  object AutoStartProjectNameLabeledEdit: TLabeledEdit
    Left = 3
    Top = 133
    Width = 271
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    EditLabel.Width = 130
    EditLabel.Height = 13
    EditLabel.Caption = 'Auto Loading Project Name'
    TabOrder = 3
  end
  object BrowseAutoLoadingProjectNameButton: TButton
    Left = 280
    Top = 131
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Browse'
    Enabled = False
    TabOrder = 4
  end
  object BrowseMainFormNameButton: TButton
    Left = 280
    Top = 65
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Select'
    Enabled = False
    TabOrder = 5
  end
  object HideAdminFormCheckBox: TCheckBox
    Left = 3
    Top = 91
    Width = 102
    Height = 17
    Caption = 'Hide Admin Form'
    TabOrder = 6
  end
  object DisableAdminFormCheckBox: TCheckBox
    Left = 111
    Top = 91
    Width = 244
    Height = 17
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Disable Admin Form'
    TabOrder = 7
  end
  object MinimizeToTrayCheckBox: TCheckBox
    Left = 3
    Top = 179
    Width = 352
    Height = 17
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Minimize To Tray'
    TabOrder = 8
  end
  object StartMinimizedCheckBox: TCheckBox
    Left = 3
    Top = 202
    Width = 352
    Height = 17
    Anchors = [akLeft, akTop, akRight]
    Caption = 'Start Minimized'
    TabOrder = 9
  end
  object ProjectHistorySpinEdit: TSpinEdit
    Left = 133
    Top = 222
    Width = 41
    Height = 22
    MaxValue = 0
    MinValue = 0
    TabOrder = 10
    Value = 10
  end
end
