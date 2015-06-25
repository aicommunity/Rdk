object UEngineMonitorForm: TUEngineMonitorForm
  Left = 686
  Top = 386
  Caption = 'Engine Monitor'
  ClientHeight = 234
  ClientWidth = 551
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = EngineMonitorFrame.MainMenu
  OldCreateOrder = False
  Position = poDesigned
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  inline EngineMonitorFrame: TUEngineMonitorFrame
    Left = 0
    Top = 0
    Width = 551
    Height = 234
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 551
    ExplicitHeight = 234
    inherited RichEdit: TRichEdit
      Width = 551
      Height = 196
      Font.Height = -12
      OnMouseEnter = EngineMonitorFrameRichEditMouseEnter
      ExplicitWidth = 551
      ExplicitHeight = 200
    end
    inherited StatusBar: TStatusBar
      Top = 196
      Width = 551
      ExplicitTop = 200
      ExplicitWidth = 689
    end
    inherited ShowDebugMessagesCheckBox: TCheckBox
      Top = 215
      Width = 551
      Height = 19
      ExplicitTop = 215
      ExplicitWidth = 551
      ExplicitHeight = 19
    end
  end
  object LogTimer: TTimer
    Enabled = False
    Interval = 500
    OnTimer = LogTimerTimer
    Left = 128
    Top = 16
  end
end
