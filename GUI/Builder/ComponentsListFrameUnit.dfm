object ComponentsListFrame: TComponentsListFrame
  Left = 0
  Top = 0
  Width = 755
  Height = 567
  TabOrder = 0
  OnResize = FrameResize
  object Panel1: TPanel
    Left = 361
    Top = 0
    Width = 394
    Height = 567
    Align = alClient
    TabOrder = 0
    ExplicitLeft = 384
    ExplicitTop = 208
    ExplicitWidth = 185
    ExplicitHeight = 273
    object ParametersRichEdit: TRichEdit
      Left = 1
      Top = 35
      Width = 392
      Height = 531
      Align = alClient
      Font.Charset = RUSSIAN_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      ScrollBars = ssBoth
      TabOrder = 0
      ExplicitLeft = -114
      ExplicitTop = -260
      ExplicitWidth = 299
      ExplicitHeight = 533
    end
    object ParametersHeaderControl: THeaderControl
      Left = 1
      Top = 1
      Width = 392
      Height = 34
      HotTrack = True
      Sections = <
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Set'
          Width = 50
        end
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Reload'
          Width = 50
        end
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Default'
          Width = 50
        end>
      OnSectionClick = ParametersHeaderControlSectionClick
      ShowHint = False
      ParentShowHint = False
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 361
    Height = 567
    Align = alLeft
    TabOrder = 1
    object StringGrid: TStringGrid
      Left = 1
      Top = 35
      Width = 359
      Height = 531
      Align = alClient
      FixedCols = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
      TabOrder = 0
      OnClick = StringGridClick
      OnDblClick = StringGridDblClick
      OnKeyPress = StringGridKeyPress
      OnSelectCell = StringGridSelectCell
      ExplicitLeft = -160
      ExplicitTop = 1
      ExplicitWidth = 345
      ExplicitHeight = 271
    end
    object HeaderControl: THeaderControl
      Left = 1
      Top = 1
      Width = 359
      Height = 34
      HotTrack = True
      Sections = <>
      Style = hsFlat
      NoSizing = True
      OverFlow = True
      ShowHint = False
      ParentShowHint = False
      ExplicitWidth = 456
    end
  end
end
