object ComponentsListFrame: TComponentsListFrame
  Left = 0
  Top = 0
  Width = 456
  Height = 567
  TabOrder = 0
  OnResize = FrameResize
  object StringGrid: TStringGrid
    Left = 0
    Top = 34
    Width = 456
    Height = 533
    Align = alClient
    FixedCols = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
    TabOrder = 0
    OnDblClick = StringGridDblClick
    OnKeyPress = StringGridKeyPress
    OnSelectCell = StringGridSelectCell
  end
  object HeaderControl: THeaderControl
    Left = 0
    Top = 0
    Width = 456
    Height = 34
    HotTrack = True
    Sections = <>
    Style = hsFlat
    OnSectionClick = HeaderControlSectionClick
    NoSizing = True
    OverFlow = True
    ShowHint = False
    ParentShowHint = False
  end
end
