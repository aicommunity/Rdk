object ImagesFrame: TImagesFrame
  Left = 0
  Top = 0
  Width = 536
  Height = 537
  TabOrder = 0
  object DrawGrid: TDrawGrid
    Left = 0
    Top = 0
    Width = 536
    Height = 537
    Align = alClient
    ColCount = 2
    DefaultColWidth = 320
    DefaultRowHeight = 256
    DoubleBuffered = True
    FixedCols = 0
    RowCount = 2
    FixedRows = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goDrawFocusSelected]
    ParentDoubleBuffered = False
    TabOrder = 0
    OnDrawCell = DrawGridDrawCell
  end
end
