object IdHttpResultBroadcasterForm: TIdHttpResultBroadcasterForm
  Left = 0
  Top = 0
  Caption = 'Result Broadcaster'
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
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 527
    Height = 243
    Align = alClient
    TabOrder = 0
    ExplicitLeft = 80
    ExplicitTop = 48
    ExplicitWidth = 289
    ExplicitHeight = 193
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
end
