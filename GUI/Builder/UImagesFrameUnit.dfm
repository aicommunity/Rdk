object UImagesFrame: TUImagesFrame
  Left = 0
  Top = 0
  Width = 667
  Height = 585
  TabOrder = 0
  object DrawGrid: TDrawGrid
    Left = 0
    Top = 0
    Width = 667
    Height = 585
    Align = alClient
    ColCount = 2
    DefaultColWidth = 360
    DefaultRowHeight = 240
    DoubleBuffered = True
    FixedCols = 0
    RowCount = 2
    FixedRows = 0
    Options = [goVertLine, goHorzLine, goDrawFocusSelected, goRowSizing, goColSizing, goEditing, goAlwaysShowEditor]
    ParentDoubleBuffered = False
    TabOrder = 0
    OnDrawCell = DrawGridDrawCell
    ExplicitHeight = 536
  end
end
