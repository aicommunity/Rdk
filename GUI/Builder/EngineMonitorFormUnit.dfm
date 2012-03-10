object EngineMonitorForm: TEngineMonitorForm
  Left = 686
  Top = 386
  Caption = 'EngineMonitorForm'
  ClientHeight = 243
  ClientWidth = 527
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  PixelsPerInch = 96
  TextHeight = 13
  inline EngineMonitorFrame: TEngineMonitorFrame
    Left = 0
    Top = 0
    Width = 527
    Height = 243
    Align = alClient
    TabOrder = 0
    ExplicitLeft = -41
    ExplicitTop = -164
    inherited RichEdit: TRichEdit
      Width = 527
      Height = 224
      ExplicitWidth = 568
      ExplicitHeight = 407
    end
    inherited StatusBar: TStatusBar
      Top = 224
      Width = 527
    end
  end
end
