object VideoCaptureOptionsDeviceFrame: TVideoCaptureOptionsDeviceFrame
  Left = 0
  Top = 0
  Width = 461
  Height = 267
  TabOrder = 0
  object VDCapturePanel: TPanel
    Left = 0
    Top = 0
    Width = 461
    Height = 267
    Align = alClient
    TabOrder = 0
    ExplicitTop = -96
    ExplicitWidth = 445
    ExplicitHeight = 266
    DesignSize = (
      461
      267)
    object ModeLabel: TLabel
      Left = 8
      Top = 89
      Width = 445
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video Size:'
      ExplicitWidth = 390
    end
    object InputLabel: TLabel
      Left = 8
      Top = 47
      Width = 445
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video Input:'
      ExplicitWidth = 390
    end
    object DeviceLabel: TLabel
      Left = 8
      Top = 3
      Width = 445
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video device:'
      ExplicitWidth = 429
    end
    object Label1: TLabel
      Left = 8
      Top = 137
      Width = 445
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Subtype:'
      ExplicitWidth = 429
    end
    object Label2: TLabel
      Left = 8
      Top = 185
      Width = 445
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Analog video standard:'
      ExplicitWidth = 429
    end
    object VideoSizeComboBox: TComboBox
      Left = 8
      Top = 104
      Width = 445
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
      ExplicitWidth = 429
    end
    object InputComboBox: TComboBox
      Left = 8
      Top = 62
      Width = 445
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 1
      ExplicitWidth = 429
    end
    object DeviceComboBox: TComboBox
      Left = 8
      Top = 19
      Width = 445
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      ExplicitWidth = 429
    end
    object VideoSubTypeComboBox: TComboBox
      Left = 8
      Top = 152
      Width = 445
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 3
      ExplicitWidth = 429
    end
    object AnalogVideoStandardComboBox: TComboBox
      Left = 8
      Top = 200
      Width = 445
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 4
      ExplicitWidth = 429
    end
    object StreamButton: TButton
      Left = 8
      Top = 231
      Width = 75
      Height = 25
      Caption = 'Stream'
      TabOrder = 5
    end
  end
end
