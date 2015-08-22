object VideoCaptureOptionsBmpForm: TVideoCaptureOptionsBmpForm
  Left = 0
  Top = 0
  ClientHeight = 248
  ClientWidth = 580
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
    580
    248)
  PixelsPerInch = 120
  TextHeight = 17
  object Label3: TLabel
    Left = 10
    Top = 4
    Width = 516
    Height = 16
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
    Top = 61
    Width = 191
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
    Top = 109
    Width = 61
    Height = 25
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
    Left = 474
    Top = 25
    Width = 99
    Height = 31
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
    Top = 28
    Width = 456
    Height = 25
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 3
  end
  object PicturesOpenDialog: TOpenDialog
    DefaultExt = 'bmp'
    Filter = #1060#1086#1090#1086'|*.bmp,*.jpg,*jpeg|'#1042#1089#1077' '#1092#1072#1081#1083#1099'|*.*'
    FilterIndex = 0
    Left = 288
    Top = 112
  end
end
