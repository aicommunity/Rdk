object EnchancedSG: TEnchancedSG
  Left = 0
  Top = 0
  Width = 311
  Height = 383
  TabOrder = 0
  TabStop = True
  OnResize = FrameResize
  object BasicStringGrid: TStringGrid
    Left = 16
    Top = 56
    Width = 161
    Height = 97
    ColCount = 1
    DefaultRowHeight = 18
    FixedCols = 0
    RowCount = 1
    FixedRows = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing]
    ParentShowHint = False
    PopupMenu = PopupMenu1
    ShowHint = True
    TabOrder = 1
    OnDblClick = BasicStringGridDblClick
    OnDrawCell = BasicStringGridDrawCell
    OnKeyPress = BasicStringGridKeyPress
    OnMouseMove = BasicStringGridMouseMove
    OnSelectCell = BasicStringGridSelectCell
  end
  object txtStringEdit: TEdit
    Left = 16
    Top = 16
    Width = 121
    Height = 24
    BorderStyle = bsNone
    TabOrder = 0
    Text = '0'
    Visible = False
    OnKeyPress = txtStringEditKeyPress
  end
  object chbBoolEdit: TCheckBox
    Left = 195
    Top = 94
    Width = 97
    Height = 17
    Color = clBtnFace
    ParentColor = False
    TabOrder = 3
    Visible = False
    OnClick = chbBoolEditClick
    OnKeyPress = chbBoolEditKeyPress
  end
  object udBorderedInt: TUpDown
    Left = 344
    Top = 132
    Width = 17
    Height = 21
    TabOrder = 4
    Visible = False
  end
  object txtDoubleEdit: TEdit
    Left = 246
    Top = 16
    Width = 121
    Height = 24
    BorderStyle = bsNone
    TabOrder = 2
    Text = 'txtDoubleEdit'
    Visible = False
    OnKeyPress = txtDoubleEditKeyPress
  end
  object txtIntEdit: TEdit
    Left = 224
    Top = 43
    Width = 121
    Height = 24
    BorderStyle = bsNone
    TabOrder = 5
    Text = 'txtIntEdit'
    Visible = False
    OnKeyPress = txtIntEditKeyPress
  end
  object txtBorderedIntEdit: TEdit
    Left = 224
    Top = 132
    Width = 121
    Height = 24
    BorderStyle = bsNone
    ReadOnly = True
    TabOrder = 6
    Text = 'txtBorderedIntEdit'
    Visible = False
    OnKeyPress = txtBorderedIntEditKeyPress
  end
  object cmbListEdit: TComboBox
    Left = 216
    Top = 159
    Width = 145
    Height = 22
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 7
    Visible = False
    OnKeyPress = cmbListEditKeyPress
  end
  object PopupMenu1: TPopupMenu
    Left = 72
    Top = 232
    object CopynametoClipboard1: TMenuItem
      Caption = 'Copy name to Clipboard'
      OnClick = CopynametoClipboard1Click
    end
    object CopyvaluetoClipboard1: TMenuItem
      Caption = 'Copy value to Clipboard'
      OnClick = CopyvaluetoClipboard1Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object PastevaluefromClipboard1: TMenuItem
      Caption = 'Paste value from Clipboard'
      OnClick = PastevaluefromClipboard1Click
    end
  end
end
