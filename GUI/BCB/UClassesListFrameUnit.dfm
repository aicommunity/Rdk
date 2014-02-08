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
    ActivePage = NameTabSheet
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
        TabOrder = 0
        OnMouseEnter = StringGridMouseEnter
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
        ReadOnly = True
        RightClickSelect = True
        RowSelect = True
        ShowLines = False
        TabOrder = 0
        OnMouseEnter = TreeViewMouseEnter
      end
    end
    object LibsControlTabSheet: TTabSheet
      Caption = 'Libs Control'
      ImageIndex = 2
      object Splitter1: TSplitter
        Left = 0
        Top = 304
        Width = 499
        Height = 7
        Cursor = crVSplit
        Align = alBottom
      end
      object Panel1: TPanel
        Left = 0
        Top = 416
        Width = 499
        Height = 99
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
      end
      object GroupBox1: TGroupBox
        Left = 0
        Top = 0
        Width = 499
        Height = 304
        Align = alClient
        Caption = ' Library list '
        TabOrder = 1
        ExplicitHeight = 302
        object LibsListStringGrid: TStringGrid
          Left = 2
          Top = 15
          Width = 495
          Height = 287
          Align = alClient
          ColCount = 2
          DefaultRowHeight = 18
          FixedCols = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
          TabOrder = 0
          OnMouseEnter = LibsListStringGridMouseEnter
          OnSelectCell = LibsListStringGridSelectCell
          ExplicitHeight = 285
        end
      end
      object GroupBox2: TGroupBox
        Left = 0
        Top = 311
        Width = 499
        Height = 105
        Align = alBottom
        Caption = ' Selected Library Components'
        TabOrder = 2
        object LibComponentListStringGrid: TStringGrid
          Left = 2
          Top = 15
          Width = 495
          Height = 88
          Align = alClient
          ColCount = 2
          DefaultRowHeight = 18
          FixedCols = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
          TabOrder = 0
          OnMouseEnter = LibComponentListStringGridMouseEnter
        end
      end
    end
  end
end
