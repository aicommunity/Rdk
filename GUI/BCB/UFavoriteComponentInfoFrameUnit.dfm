inherited UFavoriteComponentInfoFrame: TUFavoriteComponentInfoFrame
  object StringGrid: TStringGrid
    Left = 0
    Top = 0
    Width = 320
    Height = 240
    Align = alClient
    ColCount = 2
    DoubleBuffered = True
    FixedCols = 0
    RowCount = 2
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goColSizing, goRowSelect]
    ParentDoubleBuffered = False
    PopupMenu = PopupMenu
    TabOrder = 0
  end
  object PopupMenu: TPopupMenu
    Left = 136
    Top = 120
    object Add1: TMenuItem
      Caption = 'Add'
      OnClick = Add1Click
    end
    object Delete1: TMenuItem
      Caption = 'Delete'
      OnClick = Delete1Click
    end
    object Rename1: TMenuItem
      Caption = 'Rename'
      OnClick = Rename1Click
    end
    object Moveup1: TMenuItem
      Caption = 'Move Up'
      OnClick = Moveup1Click
    end
    object MoveDown1: TMenuItem
      Caption = 'Move Down'
      OnClick = MoveDown1Click
    end
  end
end
