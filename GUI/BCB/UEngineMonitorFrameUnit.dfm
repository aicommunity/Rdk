object UEngineMonitorFrame: TUEngineMonitorFrame
  Left = 0
  Top = 0
  Width = 568
  Height = 407
  TabOrder = 0
  object RichEdit: TRichEdit
    Left = 0
    Top = 0
    Width = 568
    Height = 388
    Align = alClient
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 388
    Width = 568
    Height = 19
    Panels = <>
    SimplePanel = True
  end
  object MainMenu: TMainMenu
    Left = 16
    Top = 16
    object Control1: TMenuItem
      Caption = 'Control'
    end
    object Calculate1: TMenuItem
      Caption = 'Calculate'
      object Start1: TMenuItem
        Caption = 'Start'
        OnClick = Start1Click
      end
      object Pause1: TMenuItem
        Caption = 'Pause'
        OnClick = Pause1Click
      end
      object Reset1: TMenuItem
        Caption = 'Reset'
        OnClick = Reset1Click
      end
      object Step1: TMenuItem
        Caption = 'Step'
        OnClick = Step1Click
      end
    end
  end
  object Timer: TTimer
    Enabled = False
    Interval = 1
    OnTimer = TimerTimer
    Left = 64
    Top = 16
  end
end
