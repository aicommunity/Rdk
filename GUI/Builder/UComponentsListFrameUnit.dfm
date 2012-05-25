object UComponentsListFrame: TUComponentsListFrame
  Left = 0
  Top = 0
  Width = 755
  Height = 567
  TabOrder = 0
  OnResize = FrameResize
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
      ActivePage = TabSheet1
      Align = alClient
      TabOrder = 0
      TabPosition = tpBottom
      OnChange = PageControl1Change
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
      object TabSheet3: TTabSheet
        Caption = 'I/O'
        ImageIndex = 2
        object Splitter2: TSplitter
          Left = 0
          Top = 273
          Width = 376
          Height = 8
          Cursor = crVSplit
          Align = alTop
          ExplicitTop = 241
        end
        object IOHeaderControl: THeaderControl
          Left = 0
          Top = 0
          Width = 376
          Height = 34
          HotTrack = True
          Sections = <>
          ShowHint = False
          ParentShowHint = False
        end
        object GroupBox: TGroupBox
          Left = 0
          Top = 34
          Width = 376
          Height = 239
          Align = alTop
          Caption = ' Outputs '
          TabOrder = 1
          object OutputsStringGrid: TStringGrid
            Left = 2
            Top = 15
            Width = 372
            Height = 222
            Align = alClient
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
            TabOrder = 0
            OnClick = OutputsStringGridClick
            OnDblClick = OutputsStringGridDblClick
            OnSelectCell = OutputsStringGridSelectCell
          end
        end
        object GroupBox2: TGroupBox
          Left = 0
          Top = 281
          Width = 376
          Height = 258
          Align = alClient
          Caption = ' Inputs '
          TabOrder = 2
          object InputsStringGrid: TStringGrid
            Left = 2
            Top = 15
            Width = 372
            Height = 241
            Align = alClient
            Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
            TabOrder = 0
            OnClick = InputsStringGridClick
            OnDblClick = InputsStringGridDblClick
            OnSelectCell = InputsStringGridSelectCell
          end
        end
      end
      object TabSheet4: TTabSheet
        Caption = 'Parameters List'
        ImageIndex = 3
        object HeaderControl1: THeaderControl
          Left = 0
          Top = 0
          Width = 376
          Height = 30
          Sections = <>
          DoubleBuffered = True
          ParentDoubleBuffered = False
        end
        object ParametersListStringGrid: TStringGrid
          Left = 0
          Top = 30
          Width = 376
          Height = 509
          Align = alClient
          Options = [goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
          TabOrder = 1
          OnClick = ParametersListStringGridClick
          OnDblClick = ParametersListStringGridDblClick
          OnSelectCell = ParametersListStringGridSelectCell
          ExplicitLeft = 2
          ExplicitTop = 15
          ExplicitWidth = 372
          ExplicitHeight = 222
        end
      end
      object TabSheet5: TTabSheet
        Caption = 'States List'
        ImageIndex = 4
        object HeaderControl2: THeaderControl
          Left = 0
          Top = 0
          Width = 376
          Height = 30
          Sections = <>
          DoubleBuffered = True
          ParentDoubleBuffered = False
        end
        object StatesListStringGrid: TStringGrid
          Left = 0
          Top = 30
          Width = 376
          Height = 509
          Align = alClient
          Options = [goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSelect]
          TabOrder = 1
          OnClick = StatesListStringGridClick
          OnDblClick = StatesListStringGridDblClick
          OnSelectCell = StatesListStringGridSelectCell
          ExplicitLeft = 2
          ExplicitTop = 15
          ExplicitWidth = 372
          ExplicitHeight = 222
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
      Height = 531
      Align = alClient
      FixedCols = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
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
  end
end
