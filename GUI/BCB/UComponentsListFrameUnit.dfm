inherited UComponentsListFrame: TUComponentsListFrame
  Width = 755
  Height = 567
  DoubleBuffered = True
  ParentDoubleBuffered = False
  OnMouseWheel = FrameMouseWheel
  OnResize = FrameResize
  ExplicitWidth = 755
  ExplicitHeight = 567
  object Splitter1: TSplitter
    Left = 361
    Top = 0
    Width = 8
    Height = 567
  end
  object Panel1: TPanel
    Left = 369
    Top = 0
    Width = 386
    Height = 567
    Align = alClient
    TabOrder = 0
    object PageControl1: TPageControl
      Left = 1
      Top = 1
      Width = 384
      Height = 565
      ActivePage = TabSheet6
      Align = alClient
      TabOrder = 0
      TabPosition = tpBottom
      OnChange = PageControl1Change
      object TabSheet6: TTabSheet
        Caption = 'Parameters List'
        ImageIndex = 5
        object Splitter5: TSplitter
          Left = 0
          Top = 462
          Width = 376
          Height = 10
          Cursor = crVSplit
          Align = alBottom
        end
        inline EnchancedSG1: TEnchancedSG
          Left = 0
          Top = 0
          Width = 376
          Height = 462
          Align = alClient
          TabOrder = 0
          TabStop = True
          ExplicitWidth = 376
          ExplicitHeight = 462
          inherited BasicStringGrid: TStringGrid
            ColCount = 2
            FixedCols = 1
            RowCount = 2
            FixedRows = 1
            OnClick = EnchancedSG1BasicStringGridClick
            OnDblClick = EnchancedSG1BasicStringGridDblClick
            OnDrawCell = EnchancedSG1BasicStringGridDrawCell
            OnKeyPress = EnchancedSG1BasicStringGridKeyPress
            OnMouseEnter = EnchancedSG1BasicStringGridMouseEnter
            OnSelectCell = EnchancedSG1BasicStringGridSelectCell
          end
          inherited txtStringEdit: TEdit
            Left = 32
            Top = 248
            Height = 21
            OnKeyPress = EnchancedSG1txtStringEditKeyPress
            ExplicitLeft = 32
            ExplicitTop = 248
            ExplicitHeight = 21
          end
          inherited chbBoolEdit: TCheckBox
            Left = 35
            Top = 366
            ExplicitLeft = 35
            ExplicitTop = 366
          end
          inherited udBorderedInt: TUpDown
            Left = 152
            Top = 324
            ExplicitLeft = 152
            ExplicitTop = 324
          end
          inherited txtDoubleEdit: TEdit
            Left = 30
            Top = 208
            Height = 21
            OnKeyPress = EnchancedSG1txtDoubleEditKeyPress
            ExplicitLeft = 30
            ExplicitTop = 208
            ExplicitHeight = 21
          end
          inherited txtIntEdit: TEdit
            Left = 32
            Top = 389
            Height = 21
            OnKeyPress = EnchancedSG1txtIntEditKeyPress
            ExplicitLeft = 32
            ExplicitTop = 389
            ExplicitHeight = 21
          end
          inherited txtBorderedIntEdit: TEdit
            Left = 32
            Top = 324
            Height = 21
            OnKeyPress = EnchancedSG1txtBorderedIntEditKeyPress
            ExplicitLeft = 32
            ExplicitTop = 324
            ExplicitHeight = 21
          end
          inherited cmbListEdit: TComboBox
            Left = 32
            Top = 287
            OnKeyPress = EnchancedSG1cmbListEditKeyPress
            ExplicitLeft = 32
            ExplicitTop = 287
          end
          inherited PopupMenu1: TPopupMenu
            inherited PastevaluefromClipboard1: TMenuItem
              OnClick = EnchancedSG1PastevaluefromClipboard1Click
            end
          end
        end
        object HeaderControl2: THeaderControl
          Left = 0
          Top = 505
          Width = 376
          Height = 34
          Align = alBottom
          HotTrack = True
          Sections = <
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Set'
              Width = 50
            end
            item
              ImageIndex = -1
              Text = 'Set Global'
              Width = 60
            end
            item
              ImageIndex = -1
              Text = 'Set Global Owner'
              Width = 100
            end
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Reload'
              Width = 50
            end
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Default'
              Width = 50
            end
            item
              ImageIndex = -1
              Text = 'Default All'
              Width = 65
            end>
          OnSectionClick = HeaderControl2SectionClick
          ShowHint = False
          ParentShowHint = False
        end
        object NiceParamValRichEdit: TRichEdit
          Left = 0
          Top = 472
          Width = 376
          Height = 33
          Align = alBottom
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          Constraints.MinHeight = 25
          ParentFont = False
          PlainText = True
          ScrollBars = ssBoth
          TabOrder = 2
          Zoom = 100
          OnChange = NiceParamValRichEditChange
          OnMouseEnter = NiceParamValRichEditMouseEnter
        end
      end
      object TabSheet7: TTabSheet
        Caption = 'States List'
        ImageIndex = 6
        object Splitter6: TSplitter
          Left = 0
          Top = 462
          Width = 376
          Height = 10
          Cursor = crVSplit
          Align = alBottom
          ExplicitTop = 456
        end
        object HeaderControl4: THeaderControl
          Left = 0
          Top = 505
          Width = 376
          Height = 34
          Align = alBottom
          HotTrack = True
          Sections = <
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Set'
              Width = 50
            end
            item
              ImageIndex = -1
              Text = 'Set Global'
              Width = 60
            end
            item
              ImageIndex = -1
              Text = 'Set Global Owner'
              Width = 100
            end
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Reload'
              Width = 50
            end
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Default'
              Width = 50
            end>
          OnSectionClick = HeaderControl4SectionClick
          ShowHint = False
          ParentShowHint = False
        end
        object NiceStateValRichEdit: TRichEdit
          Left = 0
          Top = 472
          Width = 376
          Height = 33
          Align = alBottom
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          Constraints.MinHeight = 25
          ParentFont = False
          PlainText = True
          ScrollBars = ssBoth
          TabOrder = 1
          Zoom = 100
          OnChange = NiceStateValRichEditChange
          OnMouseEnter = NiceStateValRichEditMouseEnter
        end
        inline EnchancedSG2: TEnchancedSG
          Left = 0
          Top = 0
          Width = 376
          Height = 462
          Align = alClient
          TabOrder = 2
          TabStop = True
          OnMouseEnter = EnchancedSG2MouseEnter
          ExplicitWidth = 376
          ExplicitHeight = 462
          inherited BasicStringGrid: TStringGrid
            ColCount = 2
            FixedCols = 1
            RowCount = 2
            FixedRows = 1
            OnClick = EnchancedSG2BasicStringGridClick
            OnDblClick = EnchancedSG2BasicStringGridDblClick
            OnDrawCell = EnchancedSG2BasicStringGridDrawCell
            OnKeyPress = EnchancedSG2BasicStringGridKeyPress
            OnMouseEnter = EnchancedSG2BasicStringGridMouseEnter
            OnSelectCell = EnchancedSG2BasicStringGridSelectCell
            RowHeights = (
              18
              18)
          end
          inherited txtStringEdit: TEdit
            Left = 32
            Top = 248
            Height = 21
            OnKeyPress = EnchancedSG2txtStringEditKeyPress
            ExplicitLeft = 32
            ExplicitTop = 248
            ExplicitHeight = 21
          end
          inherited chbBoolEdit: TCheckBox
            Left = 35
            Top = 366
            ExplicitLeft = 35
            ExplicitTop = 366
          end
          inherited udBorderedInt: TUpDown
            Left = 152
            Top = 324
            ExplicitLeft = 152
            ExplicitTop = 324
          end
          inherited txtDoubleEdit: TEdit
            Left = 30
            Top = 208
            Height = 21
            OnKeyPress = EnchancedSG2txtDoubleEditKeyPress
            ExplicitLeft = 30
            ExplicitTop = 208
            ExplicitHeight = 21
          end
          inherited txtIntEdit: TEdit
            Left = 32
            Top = 389
            Height = 21
            OnKeyPress = EnchancedSG2txtIntEditKeyPress
            ExplicitLeft = 32
            ExplicitTop = 389
            ExplicitHeight = 21
          end
          inherited txtBorderedIntEdit: TEdit
            Left = 32
            Top = 324
            Height = 21
            OnKeyPress = EnchancedSG2txtBorderedIntEditKeyPress
            ExplicitLeft = 32
            ExplicitTop = 324
            ExplicitHeight = 21
          end
          inherited cmbListEdit: TComboBox
            Left = 32
            Top = 287
            OnKeyPress = EnchancedSG2cmbListEditKeyPress
            ExplicitLeft = 32
            ExplicitTop = 287
          end
          inherited PopupMenu1: TPopupMenu
            inherited PastevaluefromClipboard1: TMenuItem
              OnClick = EnchancedSG2PastevaluefromClipboard1Click
            end
          end
        end
      end
      object TabSheet3: TTabSheet
        Caption = 'I/O'
        ImageIndex = 2
        object Splitter2: TSplitter
          Left = 0
          Top = 239
          Width = 376
          Height = 8
          Cursor = crVSplit
          Align = alTop
          ExplicitTop = 241
        end
        object GroupBox: TGroupBox
          Left = 0
          Top = 0
          Width = 376
          Height = 239
          Align = alTop
          Caption = ' Outputs '
          TabOrder = 0
          object OutputsStringGrid: TStringGrid
            Left = 2
            Top = 15
            Width = 372
            Height = 222
            Align = alClient
            DefaultRowHeight = 18
            FixedCols = 0
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goColSizing, goRowSelect]
            TabOrder = 0
            OnClick = OutputsStringGridClick
            OnDblClick = OutputsStringGridDblClick
            OnMouseEnter = OutputsStringGridMouseEnter
            OnSelectCell = OutputsStringGridSelectCell
          end
        end
        object GroupBox2: TGroupBox
          Left = 0
          Top = 247
          Width = 376
          Height = 292
          Align = alClient
          Caption = ' Inputs '
          TabOrder = 1
          object InputsStringGrid: TStringGrid
            Left = 2
            Top = 15
            Width = 372
            Height = 275
            Align = alClient
            DefaultRowHeight = 18
            FixedCols = 0
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goColSizing, goRowSelect]
            TabOrder = 0
            OnClick = InputsStringGridClick
            OnDblClick = InputsStringGridDblClick
            OnMouseEnter = InputsStringGridMouseEnter
            OnSelectCell = InputsStringGridSelectCell
          end
        end
      end
      object TabSheet1: TTabSheet
        Caption = 'Parameters'
        object ParametersHeaderControl: THeaderControl
          Left = 0
          Top = 505
          Width = 376
          Height = 34
          Align = alBottom
          HotTrack = True
          Sections = <
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Set'
              Width = 50
            end
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Reload'
              Width = 50
            end
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Default'
              Width = 50
            end
            item
              ImageIndex = -1
              Text = 'Default All'
              Width = 65
            end>
          OnSectionClick = ParametersHeaderControlSectionClick
          ShowHint = False
          ParentShowHint = False
        end
        object ParametersRichEdit: TRichEdit
          Left = 0
          Top = 0
          Width = 376
          Height = 505
          Align = alClient
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          ScrollBars = ssBoth
          TabOrder = 1
          Zoom = 100
          OnChange = ParametersRichEditChange
        end
      end
      object TabSheet2: TTabSheet
        Caption = 'State'
        ImageIndex = 1
        object StateHeaderControl: THeaderControl
          Left = 0
          Top = 505
          Width = 376
          Height = 34
          Align = alBottom
          HotTrack = True
          Sections = <
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Set'
              Width = 50
            end
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Reload'
              Width = 50
            end
            item
              Alignment = taCenter
              ImageIndex = -1
              Text = 'Default'
              Width = 50
            end>
          OnSectionClick = StateHeaderControlSectionClick
          ShowHint = False
          ParentShowHint = False
        end
        object StateRichEdit: TRichEdit
          Left = 0
          Top = 0
          Width = 376
          Height = 505
          Align = alClient
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          ScrollBars = ssBoth
          TabOrder = 1
          Zoom = 100
          OnChange = StateRichEditChange
        end
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 361
    Height = 567
    Align = alLeft
    TabOrder = 1
    object StringGrid: TStringGrid
      Left = 1
      Top = 35
      Width = 359
      Height = 509
      Align = alClient
      FixedCols = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
      PopupMenu = PopupMenu
      TabOrder = 0
      OnClick = StringGridClick
      OnDblClick = StringGridDblClick
      OnKeyDown = StringGridKeyDown
      OnKeyPress = StringGridKeyPress
      OnMouseEnter = StringGridMouseEnter
      OnSelectCell = StringGridSelectCell
    end
    object HeaderControl: THeaderControl
      Left = 1
      Top = 1
      Width = 359
      Height = 34
      HotTrack = True
      Sections = <>
      Style = hsFlat
      NoSizing = True
      OverFlow = True
      ShowHint = False
      ParentShowHint = False
    end
    object ClassNamePanel: TPanel
      Left = 1
      Top = 544
      Width = 359
      Height = 22
      Hint = 'Component class name'
      Align = alBottom
      Alignment = taLeftJustify
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
    end
  end
  object PopupMenu: TPopupMenu
    Left = 288
    Top = 288
    object Moveup1: TMenuItem
      Caption = 'Move up'
      Hint = 'Changing component calculation order'
      OnClick = Moveup1Click
    end
    object Movedown1: TMenuItem
      Caption = 'Move down'
      Hint = 'Changing component calculation order'
      OnClick = Movedown1Click
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object Rename1: TMenuItem
      Caption = 'Rename'
      OnClick = Rename1Click
    end
    object Delete1: TMenuItem
      Caption = 'Delete'
      Hint = 'Press '#39'Shift'#39' to delete without prompt'
      OnClick = Delete1Click
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object Reset1: TMenuItem
      Caption = 'Reset'
      Hint = 'Calls component Reset'
      OnClick = Reset1Click
    end
    object Calculate1: TMenuItem
      Caption = 'Calculate'
      Hint = 'Calls component Calculate'
      OnClick = Calculate1Click
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object Init1: TMenuItem
      Caption = 'Init'
      OnClick = Init1Click
    end
    object UnInit1: TMenuItem
      Caption = 'UnInit'
      OnClick = UnInit1Click
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object CopynametoClipboard1: TMenuItem
      Caption = 'Copy name to Clipboard'
      OnClick = CopynametoClipboard1Click
    end
    object CopylongnametoClipboard1: TMenuItem
      Caption = 'Copy long name to Clipboard'
      OnClick = CopylongnametoClipboard1Click
    end
    object CopyclasstoClipboard2: TMenuItem
      Caption = 'Copy class to Clipboard'
      OnClick = CopyclasstoClipboard2Click
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object GUI1: TMenuItem
      Caption = 'GUI'
      Hint = 'Open component GUI if possible'
      OnClick = GUI1Click
    end
  end
end
