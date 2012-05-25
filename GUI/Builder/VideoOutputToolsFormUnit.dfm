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
    ExplicitHeight = 396
    object GroupBox1: TGroupBox
      Left = 0
      Top = 174
      Width = 113
      Height = 212
      Align = alClient
      Caption = ' '#1057#1087#1080#1089#1086#1082' '#1092#1080#1075#1091#1088' '
      TabOrder = 0
      ExplicitHeight = 222
      object GeometryCheckListBox: TCheckListBox
        Left = 2
        Top = 15
        Width = 109
        Height = 195
        OnClickCheck = GeometryCheckListBoxClickCheck
        Align = alClient
        ItemHeight = 13
        TabOrder = 0
        OnClick = GeometryCheckListBoxClick
        ExplicitHeight = 205
      end
    end
    object GroupBox4: TGroupBox
      Left = 0
      Top = 0
      Width = 113
      Height = 174
      Align = alTop
      Caption = ' '#1060#1080#1075#1091#1088#1099' '
      TabOrder = 1
      object ColorBox: TColorBox
        Left = 6
        Top = 19
        Width = 99
        Height = 22
        Selected = clRed
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames, cbCustomColors]
        TabOrder = 0
      end
      object AddFigureButton: TButton
        Left = 6
        Top = 47
        Width = 99
        Height = 25
        Caption = #1044#1086#1073#1072#1074#1080#1090#1100' '#1092#1080#1075#1091#1088#1091
        TabOrder = 1
        OnClick = AddFigureButtonClick
      end
      object DelFigureButton: TButton
        Left = 6
        Top = 109
        Width = 99
        Height = 25
        Caption = #1059#1076#1072#1083#1080#1090#1100' '#1092#1080#1075#1091#1088#1091
        TabOrder = 2
        OnClick = DelFigureButtonClick
      end
      object DelAllFiguresButton: TButton
        Left = 6
        Top = 141
        Width = 99
        Height = 25
        Caption = #1059#1076#1072#1083#1080#1090#1100' '#1074#1089#1077
        TabOrder = 3
        OnClick = DelAllFiguresButtonClick
      end
      object EditFigureButton: TButton
        Left = 6
        Top = 78
        Width = 99
        Height = 25
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
    ExplicitWidth = 115
    ExplicitHeight = 396
    object GroupBox2: TGroupBox
      Left = 1
      Top = 1
      Width = 103
      Height = 174
      Align = alTop
      Caption = ' '#1058#1086#1095#1082#1080' '
      TabOrder = 0
      ExplicitWidth = 113
      object AddPointButton: TButton
        Left = 6
        Top = 46
        Width = 99
        Height = 25
        Caption = #1044#1086#1073#1072#1074#1080#1090#1100' '#1090#1086#1095#1082#1091
        TabOrder = 0
        OnClick = AddPointButtonClick
      end
      object DelPointButton: TButton
        Left = 5
        Top = 108
        Width = 99
        Height = 25
        Caption = #1059#1076#1072#1083#1080#1090#1100' '#1090#1086#1095#1082#1091
        TabOrder = 1
      end
      object DelAllPointsButton: TButton
        Left = 6
        Top = 140
        Width = 99
        Height = 25
        Caption = #1059#1076#1072#1083#1080#1090#1100' '#1074#1089#1077
        TabOrder = 2
        OnClick = DelAllPointsButtonClick
      end
      object EditPointButton: TButton
        Left = 6
        Top = 77
        Width = 99
        Height = 25
        Caption = #1055#1088#1072#1074#1080#1090#1100' '#1090#1086#1095#1082#1091
        TabOrder = 3
        OnClick = EditPointButtonClick
      end
      object PointNameEdit: TEdit
        Left = 6
        Top = 18
        Width = 99
        Height = 21
        TabOrder = 4
        Text = 'v'
      end
    end
    object GroupBox3: TGroupBox
      Left = 1
      Top = 175
      Width = 103
      Height = 210
      Align = alClient
      Caption = ' '#1057#1087#1080#1089#1086#1082' '#1090#1086#1095#1077#1082' '
      TabOrder = 1
      ExplicitWidth = 113
      ExplicitHeight = 220
      object PointsCheckListBox: TCheckListBox
        Left = 2
        Top = 15
        Width = 99
        Height = 193
        Align = alClient
        ItemHeight = 13
        TabOrder = 0
        OnClick = PointsCheckListBoxClick
        ExplicitWidth = 109
        ExplicitHeight = 203
      end
    end
  end
end
