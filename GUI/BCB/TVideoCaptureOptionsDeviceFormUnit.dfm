object VideoCaptureOptionsDeviceForm: TVideoCaptureOptionsDeviceForm
  Left = 0
  Top = 0
  ClientHeight = 307
  ClientWidth = 433
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  Position = poOwnerFormCenter
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object VDCapturePanel: TPanel
    Left = 0
    Top = 0
    Width = 433
    Height = 307
    Align = alClient
    TabOrder = 0
    DesignSize = (
      433
      307)
    object ModeLabel: TLabel
      Left = 8
      Top = 89
      Width = 417
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video Size:'
      ExplicitWidth = 390
    end
    object InputLabel: TLabel
      Left = 8
      Top = 47
      Width = 417
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video Input:'
      ExplicitWidth = 390
    end
    object DeviceLabel: TLabel
      Left = 8
      Top = 3
      Width = 417
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video device:'
      ExplicitWidth = 429
    end
    object Label1: TLabel
      Left = 8
      Top = 137
      Width = 417
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Subtype:'
      ExplicitWidth = 429
    end
    object Label2: TLabel
      Left = 8
      Top = 218
      Width = 417
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Analog video standard:'
      ExplicitWidth = 428
    end
    object VideoSizeComboBox: TComboBox
      Left = 8
      Top = 104
      Width = 417
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
    end
    object InputComboBox: TComboBox
      Left = 8
      Top = 62
      Width = 417
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 1
    end
    object DeviceComboBox: TComboBox
      Left = 8
      Top = 19
      Width = 417
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      OnSelect = DeviceComboBoxSelect
    end
    object VideoSubTypeComboBox: TComboBox
      Left = 8
      Top = 152
      Width = 417
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 3
    end
    object AnalogVideoStandardComboBox: TComboBox
      Left = 8
      Top = 237
      Width = 417
      Height = 21
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 4
    end
    object StreamButton: TButton
      Left = 8
      Top = 268
      Width = 75
      Height = 25
      Caption = 'Stream'
      TabOrder = 5
    end
    object FpsLabeledEdit: TLabeledEdit
      Left = 8
      Top = 191
      Width = 417
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 17
      EditLabel.Height = 13
      EditLabel.Caption = 'Fps'
      TabOrder = 6
    end
  end
end
