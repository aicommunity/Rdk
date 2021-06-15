inherited UClassesListFrame: TUClassesListFrame
  Width = 507
  Height = 543
  DoubleBuffered = True
  ParentDoubleBuffered = False
  ExplicitWidth = 507
  ExplicitHeight = 543
  object PageControl: TPageControl
    Left = 0
    Top = 21
    Width = 507
    Height = 522
    ActivePage = LibsTabSheet
    Align = alClient
    TabOrder = 0
    OnChange = PageControlChange
    object NameTabSheet: TTabSheet
      Caption = 'By Name'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object StringGrid: TStringGrid
        Left = 0
        Top = 0
        Width = 499
        Height = 494
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
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object TreeView: TTreeView
        Left = 0
        Top = 0
        Width = 499
        Height = 494
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
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object Splitter1: TSplitter
        Left = 0
        Top = 177
        Width = 499
        Height = 7
        Cursor = crVSplit
        Align = alTop
        ExplicitLeft = 3
        ExplicitTop = 295
      end
      object GroupBox1: TGroupBox
        Left = 0
        Top = 0
        Width = 499
        Height = 177
        Align = alTop
        Caption = ' Library list '
        TabOrder = 0
        object LibsListStringGrid: TStringGrid
          Left = 2
          Top = 15
          Width = 495
          Height = 160
          Align = alClient
          ColCount = 2
          DefaultRowHeight = 18
          FixedCols = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
          PopupMenu = RTLibsPopupMenu
          TabOrder = 0
          OnMouseEnter = LibsListStringGridMouseEnter
          OnSelectCell = LibsListStringGridSelectCell
        end
      end
      object GroupBox2: TGroupBox
        Left = 0
        Top = 184
        Width = 499
        Height = 310
        Align = alClient
        Caption = ' Selected Library Components'
        TabOrder = 1
        object LibComponentListStringGrid: TStringGrid
          Left = 2
          Top = 15
          Width = 495
          Height = 293
          Align = alClient
          ColCount = 2
          DefaultRowHeight = 18
          FixedCols = 0
          Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
          ParentShowHint = False
          PopupMenu = RTClassesPopupMenu
          ShowHint = True
          TabOrder = 0
          OnMouseEnter = LibComponentListStringGridMouseEnter
          OnMouseMove = LibComponentListStringGridMouseMove
        end
      end
    end
  end
  object SearchEdit: TEdit
    Left = 0
    Top = 0
    Width = 507
    Height = 21
    Align = alTop
    TabOrder = 1
    OnChange = SearchEditChange
  end
  object RTLibsPopupMenu: TPopupMenu
    Left = 356
    Top = 109
    object CreateRuntimeLibrary1: TMenuItem
      Caption = 'Create Runtime Library'
      OnClick = CreateRuntimeLibrary1Click
    end
    object DeleteRuntimeLibrary1: TMenuItem
      Caption = 'Delete Runtime Library'
      OnClick = DeleteRuntimeLibrary1Click
    end
  end
  object RTClassesPopupMenu: TPopupMenu
    Left = 356
    Top = 317
    object DeleteClass1: TMenuItem
      Caption = 'Delete Class'
      OnClick = DeleteClass1Click
    end
  end
end
