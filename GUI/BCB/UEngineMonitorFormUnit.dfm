object UEngineMonitorForm: TUEngineMonitorForm
  Left = 686
  Top = 386
  Caption = 'Engine Monitor'
  ClientHeight = 293
  ClientWidth = 750
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = EngineMonitorFrame.MainMenu
  OldCreateOrder = False
  Position = poDesigned
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 120
  TextHeight = 17
  inline EngineMonitorFrame: TUEngineMonitorFrame
    Left = 0
    Top = 0
    Width = 750
    Height = 293
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 689
    ExplicitHeight = 293
    inherited RichEdit: TRichEdit
      Width = 750
      Height = 250
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Font.Height = -15
      OnMouseEnter = EngineMonitorFrameRichEditMouseEnter
      ExplicitWidth = 689
      ExplicitHeight = 250
    end
    inherited StatusBar: TStatusBar
      Top = 250
      Width = 750
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      ExplicitTop = 250
      ExplicitWidth = 689
    end
    inherited Panel1: TPanel
      Top = 269
      Width = 750
      Height = 24
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      ExplicitTop = 269
      ExplicitWidth = 689
      ExplicitHeight = 24
      inherited ShowDebugMessagesCheckBox: TCheckBox
        Width = 184
        Height = 22
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ExplicitWidth = 184
        ExplicitHeight = 22
      end
      inherited AutoupdatePropertiesCheckBox: TCheckBox
        Left = 185
        Width = 180
        Height = 22
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ExplicitLeft = 690
        ExplicitWidth = 180
        ExplicitHeight = 22
      end
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
