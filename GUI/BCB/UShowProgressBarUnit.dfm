object UShowProgressBarForm: TUShowProgressBarForm
  Left = 193
  Top = 123
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = 'Please wait'
  ClientHeight = 102
  ClientWidth = 569
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  DesignSize = (
    569
    102)
  PixelsPerInch = 96
  TextHeight = 16
  object ProgressBar1: TProgressBar
    Left = 0
    Top = 27
    Width = 569
    Height = 16
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
  end
  object ProgressBar2: TProgressBar
    Left = 0
    Top = 80
    Width = 569
    Height = 16
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 1
  end
  object StaticText1: TStaticText
    Left = 0
    Top = 2
    Width = 569
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Current:'
    Color = clBtnFace
    ParentColor = False
    TabOrder = 2
  end
  object StaticText2: TStaticText
    Left = 0
    Top = 57
    Width = 569
    Height = 19
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 'Total:'
    TabOrder = 3
  end
end
