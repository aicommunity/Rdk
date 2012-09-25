object UFavoriteComponentInfoForm: TUFavoriteComponentInfoForm
  Left = 0
  Top = 0
  Caption = 'Favorite Information'
  ClientHeight = 243
  ClientWidth = 527
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 527
    Height = 243
    Align = alClient
    PopupMenu = PopupMenu
    TabOrder = 0
    ExplicitLeft = 160
    ExplicitTop = 112
    ExplicitWidth = 289
    ExplicitHeight = 193
  end
  object PopupMenu: TPopupMenu
    Left = 224
    Top = 96
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
