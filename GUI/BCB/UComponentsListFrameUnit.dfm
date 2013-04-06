inherited UComponentsListFrame: TUComponentsListFrame
  Width = 755
  Height = 567
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
      ActivePage = TabSheet4
      Align = alClient
      TabOrder = 0
      TabPosition = tpBottom
      OnChange = PageControl1Change
      object TabSheet4: TTabSheet
        Caption = 'Parameters List'
        ImageIndex = 3
        object Splitter3: TSplitter
          Left = 0
          Top = 462
          Width = 376
          Height = 10
          Cursor = crVSplit
          Align = alBottom
          ExplicitTop = 440
        end
        object ParametersListStringGrid: TStringGrid
          Left = 0
          Top = 0
          Width = 376
          Height = 462
          Align = alClient
          DefaultRowHeight = 18
          Options = [goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goEditing, goRowSelect]
          TabOrder = 0
          OnClick = ParametersListStringGridClick
          OnDblClick = ParametersListStringGridDblClick
          OnSelectCell = ParametersListStringGridSelectCell
        end
        object ParameterValueRichEdit: TRichEdit
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
          ParentFont = False
          PlainText = True
          ScrollBars = ssBoth
          TabOrder = 1
        end
        object HeaderControl3: THeaderControl
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
          OnSectionClick = HeaderControl3SectionClick
          ShowHint = False
          ParentShowHint = False
        end
      end
      object TabSheet5: TTabSheet
        Caption = 'States List'
        ImageIndex = 4
        object Splitter4: TSplitter
          Left = 0
          Top = 461
          Width = 376
          Height = 11
          Cursor = crVSplit
          Align = alBottom
          ExplicitTop = 457
        end
        object StatesListStringGrid: TStringGrid
          Left = 0
          Top = 0
          Width = 376
          Height = 461
          Align = alClient
          DefaultRowHeight = 18
          Options = [goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
          TabOrder = 0
          OnClick = StatesListStringGridClick
          OnDblClick = StatesListStringGridDblClick
          OnSelectCell = StatesListStringGridSelectCell
        end
        object HeaderControl1: THeaderControl
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
          OnSectionClick = HeaderControl1SectionClick
          ShowHint = False
          ParentShowHint = False
        end
        object StateValueRichEdit: TRichEdit
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
          ParentFont = False
          PlainText = True
          ScrollBars = ssBoth
          TabOrder = 2
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
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
            TabOrder = 0
            OnClick = OutputsStringGridClick
            OnDblClick = OutputsStringGridDblClick
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
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
            TabOrder = 0
            OnClick = InputsStringGridClick
            OnDblClick = InputsStringGridDblClick
            OnSelectCell = InputsStringGridSelectCell
          end
        end
      end
      object TabSheet1: TTabSheet
        Caption = 'Parameters'
        object ParametersHeaderControl: THeaderControl
          Left = 0
          Top = 0
          Width = 376
          Height = 34
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
          OnSectionClick = ParametersHeaderControlSectionClick
          ShowHint = False
          ParentShowHint = False
        end
        object ParametersRichEdit: TRichEdit
          Left = 0
          Top = 34
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
          OnChange = ParametersRichEditChange
        end
      end
      object TabSheet2: TTabSheet
        Caption = 'State'
        ImageIndex = 1
        object StateHeaderControl: THeaderControl
          Left = 0
          Top = 0
          Width = 376
          Height = 34
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
          Top = 34
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
      OnKeyPress = StringGridKeyPress
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
      Align = alBottom
      Alignment = taLeftJustify
      TabOrder = 2
    end
  end
  object PopupMenu: TPopupMenu
    Left = 288
    Top = 288
    object GUI1: TMenuItem
      Caption = 'GUI'
      OnClick = GUI1Click
    end
  end
end
