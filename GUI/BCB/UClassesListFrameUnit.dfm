inherited UClassesListFrame: TUClassesListFrame
  Width = 507
  Height = 447
  DoubleBuffered = True
  ParentDoubleBuffered = False
  ExplicitWidth = 507
  ExplicitHeight = 447
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 507
    Height = 447
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
        Height = 419
        Align = alClient
        ColCount = 2
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
        Height = 419
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
  end
end
