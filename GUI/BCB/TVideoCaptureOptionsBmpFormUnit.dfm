object VideoCaptureOptionsBmpForm: TVideoCaptureOptionsBmpForm
  Left = 0
  Top = 0
  ClientHeight = 198
  ClientWidth = 464
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = True
  Position = poOwnerFormCenter
  OnCreate = FormCreate
  DesignSize = (
    464
    198)
  PixelsPerInch = 96
  TextHeight = 13
  object Label3: TLabel
    Left = 8
    Top = 3
    Width = 413
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Image file name:'
    ExplicitWidth = 369
  end
  object PictureTruncPathCheckBox: TCheckBox
    Left = 8
    Top = 49
    Width = 153
    Height = 17
    Caption = 'Truncate full path'
    TabOrder = 0
  end
  object PictureFileFpsLabeledEdit: TLabeledEdit
    Left = 8
    Top = 87
    Width = 49
    Height = 21
    EditLabel.Width = 22
    EditLabel.Height = 13
    EditLabel.Caption = 'FPS:'
    TabOrder = 1
    Text = '25.0'
  end
  object OpenImageFileButton: TButton
    Left = 379
    Top = 20
    Width = 79
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Browse'
    TabOrder = 2
    OnClick = OpenImageFileButtonClick
  end
  object ImageFileNameEdit: TEdit
    Left = 8
    Top = 22
    Width = 365
    Height = 21
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
