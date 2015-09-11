object UWatchForm: TUWatchForm
  Left = 230
  Top = 232
  BorderIcons = [biSystemMenu, biMinimize, biMaximize, biHelp]
  Caption = 'Watch'
  ClientHeight = 433
  ClientWidth = 684
  Color = clBtnFace
  DoubleBuffered = True
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnPaint = FormPaint
  PixelsPerInch = 120
  TextHeight = 16
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 684
    Height = 433
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
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
