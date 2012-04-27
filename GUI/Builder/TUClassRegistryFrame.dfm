object UClassRegistryFrame: TUClassRegistryFrame
  Left = 0
  Top = 0
  Width = 517
  Height = 411
  TabOrder = 0
  object Splitter: TSplitter
    Left = 227
    Top = 25
    Height = 327
    Align = alRight
    ExplicitLeft = 120
    ExplicitTop = 80
    ExplicitHeight = 100
  end
  object Panel: TPanel
    Left = 0
    Top = 352
    Width = 517
    Height = 59
    Align = alBottom
    TabOrder = 0
    DesignSize = (
      517
      59)
    object LabeledEdit: TLabeledEdit
      Left = 8
      Top = 24
      Width = 401
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 119
      EditLabel.Height = 13
      EditLabel.Caption = 'Current parameter value'
      TabOrder = 0
      OnKeyPress = LabeledEditKeyPress
    end
    object SetBitBtn: TBitBtn
      Left = 415
      Top = 22
      Width = 42
      Height = 25
      Anchors = [akTop, akRight]
      Default = True
      Glyph.Data = {
        DE010000424DDE01000000000000760000002800000024000000120000000100
        0400000000006801000000000000000000001000000000000000000000000000
        80000080000000808000800000008000800080800000C0C0C000808080000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        3333333333333333333333330000333333333333333333333333F33333333333
        00003333344333333333333333388F3333333333000033334224333333333333
        338338F3333333330000333422224333333333333833338F3333333300003342
        222224333333333383333338F3333333000034222A22224333333338F338F333
        8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
        33333338F83338F338F33333000033A33333A222433333338333338F338F3333
        0000333333333A222433333333333338F338F33300003333333333A222433333
        333333338F338F33000033333333333A222433333333333338F338F300003333
        33333333A222433333333333338F338F00003333333333333A22433333333333
        3338F38F000033333333333333A223333333333333338F830000333333333333
        333A333333333333333338330000333333333333333333333333333333333333
        0000}
      NumGlyphs = 2
      TabOrder = 1
      OnClick = SetBitBtnClick
    end
    object RestoreBitBtn: TBitBtn
      Left = 463
      Top = 22
      Width = 42
      Height = 25
      Anchors = [akTop, akRight]
      Cancel = True
      Glyph.Data = {
        DE010000424DDE01000000000000760000002800000024000000120000000100
        0400000000006801000000000000000000001000000000000000000000000000
        80000080000000808000800000008000800080800000C0C0C000808080000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
        333333333333333333333333000033338833333333333333333F333333333333
        0000333911833333983333333388F333333F3333000033391118333911833333
        38F38F333F88F33300003339111183911118333338F338F3F8338F3300003333
        911118111118333338F3338F833338F3000033333911111111833333338F3338
        3333F8330000333333911111183333333338F333333F83330000333333311111
        8333333333338F3333383333000033333339111183333333333338F333833333
        00003333339111118333333333333833338F3333000033333911181118333333
        33338333338F333300003333911183911183333333383338F338F33300003333
        9118333911183333338F33838F338F33000033333913333391113333338FF833
        38F338F300003333333333333919333333388333338FFF830000333333333333
        3333333333333333333888330000333333333333333333333333333333333333
        0000}
      NumGlyphs = 2
      TabOrder = 2
      OnClick = RestoreBitBtnClick
    end
  end
  object ListView: TListView
    Left = 230
    Top = 25
    Width = 287
    Height = 327
    Align = alRight
    Columns = <
      item
        Caption = 'Parameter'
      end
      item
        Caption = 'Value'
      end>
    DoubleBuffered = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    FlatScrollBars = True
    GridLines = True
    HideSelection = False
    GroupView = True
    ReadOnly = True
    RowSelect = True
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 1
    ViewStyle = vsReport
    OnSelectItem = ListViewSelectItem
  end
  object HeaderControl: THeaderControl
    Left = 0
    Top = 0
    Width = 517
    Height = 25
    HotTrack = True
    Sections = <>
    OnSectionClick = HeaderControlSectionClick
    NoSizing = True
    OverFlow = True
    DoubleBuffered = True
    ParentDoubleBuffered = False
  end
  object ComponentsListView: TListView
    Left = 0
    Top = 25
    Width = 227
    Height = 327
    Align = alClient
    Columns = <
      item
        Caption = '.. Up to previous level'
        Width = 100
      end>
    DoubleBuffered = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    FlatScrollBars = True
    GridLines = True
    HideSelection = False
    GroupView = True
    ReadOnly = True
    RowSelect = True
    ParentDoubleBuffered = False
    ParentFont = False
    ShowWorkAreas = True
    TabOrder = 3
    ViewStyle = vsReport
    OnColumnClick = ComponentsListViewColumnClick
    OnDblClick = ComponentsListViewDblClick
    OnKeyUp = ComponentsListViewKeyUp
    OnResize = ComponentsListViewResize
    OnSelectItem = ComponentsListViewSelectItem
  end
end
