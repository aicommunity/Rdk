inherited UClassesListFrame: TUClassesListFrame
  Width = 507
  Height = 543
  DoubleBuffered = True
  ParentDoubleBuffered = False
  ExplicitWidth = 507
  ExplicitHeight = 543
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 507
    Height = 543
    ActivePage = LibsTabSheet
    Align = alClient
    TabOrder = 0
    OnChange = PageControlChange
    object NameTabSheet: TTabSheet
      Caption = 'By Name'
      object StringGrid: TStringGrid
        Left = 0
        Top = 0
        Width = 499
        Height = 515
        Align = alClient
        ColCount = 2
        DefaultRowHeight = 18
        FixedCols = 0
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
        ParentShowHint = False
        ShowHint = True
        TabOrder = 0
        OnMouseEnter = StringGridMouseEnter
        OnMouseMove = StringGridMouseMove
      end
    end
    object LibsTabSheet: TTabSheet
      Caption = 'By Libs'
      ImageIndex = 1
      object TreeView: TTreeView
        Left = 0
        Top = 0
        Width = 499
        Height = 515
        Align = alClient
        Indent = 19
        ParentShowHint = False
        ReadOnly = True
        RightClickSelect = True
        RowSelect = True
        ShowHint = True
        ShowLines = False
        TabOrder = 0
        OnMouseEnter = TreeViewMouseEnter
        OnMouseMove = TreeViewMouseMove
      end
    end
    object LibsControlTabSheet: TTabSheet
      Caption = 'Libs Control'
      ImageIndex = 2
      object Splitter1: TSplitter
        Left = 0
        Top = 225
        Width = 499
        Height = 7
        Cursor = crVSplit
        Align = alBottom
        ExplicitTop = 304
      end
      object Panel1: TPanel
        Left = 0
        Top = 392
        Width = 499
        Height = 123
        Align = alBottom
        TabOrder = 0
        object LoadLibraryButton: TButton
          Left = 1
          Top = 1
          Width = 497
          Height = 25
          Align = alTop
          Caption = 'Load Library'
          TabOrder = 0
        end
        object CreateRuntimeLibraryButton: TButton
          Left = 1
          Top = 26
          Width = 497
          Height = 25
          Align = alTop
          Caption = 'Create Runtime Library'
          TabOrder = 1
          OnClick = CreateRuntimeLibraryButtonClick
        end
        object DeleteLibraryButton: TButton
          Left = 1
          Top = 51
          Width = 497
          Height = 25
          Align = alTop
          Caption = 'Delete Library'
          TabOrder = 2
        end
        object RenameRuntimeLibraryButton: TButton
          Left = 1
          Top = 76
          Width = 497
          Height = 25
          Align = alTop
          Caption = 'Rename Runtime Library'
          TabOrder = 3
        end
        object AddClassButton: TButton
          Left = 1
          Top = 101
          Width = 497
          Height = 25
          Align = alTop
          Caption = 'Create New Class'
          TabOrder = 4
          OnClick = AddClassButtonClick
        end
      end
      object GroupBox1: TGroupBox
        Left = 0
        Top = 0
        Width = 499
        Height = 225
        Align = alClient
        Caption = ' Library list '
        TabOrder = 1
        object LibsListStringGrid: TStringGrid
          Left = 2
          Top = 15
          Width = 495
          Height = 208
          Align = alClient
          ColCount = 2
          DefaultRowHeight = 18
          FixedCols = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
          TabOrder = 0
          OnMouseEnter = LibsListStringGridMouseEnter
          OnSelectCell = LibsListStringGridSelectCell
        end
      end
      object GroupBox2: TGroupBox
        Left = 0
        Top = 232
        Width = 499
        Height = 160
        Align = alBottom
        Caption = ' Selected Library Components'
        TabOrder = 2
        object LibComponentListStringGrid: TStringGrid
          Left = 2
          Top = 15
          Width = 495
          Height = 143
          Align = alClient
          ColCount = 2
          DefaultRowHeight = 18
          FixedCols = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
          OnMouseEnter = LibComponentListStringGridMouseEnter
          OnMouseMove = LibComponentListStringGridMouseMove
        end
      end
    end
  end
end
