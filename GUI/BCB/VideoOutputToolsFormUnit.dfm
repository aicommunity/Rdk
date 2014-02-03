object VideoOutputToolsForm: TVideoOutputToolsForm
  Left = 0
  Top = 0
  BorderStyle = bsSizeToolWin
  Caption = 'VideoOutputToolsForm'
  ClientHeight = 386
  ClientWidth = 218
  Color = clBtnFace
  DockSite = True
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object FigurePanel: TPanel
    Left = 0
    Top = 0
    Width = 113
    Height = 386
    Align = alLeft
    BevelOuter = bvNone
    TabOrder = 0
    object GroupBox1: TGroupBox
      Left = 0
      Top = 142
      Width = 113
      Height = 244
      Align = alClient
      Caption = ' '#1057#1087#1080#1089#1086#1082' '#1092#1080#1075#1091#1088' '
      TabOrder = 0
      object GeometryCheckListBox: TCheckListBox
        Left = 2
        Top = 15
        Width = 109
        Height = 227
        OnClickCheck = GeometryCheckListBoxClickCheck
        Align = alClient
        ItemHeight = 13
        TabOrder = 0
        OnClick = GeometryCheckListBoxClick
      end
    end
    object GroupBox4: TGroupBox
      Left = 0
      Top = 0
      Width = 113
      Height = 142
      Align = alTop
      Caption = ' '#1060#1080#1075#1091#1088#1099' '
      TabOrder = 1
      object ColorBox: TColorBox
        Left = 2
        Top = 15
        Width = 109
        Height = 22
        Align = alTop
        Selected = clRed
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames, cbCustomColors]
        TabOrder = 0
      end
      object AddFigureButton: TButton
        Left = 2
        Top = 37
        Width = 109
        Height = 25
        Align = alTop
        Caption = #1044#1086#1073#1072#1074#1080#1090#1100' '#1092#1080#1075#1091#1088#1091
        TabOrder = 1
        OnClick = AddFigureButtonClick
      end
      object DelFigureButton: TButton
        Left = 2
        Top = 87
        Width = 109
        Height = 25
        Align = alTop
        Caption = #1059#1076#1072#1083#1080#1090#1100' '#1092#1080#1075#1091#1088#1091
        TabOrder = 2
        OnClick = DelFigureButtonClick
      end
      object DelAllFiguresButton: TButton
        Left = 2
        Top = 112
        Width = 109
        Height = 25
        Align = alTop
        Caption = #1059#1076#1072#1083#1080#1090#1100' '#1074#1089#1077
        TabOrder = 3
        OnClick = DelAllFiguresButtonClick
      end
      object EditFigureButton: TButton
        Left = 2
        Top = 62
        Width = 109
        Height = 25
        Align = alTop
        Caption = #1055#1088#1072#1074#1080#1090#1100' '#1092#1080#1075#1091#1088#1091
        TabOrder = 4
        OnClick = EditFigureButtonClick
      end
    end
  end
  object PointsPanel: TPanel
    Left = 113
    Top = 0
    Width = 105
    Height = 386
    Align = alClient
    TabOrder = 1
    object GroupBox2: TGroupBox
      Left = 1
      Top = 1
      Width = 103
      Height = 141
      Align = alTop
      Caption = ' '#1058#1086#1095#1082#1080' '
      TabOrder = 0
      object AddPointButton: TButton
        Left = 2
        Top = 36
        Width = 99
        Height = 25
        Align = alTop
        Caption = #1044#1086#1073#1072#1074#1080#1090#1100' '#1090#1086#1095#1082#1091
        TabOrder = 0
        OnClick = AddPointButtonClick
      end
      object DelPointButton: TButton
        Left = 2
        Top = 86
        Width = 99
        Height = 25
        Align = alTop
        Caption = #1059#1076#1072#1083#1080#1090#1100' '#1090#1086#1095#1082#1091
        TabOrder = 1
        OnClick = DelPointButtonClick
      end
      object DelAllPointsButton: TButton
        Left = 2
        Top = 111
        Width = 99
        Height = 25
        Align = alTop
        Caption = #1059#1076#1072#1083#1080#1090#1100' '#1074#1089#1077
        TabOrder = 2
        OnClick = DelAllPointsButtonClick
      end
      object EditPointButton: TButton
        Left = 2
        Top = 61
        Width = 99
        Height = 25
        Align = alTop
        Caption = #1055#1088#1072#1074#1080#1090#1100' '#1090#1086#1095#1082#1091
        TabOrder = 3
        OnClick = EditPointButtonClick
      end
      object PointNameEdit: TEdit
        Left = 2
        Top = 15
        Width = 99
        Height = 21
        Align = alTop
        TabOrder = 4
        Text = 'v'
      end
    end
    object GroupBox3: TGroupBox
      Left = 1
      Top = 142
      Width = 103
      Height = 243
      Align = alClient
      Caption = ' '#1057#1087#1080#1089#1086#1082' '#1090#1086#1095#1077#1082' '
      TabOrder = 1
      object PointsCheckListBox: TCheckListBox
        Left = 2
        Top = 15
        Width = 99
        Height = 226
        Align = alClient
        ItemHeight = 13
        TabOrder = 0
        OnClick = PointsCheckListBoxClick
      end
    end
  end
end
