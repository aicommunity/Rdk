object ShowProgressBarForm: TShowProgressBarForm
  Left = 193
  Top = 123
  BorderIcons = []
  BorderStyle = bsDialog
  Caption = #1055#1086#1078#1072#1083#1091#1081#1089#1090#1072' '#1087#1086#1076#1086#1078#1076#1080#1090#1077' '#1079#1072#1074#1077#1088#1096#1077#1085#1080#1103' '#1086#1087#1077#1088#1072#1094#1080#1080
  ClientHeight = 83
  ClientWidth = 462
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poDesktopCenter
  OnShow = FormShow
  DesignSize = (
    462
    83)
  PixelsPerInch = 96
  TextHeight = 13
  object ProgressBar1: TProgressBar
    Left = 0
    Top = 22
    Width = 462
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
  end
  object ProgressBar2: TProgressBar
    Left = 0
    Top = 65
    Width = 462
    Height = 13
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 1
  end
  object StaticText1: TStaticText
    Left = 0
    Top = 2
    Width = 462
    Height = 17
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = #1058#1077#1082#1091#1097#1072#1103' '#1086#1087#1077#1088#1072#1094#1080#1103':'
    Color = clBtnFace
    ParentColor = False
    TabOrder = 2
  end
  object StaticText2: TStaticText
    Left = 0
    Top = 46
    Width = 462
    Height = 16
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = #1042#1089#1077#1075#1086':'
    TabOrder = 3
  end
end
