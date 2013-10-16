object UEngineMonitorForm: TUEngineMonitorForm
  Left = 686
  Top = 386
  Caption = 'Engine Monitor'
  ClientHeight = 223
  ClientWidth = 527
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
    Width = 527
    Height = 223
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 527
    ExplicitHeight = 223
    inherited RichEdit: TRichEdit
      Width = 527
      Height = 204
      ExplicitWidth = 527
      ExplicitHeight = 204
    end
    inherited StatusBar: TStatusBar
      Top = 204
      Width = 527
      ExplicitTop = 204
      ExplicitWidth = 527
    end
  end
end
