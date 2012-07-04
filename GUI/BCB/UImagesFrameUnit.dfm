inherited UImagesFrame: TUImagesFrame
  Width = 667
  Height = 585
  ExplicitWidth = 667
  ExplicitHeight = 585
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
    PopupMenu = PopupMenu
    TabOrder = 0
    OnDrawCell = DrawGridDrawCell
  end
  object PopupMenu: TPopupMenu
    Left = 192
    Top = 296
    object SaveToBmp: TMenuItem
      Caption = 'Save to Bmp'
      OnClick = SaveToBmpClick
    end
    object SaveToJpeg: TMenuItem
      Caption = 'Save to Jpeg'
      Enabled = False
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object SaveAllToBmp: TMenuItem
      Caption = 'Save All to Bmp'
      Enabled = False
    end
    object SaveAllToJpeg: TMenuItem
      Caption = 'Save All to Jpeg'
      Enabled = False
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object SelectSource: TMenuItem
      Caption = 'Select Source'
      OnClick = SelectSourceClick
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object AddColumn: TMenuItem
      Caption = 'Add Column'
      OnClick = AddColumnClick
    end
    object AddRow: TMenuItem
      Caption = 'Add Row'
      OnClick = AddRowClick
    end
    object DeleteColumn: TMenuItem
      Caption = 'Delete Column'
      OnClick = DeleteColumnClick
    end
    object DeleteRow: TMenuItem
      Caption = 'Delete Row'
      OnClick = DeleteRowClick
    end
  end
  object SavePictureDialog: TSavePictureDialog
    Filter = 
      'All (*.gif;*.png;*.jpg;*.jpeg;*.bmp;*.tif;*.tiff;*.ico;*.emf;*.w' +
      'mf)|*.gif;*.png;*.jpg;*.jpeg;*.bmp;*.tif;*.tiff;*.ico;*.emf;*.wm' +
      'f|GIF Image (*.gif)|*.gif|Portable Network Graphics (*.png)|*.pn' +
      'g|JPEG Image File (*.jpg)|*.jpg|JPEG Image File (*.jpeg)|*.jpeg|' +
      'Bitmaps (*.bmp)|*.bmp|TIFF Images (*.tif)|*.tif|TIFF Images (*.t' +
      'iff)|*.tiff|Icons (*.ico)|*.ico|Enhanced Metafiles (*.emf)|*.emf' +
      '|Metafiles (*.wmf)|*.wmf'
    Left = 240
    Top = 296
  end
end