object VideoCaptureOptionsBmpForm: TVideoCaptureOptionsBmpForm
  Left = 0
  Top = 0
  ClientHeight = 259
  ClientWidth = 607
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  Position = poOwnerFormCenter
  OnCreate = FormCreate
  DesignSize = (
    607
    259)
  PixelsPerInch = 120
  TextHeight = 17
  object Label3: TLabel
    Left = 10
    Top = 4
    Width = 541
    Height = 17
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Image file name:'
  end
  object PictureTruncPathCheckBox: TCheckBox
    Left = 10
    Top = 64
    Width = 201
    Height = 22
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Caption = 'Truncate full path'
    TabOrder = 0
  end
  object PictureFileFpsLabeledEdit: TLabeledEdit
    Left = 10
    Top = 114
    Width = 65
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    EditLabel.Width = 28
    EditLabel.Height = 17
    EditLabel.Margins.Left = 4
    EditLabel.Margins.Top = 4
    EditLabel.Margins.Right = 4
    EditLabel.Margins.Bottom = 4
    EditLabel.Caption = 'FPS:'
    TabOrder = 1
    Text = '25.0'
  end
  object OpenImageFileButton: TButton
    Left = 496
    Top = 26
    Width = 103
    Height = 33
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akTop, akRight]
    Caption = 'Browse'
    TabOrder = 2
    OnClick = OpenImageFileButtonClick
  end
  object ImageFileNameEdit: TEdit
    Left = 10
    Top = 29
    Width = 478
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 3
  end
  object PicturesOpenDialog: TOpenDialog
    DefaultExt = 'bmp'
    Filter = #1060#1086#1090#1086'|*.bmp|'#1042#1089#1077' '#1092#1072#1081#1083#1099'|*.*'
    FilterIndex = 0
    Left = 288
    Top = 112
  end
end
