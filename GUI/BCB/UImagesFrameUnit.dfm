inherited UImagesFrame: TUImagesFrame
  Width = 730
  Height = 509
  ExplicitWidth = 730
  ExplicitHeight = 509
  object DrawGrid: TDrawGrid
    Left = 0
    Top = 0
    Width = 730
    Height = 492
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
    Width = 730
    Height = 492
    Align = alClient
    DoubleBuffered = True
    Color = clBtnFace
    ParentColor = False
    ParentDoubleBuffered = False
    TabOrder = 1
    Visible = False
    object FullImage: TImage
      Left = 237
      Top = 172
      Width = 105
      Height = 105
      IncrementalDisplay = True
      Proportional = True
      OnDblClick = FullImageDblClick
      OnMouseDown = FullImageMouseDown
      OnMouseMove = FullImageMouseMove
      OnMouseUp = FullImageMouseUp
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 492
    Width = 730
    Height = 17
    Align = alBottom
    TabOrder = 2
    object Labelshow: TLabel
      Left = 1
      Top = 1
      Width = 38
      Height = 15
      Align = alLeft
      Caption = 'show   '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitHeight = 13
    end
    object Labelsize: TLabel
      Left = 520
      Top = 1
      Width = 31
      Height = 15
      Align = alRight
      Caption = 'size   '
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitHeight = 13
    end
    object ShowLegendCheckBox: TCheckBox
      Left = 39
      Top = 1
      Width = 58
      Height = 15
      Align = alLeft
      Caption = 'legend'
      Checked = True
      DoubleBuffered = True
      ParentDoubleBuffered = False
      State = cbChecked
      TabOrder = 0
      OnClick = ShowLegendCheckBoxClick
    end
    object ShowHistogramCheckBox: TCheckBox
      Left = 97
      Top = 1
      Width = 70
      Height = 15
      Align = alLeft
      Caption = 'histogram'
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 1
      OnClick = ShowHistogramCheckBoxClick
    end
    object TiledSizeRadioButton: TRadioButton
      Left = 609
      Top = 1
      Width = 45
      Height = 15
      Align = alRight
      Caption = 'Tiled'
      Checked = True
      TabOrder = 2
      TabStop = True
      OnClick = TiledSizeRadioButtonClick
    end
    object OriginalSizeRadioButton: TRadioButton
      Left = 551
      Top = 1
      Width = 58
      Height = 15
      Align = alRight
      Caption = 'Original '
      TabOrder = 3
      OnClick = OriginalSizeRadioButtonClick
    end
    object ShowInfoCheckBox: TCheckBox
      Left = 167
      Top = 1
      Width = 48
      Height = 15
      Align = alLeft
      Caption = 'info'
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 4
      OnClick = ShowInfoCheckBoxClick
    end
    object IndChannelsCheckBox: TCheckBox
      Left = 215
      Top = 1
      Width = 91
      Height = 15
      Align = alLeft
      Caption = 'Ind. channels'
      TabOrder = 5
    end
    object ProportionalSizeRadioButton: TRadioButton
      Left = 654
      Top = 1
      Width = 75
      Height = 15
      Align = alRight
      Caption = 'Proportional'
      TabOrder = 6
      OnClick = ProportionalSizeRadioButtonClick
    end
    object CenterImageCheckBox: TCheckBox
      Left = 306
      Top = 1
      Width = 59
      Height = 15
      Align = alLeft
      Caption = 'Center'
      TabOrder = 7
    end
  end
  object PopupMenu: TPopupMenu
    Left = 176
    Top = 224
    object SaveToBmp: TMenuItem
      Caption = 'Save to Bmp'
      OnClick = SaveToBmpClick
    end
    object SaveToJpeg: TMenuItem
      Caption = 'Save to Jpeg'
      OnClick = SaveToJpegClick
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
    object N4: TMenuItem
      Caption = '-'
    end
    object Update1: TMenuItem
      Caption = 'Update'
      OnClick = Update1Click
    end
  end
  object SavePictureDialog: TSavePictureDialog
    DefaultExt = 'bmp'
    Filter = 'JPEG Image File (*.jpg)|*.jpg|Bitmaps (*.bmp)|*.bmp'
    Left = 240
    Top = 296
  end
end
