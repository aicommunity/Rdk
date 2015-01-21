object VideoCaptureOptionsDeviceForm: TVideoCaptureOptionsDeviceForm
  Left = 0
  Top = 0
  ClientHeight = 340
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
    Height = 340
    Align = alClient
    TabOrder = 0
    DesignSize = (
      433
      340)
    object ModeLabel: TLabel
      Left = 8
      Top = 89
      Width = 417
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video Size:'
    end
    object InputLabel: TLabel
      Left = 8
      Top = 47
      Width = 417
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video Input:'
    end
    object DeviceLabel: TLabel
      Left = 8
      Top = 3
      Width = 417
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video device:'
    end
    object Label1: TLabel
      Left = 8
      Top = 170
      Width = 417
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Subtype:'
    end
    object Label2: TLabel
      Left = 8
      Top = 251
      Width = 417
      Height = 13
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Analog video standard:'
    end
    object VideoSizeComboBox: TComboBox
      Left = 8
      Top = 104
      Width = 417
      Height = 25
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
    end
    object InputComboBox: TComboBox
      Left = 8
      Top = 62
      Width = 417
      Height = 25
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 1
    end
    object DeviceComboBox: TComboBox
      Left = 8
      Top = 19
      Width = 417
      Height = 25
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      OnSelect = DeviceComboBoxSelect
    end
    object VideoSubTypeComboBox: TComboBox
      Left = 8
      Top = 185
      Width = 417
      Height = 25
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 3
    end
    object AnalogVideoStandardComboBox: TComboBox
      Left = 8
      Top = 270
      Width = 417
      Height = 25
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 4
    end
    object StreamButton: TButton
      Left = 8
      Top = 301
      Width = 75
      Height = 25
      Caption = 'Stream'
      TabOrder = 5
    end
    object FpsLabeledEdit: TLabeledEdit
      Left = 8
      Top = 224
      Width = 417
      Height = 25
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 17
      EditLabel.Height = 13
      EditLabel.Caption = 'Fps'
      TabOrder = 6
    end
    object DesiredResFlagCheckBox: TCheckBox
      Left = 8
      Top = 128
      Width = 113
      Height = 13
      Margins.Left = 2
      Margins.Top = 2
      Margins.Right = 2
      Margins.Bottom = 2
      Caption = 'Use user resolution'
      TabOrder = 7
    end
    object DesiredWidthLabeledEdit: TLabeledEdit
      Left = 125
      Top = 145
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
      TabOrder = 8
      Text = '640'
    end
    object DesiredHeightLabeledEdit: TLabeledEdit
      Left = 166
      Top = 145
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
      TabOrder = 9
      Text = '480'
    end
  end
end
