object UWatchForm: TUWatchForm
  Left = 230
  Top = 232
  BorderIcons = [biSystemMenu, biMinimize, biMaximize, biHelp]
  Caption = 'Watch'
  ClientHeight = 346
  ClientWidth = 547
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 547
    Height = 346
    Align = alClient
    DoubleBuffered = True
    HotTrack = True
    MultiLine = True
    ParentDoubleBuffered = False
    PopupMenu = PageControlPopupMenu
    TabOrder = 0
  end
  object PageControlPopupMenu: TPopupMenu
    Left = 160
    Top = 8
    object AddPage1: TMenuItem
      Caption = 'Add Page'
      OnClick = AddPage1Click
    end
    object DeletePage1: TMenuItem
      Caption = 'Delete Page'
      OnClick = DeletePage1Click
    end
    object RenamePage1: TMenuItem
      Caption = 'Rename Page'
      OnClick = RenamePage1Click
    end
  end
end
