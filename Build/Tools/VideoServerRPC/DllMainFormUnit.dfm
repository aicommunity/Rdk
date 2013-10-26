object DllMainForm: TDllMainForm
  Left = 0
  Top = 0
  Caption = 'DllMainForm'
  ClientHeight = 206
  ClientWidth = 455
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object IdTCPClient: TIdTCPClient
    ConnectTimeout = 0
    Host = '127.0.0.1'
    IPVersion = Id_IPv4
    Port = 45050
    ReadTimeout = -1
    Left = 264
    Top = 88
  end
end
