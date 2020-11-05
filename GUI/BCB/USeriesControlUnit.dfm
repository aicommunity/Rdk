object USeriesControlForm: TUSeriesControlForm
  Left = 360
  Top = 248
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #1044#1086#1073#1072#1074#1083#1077#1085#1080#1077'/'#1091#1076#1072#1083#1077#1085#1080#1077'/'#1085#1072#1089#1090#1088#1086#1081#1082#1072' '#1085#1072#1073#1083#1102#1076#1077#1085#1080#1081
  ClientHeight = 549
  ClientWidth = 651
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poScreenCenter
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 367
    Height = 549
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object Panel4: TPanel
      Left = 0
      Top = 0
      Width = 367
      Height = 225
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
      object Panel5: TPanel
        Left = 295
        Top = 0
        Width = 72
        Height = 225
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alRight
        BevelOuter = bvNone
        TabOrder = 0
        object Button1: TButton
          Left = 12
          Top = 16
          Width = 51
          Height = 25
          Hint = #1053#1072#1095#1072#1090#1100' '#1085#1072#1073#1083#1102#1076#1077#1085#1080#1077' '#1079#1072' '#1074#1089#1077#1084' '#1090#1086#1095#1082#1072#1084#1080
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = '>>>'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
          OnClick = Button1Click
        end
        object Button2: TButton
          Left = 12
          Top = 63
          Width = 51
          Height = 25
          Hint = #1053#1072#1095#1072#1090#1100' '#1085#1072#1073#1083#1102#1076#1077#1085#1080#1077
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = '>'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
          OnClick = Button2Click
        end
        object Button3: TButton
          Left = 12
          Top = 95
          Width = 51
          Height = 25
          Hint = #1055#1088#1077#1088#1074#1072#1090#1100' '#1085#1072#1073#1083#1102#1076#1077#1085#1080#1077
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = '<'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
          OnClick = Button3Click
        end
        object Button4: TButton
          Left = 12
          Top = 174
          Width = 51
          Height = 24
          Hint = #1055#1088#1077#1088#1074#1072#1090#1100' '#1085#1072#1073#1083#1102#1076#1077#1085#1080#1077' '#1079#1072' '#1074#1089#1077#1084#1080' '#1090#1086#1095#1082#1072#1084#1080
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = '<<<'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 3
          OnClick = Button4Click
        end
        object DelPointButton: TButton
          Left = 12
          Top = 127
          Width = 51
          Height = 25
          Hint = #1059#1076#1072#1083#1080#1090#1100' '#1090#1086#1095#1082#1091
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = '<X'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 4
          OnClick = DelPointButtonClick
        end
      end
      object GroupBox3: TGroupBox
        Left = 0
        Top = 7
        Width = 294
        Height = 210
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = ' '#1044#1086#1089#1090#1091#1087#1085#1099#1077' '#1090#1086#1095#1082#1080' '
        TabOrder = 1
        object StringGrid1: TStringGrid
          Left = 2
          Top = 15
          Width = 290
          Height = 193
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alClient
          ColCount = 2
          FixedCols = 0
          RowCount = 2
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goColMoving, goRowSelect]
          TabOrder = 0
          OnDblClick = StringGrid1DblClick
        end
      end
    end
    object Panel1: TPanel
      Left = 0
      Top = 225
      Width = 367
      Height = 324
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 1
      ExplicitLeft = 1
      object GroupBox2: TGroupBox
        Left = 13
        Top = 0
        Width = 281
        Height = 313
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = ' '#1055#1072#1088#1072#1084#1077#1090#1088#1099' '#1086#1082#1085#1072' '
        TabOrder = 0
        DesignSize = (
          281
          313)
        object Label7: TLabel
          Left = 9
          Top = 194
          Width = 46
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = #1051#1077#1075#1077#1085#1076#1072':'
        end
        object Label8: TLabel
          Left = 57
          Top = 251
          Width = 133
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = 'Watch interval (sec, -1 - inf):'
        end
        object Label9: TLabel
          Left = 93
          Top = 285
          Width = 97
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = 'Update interval (ms):'
        end
        object CheckBox2: TCheckBox
          Left = 9
          Top = 121
          Width = 128
          Height = 17
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #1055#1086#1076#1087#1080#1089#1100' '#1086#1089#1080' Y:'
          TabOrder = 0
          OnClick = CheckBox2Click
        end
        object Edit2: TEdit
          Left = 28
          Top = 138
          Width = 237
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Anchors = [akLeft, akTop, akRight]
          Color = clActiveBorder
          Enabled = False
          TabOrder = 1
          OnChange = Edit2Change
        end
        object CheckBox4: TCheckBox
          Left = 9
          Top = 97
          Width = 128
          Height = 16
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #1064#1082#1072#1083#1072' '#1087#1086' '#1086#1089#1080' Y'
          Checked = True
          State = cbChecked
          TabOrder = 2
          OnClick = CheckBox4Click
        end
        object Edit1: TEdit
          Left = 27
          Top = 68
          Width = 238
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Anchors = [akLeft, akTop, akRight]
          Color = clActiveBorder
          Enabled = False
          TabOrder = 3
          OnChange = Edit1Change
        end
        object CheckBox1: TCheckBox
          Left = 9
          Top = 48
          Width = 128
          Height = 17
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #1055#1086#1076#1087#1080#1089#1100' '#1086#1089#1080' X:'
          TabOrder = 4
          OnClick = CheckBox1Click
        end
        object CheckBox3: TCheckBox
          Left = 9
          Top = 25
          Width = 128
          Height = 16
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #1064#1082#1072#1083#1072' '#1087#1086' '#1086#1089#1080' X'
          Checked = True
          State = cbChecked
          TabOrder = 5
          OnClick = CheckBox3Click
        end
        object ComboBox3: TComboBox
          Left = 11
          Top = 215
          Width = 254
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          ItemIndex = 5
          TabOrder = 6
          Text = #1057#1085#1080#1079#1091
          OnSelect = ComboBox3Select
          Items.Strings = (
            #1057#1083#1077#1074#1072
            #1057#1083#1077#1074#1072' ('#1085#1072' '#1075#1088#1072#1092#1080#1082#1077')'
            #1057#1087#1088#1072#1074#1072
            #1057#1087#1088#1072#1074#1072' ('#1085#1072' '#1075#1088#1072#1092#1080#1082#1077')'
            #1057#1074#1077#1088#1093#1091
            #1057#1085#1080#1079#1091)
        end
        object WatchIntervalEdit: TEdit
          Left = 198
          Top = 247
          Width = 67
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 7
          Text = '5'
          OnChange = WatchIntervalEditChange
          OnExit = Edit4Exit
          OnKeyDown = Edit4KeyDown
        end
        object UpdateIntervalEdit: TEdit
          Left = 198
          Top = 281
          Width = 67
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 8
          Text = '100'
          OnChange = WatchIntervalEditChange
          OnExit = Edit4Exit
          OnKeyDown = Edit4KeyDown
        end
        object AutoMinYValueCheckBox: TCheckBox
          Left = 16
          Top = 168
          Width = 97
          Height = 17
          Caption = #1040#1074#1090#1086' min(Y)'
          TabOrder = 9
          OnClick = AutoMinYValueCheckBoxClick
        end
        object AutoMaxYValueCheckBox: TCheckBox
          Left = 136
          Top = 168
          Width = 97
          Height = 17
          Caption = #1040#1074#1090#1086' max(Y)'
          TabOrder = 10
          OnClick = AutoMaxYValueCheckBoxClick
        end
      end
    end
  end
  object Panel3: TPanel
    Left = 367
    Top = 0
    Width = 284
    Height = 549
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alRight
    BevelOuter = bvNone
    TabOrder = 1
    object Panel6: TPanel
      Left = 0
      Top = 225
      Width = 284
      Height = 324
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 0
      DesignSize = (
        284
        324)
      object Label1: TLabel
        Left = 9
        Top = 9
        Width = 3
        Height = 13
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
      end
      object BitBtn1: TBitBtn
        Left = 55
        Top = 285
        Width = 107
        Height = 32
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Anchors = [akRight, akBottom]
        Caption = #1055#1088#1080#1084#1077#1085#1080#1090#1100
        Glyph.Data = {
          DE010000424DDE01000000000000760000002800000024000000120000000100
          0400000000006801000000000000000000001000000000000000000000000000
          80000080000000808000800000008000800080800000C0C0C000808080000000
          FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
          3333333333333333333333330000333333333333333333333333F33333333333
          00003333344333333333333333388F3333333333000033334224333333333333
          338338F3333333330000333422224333333333333833338F3333333300003342
          222224333333333383333338F3333333000034222A22224333333338F338F333
          8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
          33333338F83338F338F33333000033A33333A222433333338333338F338F3333
          0000333333333A222433333333333338F338F33300003333333333A222433333
          333333338F338F33000033333333333A222433333333333338F338F300003333
          33333333A222433333333333338F338F00003333333333333A22433333333333
          3338F38F000033333333333333A223333333333333338F830000333333333333
          333A333333333333333338330000333333333333333333333333333333333333
          0000}
        ModalResult = 1
        NumGlyphs = 2
        TabOrder = 0
        OnClick = BitBtn1Click
      end
      object GroupBox1: TGroupBox
        Left = 0
        Top = 0
        Width = 281
        Height = 277
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = ' '#1055#1072#1088#1072#1084#1077#1090#1088#1099' '#1085#1072#1073#1083#1102#1076#1077#1085#1080#1103' '
        TabOrder = 1
        DesignSize = (
          281
          277)
        object Label2: TLabel
          Left = 32
          Top = 27
          Width = 50
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #1055#1086#1076#1087#1080#1089#1100': '
        end
        object Label3: TLabel
          Left = 54
          Top = 89
          Width = 28
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = #1062#1074#1077#1090':'
        end
        object Label4: TLabel
          Left = 25
          Top = 57
          Width = 57
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = #1057#1084#1077#1097#1077#1085#1080#1077':'
        end
        object Label5: TLabel
          Left = 27
          Top = 121
          Width = 55
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = #1058#1080#1087' '#1083#1080#1085#1080#1080':'
        end
        object Label6: TLabel
          Left = 33
          Top = 156
          Width = 49
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = #1058#1086#1083#1097#1080#1085#1072':'
        end
        object Edit3: TEdit
          Left = 89
          Top = 20
          Width = 184
          Height = 21
          Cursor = crHandPoint
          Hint = #1044#1074#1086#1081#1085#1086#1081' '#1097#1077#1083#1095#1086#1082', '#1095#1090#1086#1073#1099' '#1080#1079#1084#1077#1085#1080#1090#1100' '#1087#1086#1076#1087#1080#1089#1100
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Anchors = [akLeft, akTop, akRight]
          Color = clActiveBorder
          ParentShowHint = False
          ReadOnly = True
          ShowHint = True
          TabOrder = 0
          OnDblClick = Edit3DblClick
        end
        object Edit4: TEdit
          Left = 89
          Top = 54
          Width = 184
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 1
          Text = '0'
          OnExit = Edit4Exit
          OnKeyDown = Edit4KeyDown
        end
        object OutterCPanel: TPanel
          Left = 89
          Top = 84
          Width = 184
          Height = 24
          Cursor = crHandPoint
          Hint = #1044#1074#1086#1081#1085#1086#1081' '#1097#1077#1083#1095#1086#1082', '#1095#1090#1086#1073#1099' '#1080#1079#1084#1077#1085#1080#1090#1100' '#1094#1074#1077#1090
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Anchors = [akLeft, akTop, akRight]
          BevelInner = bvLowered
          BevelOuter = bvSpace
          Color = clWhite
          ParentShowHint = False
          ShowHint = True
          TabOrder = 2
          OnDblClick = OutterCPanelDblClick
          DesignSize = (
            184
            24)
          object InnerCPanel: TPanel
            Left = 9
            Top = 9
            Width = 168
            Height = 8
            Cursor = crHandPoint
            Hint = #1044#1074#1086#1081#1085#1086#1081' '#1097#1077#1083#1095#1086#1082', '#1095#1090#1086#1073#1099' '#1080#1079#1084#1077#1085#1080#1090#1100' '#1094#1074#1077#1090
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Anchors = [akLeft, akTop, akRight]
            BevelOuter = bvNone
            Color = clBlack
            ParentShowHint = False
            ShowHint = True
            TabOrder = 0
            OnDblClick = InnerCPanelDblClick
          end
        end
        object CheckBox5: TCheckBox
          Left = 92
          Top = 191
          Width = 185
          Height = 16
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Anchors = [akLeft, akTop, akRight]
          Caption = #1054#1090#1082#1083#1102#1095#1080#1090#1100' '#1074#1089#1077' '#1087#1086#1076#1087#1080#1089#1080
          Checked = True
          State = cbChecked
          TabOrder = 3
          OnClick = CheckBox5Click
        end
        object ComboBox1: TComboBox
          Left = 89
          Top = 117
          Width = 184
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          TabOrder = 4
          OnSelect = ComboBox1Select
          Items.Strings = (
            #1057#1087#1083#1086#1096#1085#1072#1103
            #1064#1090#1088#1080#1093#1080
            #1055#1091#1085#1082#1090#1080#1088
            #1064#1090#1088#1080#1093'-'#1087#1091#1085#1082#1090#1080#1088
            '2 '#1064#1090#1088#1080#1093'-'#1087#1091#1085#1082#1090#1080#1088)
        end
        object ComboBox2: TComboBox
          Left = 89
          Top = 153
          Width = 184
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 5
          Text = '1 '#1087#1080#1082#1089#1077#1083
          OnSelect = ComboBox2Select
          Items.Strings = (
            '1 '#1087#1080#1082#1089#1077#1083
            '2 '#1087#1080#1082#1089#1077#1083#1103
            '3 '#1087#1080#1082#1089#1077#1083#1103
            '4 '#1087#1080#1082#1089#1077#1083#1103
            '5 '#1087#1080#1082#1089#1077#1083#1077#1081
            '6 '#1087#1080#1082#1089#1077#1083#1077#1081
            '7 '#1087#1080#1082#1089#1077#1083#1077#1081
            '8 '#1087#1080#1082#1089#1077#1083#1077#1081
            '9 '#1087#1080#1082#1089#1077#1083#1077#1081
            '10 '#1087#1080#1082#1089#1077#1083#1077#1081)
        end
      end
      object BitBtn2: TBitBtn
        Left = 170
        Top = 285
        Width = 104
        Height = 32
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Anchors = [akLeft, akBottom]
        Caption = #1054#1090#1084#1077#1085#1072
        Kind = bkCancel
        NumGlyphs = 2
        TabOrder = 2
        OnClick = BitBtn2Click
      end
    end
    object GroupBox4: TGroupBox
      Left = 0
      Top = 7
      Width = 281
      Height = 210
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = ' '#1053#1072#1073#1083#1102#1076#1072#1077#1084#1099#1077' '#1090#1086#1095#1082#1080' '
      TabOrder = 1
      object StringGrid2: TStringGrid
        Left = 2
        Top = 15
        Width = 277
        Height = 193
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alClient
        ColCount = 2
        FixedCols = 0
        RowCount = 2
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goColSizing, goColMoving, goRowSelect]
        TabOrder = 0
        OnClick = StringGrid2Click
        OnDblClick = StringGrid2DblClick
      end
    end
  end
  object ColorDialog1: TColorDialog
    Options = [cdFullOpen]
    Left = 254
    Top = 177
  end
end
