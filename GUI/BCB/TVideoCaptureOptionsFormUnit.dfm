object VideoCaptureOptionsForm: TVideoCaptureOptionsForm
  Left = 0
  Top = 0
  Caption = 'Video Capture Options'
  ClientHeight = 243
  ClientWidth = 527
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 202
    Width = 527
    Height = 41
    Align = alBottom
    TabOrder = 0
    DesignSize = (
      527
      41)
    object ApplyButton: TButton
      Left = 279
      Top = 9
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Apply'
      Default = True
      TabOrder = 0
      OnClick = ApplyButtonClick
    end
    object Cancel: TButton
      Left = 442
      Top = 9
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      TabOrder = 1
      OnClick = CancelClick
    end
    object RestoreButton: TButton
      Left = 361
      Top = 9
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Restore'
      TabOrder = 2
      OnClick = RestoreButtonClick
    end
  end
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 527
    Height = 202
    Align = alClient
    TabOrder = 1
  end
end
