object IdHttpResultBroadcasterForm: TIdHttpResultBroadcasterForm
  Left = 0
  Top = 0
  Caption = 'Http Broadcaster'
  ClientHeight = 304
  ClientWidth = 659
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 120
  TextHeight = 17
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 659
    Height = 283
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    TabOrder = 0
  end
  object BroadcastEnabledCheckBox: TCheckBox
    Left = 0
    Top = 283
    Width = 659
    Height = 21
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alBottom
    Caption = 'Broadcast enabled'
    TabOrder = 1
    OnClick = BroadcastEnabledCheckBoxClick
  end
  object MainMenu: TMainMenu
    Left = 144
    Top = 40
    object Servers1: TMenuItem
      Caption = 'Broadcasters'
      object Add1: TMenuItem
        Caption = 'Add'
        OnClick = Add1Click
      end
      object Del1: TMenuItem
        Caption = 'Delete'
        OnClick = Del1Click
      end
    end
    object Connect1: TMenuItem
      Caption = 'Connection'
      object ConnectAll1: TMenuItem
        Caption = 'Connect All'
        OnClick = ConnectAll1Click
      end
      object DisconnectAll1: TMenuItem
        Caption = 'Disconnect All'
        OnClick = DisconnectAll1Click
      end
    end
  end
  object PopupMenu1: TPopupMenu
    Left = 344
    Top = 80
    object HttpBroadcaster1: TMenuItem
      Caption = 'Http Broadcaster'
      OnClick = HttpBroadcaster1Click
    end
  end
end
