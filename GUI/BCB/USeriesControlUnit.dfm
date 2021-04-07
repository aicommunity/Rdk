object USeriesControlForm: TUSeriesControlForm
  Left = 360
  Top = 248
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Watch series control'
  ClientHeight = 549
  ClientWidth = 654
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
    Width = 370
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
      Width = 370
      Height = 225
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 0
      object Panel5: TPanel
        Left = 298
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
        Left = -4
        Top = 7
        Width = 294
        Height = 210
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = ' Accessible series '
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
      Width = 370
      Height = 324
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alBottom
      BevelOuter = bvNone
      TabOrder = 1
      object GroupBox2: TGroupBox
        Left = 13
        Top = 0
        Width = 281
        Height = 313
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = ' Watch parameters '
        TabOrder = 0
        DesignSize = (
          281
          313)
        object Label7: TLabel
          Left = 11
          Top = 194
          Width = 78
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = 'Legend position:'
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
          Caption = 'Y-axis legend:'
          TabOrder = 0
          WordWrap = True
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
          Caption = 'Show Y-axis'
          Checked = True
          State = cbChecked
          TabOrder = 2
          WordWrap = True
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
          Caption = 'X-axis legend:'
          TabOrder = 4
          WordWrap = True
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
          Caption = 'Show X-axis '
          Checked = True
          State = cbChecked
          TabOrder = 5
          WordWrap = True
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
          Text = 'Bottom'
          OnSelect = ComboBox3Select
          Items.Strings = (
            'Left'
            'Left (on graph)'
            'Right'
            'Right (on graph)'
            'Top'
            'Bottom')
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
          Left = 11
          Top = 166
          Width = 97
          Height = 17
          Caption = 'Auto min (Y)'
          TabOrder = 9
          WordWrap = True
          OnClick = AutoMinYValueCheckBoxClick
        end
        object AutoMaxYValueCheckBox: TCheckBox
          Left = 136
          Top = 166
          Width = 97
          Height = 17
          Caption = 'Auto max (Y)'
          TabOrder = 10
          WordWrap = True
          OnClick = AutoMaxYValueCheckBoxClick
        end
      end
    end
  end
  object Panel3: TPanel
    Left = 370
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
        Caption = 'Apply'
        Default = True
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
        Caption = ' Series parameters  '
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -10
        Font.Name = 'MS Sans Serif'
        Font.Style = []
        ParentFont = False
        TabOrder = 1
        DesignSize = (
          281
          277)
        object Label2: TLabel
          Left = 39
          Top = 23
          Width = 42
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = 'Legend: '
        end
        object Label3: TLabel
          Left = 55
          Top = 91
          Width = 27
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = 'Color:'
        end
        object Label4: TLabel
          Left = 50
          Top = 57
          Width = 32
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = 'Y-shift:'
        end
        object Label5: TLabel
          Left = 35
          Top = 119
          Width = 47
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = 'Line style:'
        end
        object Label6: TLabel
          Left = 31
          Top = 156
          Width = 51
          Height = 13
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Alignment = taRightJustify
          Caption = 'Line width:'
        end
        object Edit3: TEdit
          Left = 89
          Top = 20
          Width = 184
          Height = 21
          Cursor = crHandPoint
          Hint = 'Double clock for change series legend'
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
          Left = 90
          Top = 84
          Width = 184
          Height = 24
          Cursor = crHandPoint
          Hint = 'Double clock for change series color'
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
            Left = 8
            Top = 7
            Width = 169
            Height = 12
            Cursor = crHandPoint
            Hint = 'Double clock for change series color'
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Anchors = [akLeft, akTop, akRight]
            BevelOuter = bvNone
            Color = clBlack
            DoubleBuffered = False
            ParentBackground = False
            ParentDoubleBuffered = False
            ParentShowHint = False
            ShowHint = True
            TabOrder = 0
            OnDblClick = InnerCPanelDblClick
          end
        end
        object CheckBox5: TCheckBox
          Left = 92
          Top = 182
          Width = 185
          Height = 16
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Anchors = [akLeft, akTop, akRight]
          Caption = 'Disable all legends'
          Checked = True
          State = cbChecked
          TabOrder = 3
          WordWrap = True
          OnClick = CheckBox5Click
        end
        object ComboBox1: TComboBox
          AlignWithMargins = True
          Left = 90
          Top = 116
          Width = 184
          Height = 21
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 4
          Text = 'Solid'
          OnSelect = ComboBox1Select
          Items.Strings = (
            'Solid'
            'Dash'
            'Dot'
            'Dash dot'
            'Dash dot 2')
        end
        object ComboBox2: TComboBox
          Left = 90
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
          Text = '1 pixel'
          OnSelect = ComboBox2Select
          Items.Strings = (
            '1 pixel'
            '2 pixels'
            '3 pixels'
            '4 pixels'
            '5 pixels'
            '6 pixels'
            '7 pixels'
            '8 pixels'
            '9 pixels'
            '10 pixels')
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
        Kind = bkCancel
        NumGlyphs = 2
        TabOrder = 2
        OnClick = BitBtn2Click
      end
    end
    object GroupBox4: TGroupBox
      Left = -1
      Top = 7
      Width = 281
      Height = 210
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = ' Visible series '
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
