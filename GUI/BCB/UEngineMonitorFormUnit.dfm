object UEngineMonitorForm: TUEngineMonitorForm
  Left = 686
  Top = 386
  Caption = 'Engine Monitor'
  ClientHeight = 234
  ClientWidth = 600
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
    Width = 600
    Height = 234
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 600
    ExplicitHeight = 234
    inherited RichEdit: TRichEdit
      Width = 600
      Height = 215
      Font.Height = -12
      OnMouseEnter = EngineMonitorFrameRichEditMouseEnter
      ExplicitWidth = 600
      ExplicitHeight = 215
    end
    inherited StatusBar: TStatusBar
      Top = 215
      Width = 600
      ExplicitTop = 215
      ExplicitWidth = 600
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
