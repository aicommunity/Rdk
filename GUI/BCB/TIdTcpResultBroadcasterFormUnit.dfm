object IdTcpResultBroadcasterForm: TIdTcpResultBroadcasterForm
  Left = 0
  Top = 0
  Caption = 'Tcp Broadcaster'
  ClientHeight = 243
  ClientWidth = 527
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl: TPageControl
    Left = 0
    Top = 29
    Width = 527
    Height = 214
    Align = alClient
    TabOrder = 0
    ExplicitTop = 0
    ExplicitHeight = 243
  end
  object ToolBar: TToolBar
    Left = 0
    Top = 0
    Width = 527
    Height = 29
    ButtonHeight = 0
    ButtonWidth = 0
    Caption = 'ToolBar'
    Menu = MainMenu
    ShowCaptions = True
    TabOrder = 1
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
      Caption = 'Tcp Broadcaster'
      OnClick = HttpBroadcaster1Click
    end
  end
end
