object VideoCaptureOptionsDeviceForm: TVideoCaptureOptionsDeviceForm
  Left = 0
  Top = 0
  ClientHeight = 498
  ClientWidth = 566
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  Position = poOwnerFormCenter
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 17
  object VDCapturePanel: TPanel
    Left = 0
    Top = 0
    Width = 566
    Height = 498
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    TabOrder = 0
    ExplicitHeight = 445
    DesignSize = (
      566
      498)
    object ModeLabel: TLabel
      Left = 10
      Top = 116
      Width = 546
      Height = 17
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video Size:'
    end
    object InputLabel: TLabel
      Left = 10
      Top = 61
      Width = 546
      Height = 17
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video Input:'
    end
    object DeviceLabel: TLabel
      Left = 10
      Top = 4
      Width = 546
      Height = 17
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Video device:'
    end
    object Label1: TLabel
      Left = 10
      Top = 222
      Width = 546
      Height = 17
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Subtype:'
    end
    object Label2: TLabel
      Left = 10
      Top = 328
      Width = 546
      Height = 17
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akLeft, akTop, akRight]
      AutoSize = False
      Caption = 'Analog video standard:'
    end
    object VideoSizeComboBox: TComboBox
      Left = 10
      Top = 136
      Width = 546
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 2
    end
    object InputComboBox: TComboBox
      Left = 10
      Top = 81
      Width = 546
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 1
    end
    object DeviceComboBox: TComboBox
      Left = 10
      Top = 25
      Width = 546
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      OnSelect = DeviceComboBoxSelect
    end
    object VideoSubTypeComboBox: TComboBox
      Left = 10
      Top = 242
      Width = 546
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 3
    end
    object AnalogVideoStandardComboBox: TComboBox
      Left = 10
      Top = 353
      Width = 546
      Height = 25
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Style = csDropDownList
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 4
    end
    object StreamButton: TButton
      Left = 10
      Top = 450
      Width = 99
      Height = 32
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Stream'
      TabOrder = 5
    end
    object FpsLabeledEdit: TLabeledEdit
      Left = 10
      Top = 293
      Width = 546
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
      TabOrder = 6
    end
    object DesiredResFlagCheckBox: TCheckBox
      Left = 10
      Top = 167
      Width = 148
      Height = 17
      Caption = 'Use user resolution'
      TabOrder = 7
    end
    object DesiredWidthLabeledEdit: TLabeledEdit
      Left = 163
      Top = 190
      Width = 48
      Height = 25
      EditLabel.Width = 37
      EditLabel.Height = 17
      EditLabel.Caption = 'Width'
      TabOrder = 8
      Text = '640'
    end
    object DesiredHeightLabeledEdit: TLabeledEdit
      Left = 217
      Top = 190
      Width = 48
      Height = 25
      EditLabel.Width = 39
      EditLabel.Height = 17
      EditLabel.Caption = 'Height'
      TabOrder = 9
      Text = '480'
    end
    object TVTunerChannelLabeledEdit: TLabeledEdit
      Left = 10
      Top = 410
      Width = 167
      Height = 25
      EditLabel.Width = 167
      EditLabel.Height = 17
      EditLabel.Caption = 'Tv Tuner Channel Number:'
      TabOrder = 10
    end
  end
end
