inherited UImagesFrame: TUImagesFrame
  Width = 729
  Height = 505
  ExplicitWidth = 729
  ExplicitHeight = 505
  object DrawGrid: TDrawGrid
    Left = 0
    Top = 0
    Width = 729
    Height = 488
    Align = alClient
    ColCount = 2
    DefaultColWidth = 360
    DefaultRowHeight = 240
    DoubleBuffered = True
    FixedCols = 0
    RowCount = 2
    FixedRows = 0
    Options = [goVertLine, goHorzLine, goDrawFocusSelected, goRowSizing, goColSizing, goAlwaysShowEditor]
    ParentDoubleBuffered = False
    PopupMenu = PopupMenu
    TabOrder = 0
    OnClick = DrawGridClick
    OnDblClick = DrawGridDblClick
    OnDrawCell = DrawGridDrawCell
  end
  object ScrollBox1: TScrollBox
    Left = 0
    Top = 0
    Width = 729
    Height = 488
    Align = alClient
    TabOrder = 1
    Visible = False
    object FullImage: TImage
      Left = 237
      Top = 172
      Width = 105
      Height = 105
      IncrementalDisplay = True
      OnDblClick = FullImageDblClick
      OnMouseDown = FullImageMouseDown
      OnMouseMove = FullImageMouseMove
      OnMouseUp = FullImageMouseUp
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 488
    Width = 729
    Height = 17
    Align = alBottom
    TabOrder = 2
    object ShowLegendCheckBox: TCheckBox
      Left = 1
      Top = 1
      Width = 88
      Height = 15
      Align = alLeft
      Caption = 'Show legend'
      Checked = True
      DoubleBuffered = True
      ParentDoubleBuffered = False
      State = cbChecked
      TabOrder = 0
    end
    object ShowHistogramCheckBox: TCheckBox
      Left = 89
      Top = 1
      Width = 104
      Height = 15
      Align = alLeft
      Caption = 'Show histogram'
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 1
    end
    object TiledSizeRadioButton: TRadioButton
      Left = 656
      Top = 1
      Width = 72
      Height = 15
      Align = alRight
      Caption = 'Tiled size'
      Checked = True
      TabOrder = 2
      TabStop = True
      OnClick = TiledSizeRadioButtonClick
    end
    object OriginalSizeRadioButton: TRadioButton
      Left = 568
      Top = 1
      Width = 88
      Height = 15
      Align = alRight
      Caption = 'Original size'
      TabOrder = 3
      OnClick = OriginalSizeRadioButtonClick
    end
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
    DefaultExt = 'bmp'
    Filter = 'JPEG Image File (*.jpg)|*.jpg|Bitmaps (*.bmp)|*.bmp'
    Left = 240
    Top = 296
  end
end
