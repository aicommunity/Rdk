inherited UEngineMonitorFrame: TUEngineMonitorFrame
  Width = 568
  Height = 407
  ExplicitWidth = 568
  ExplicitHeight = 407
  object RichEdit: TRichEdit
    Left = 0
    Top = 0
    Width = 568
    Height = 369
    Align = alClient
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssBoth
    TabOrder = 0
    Zoom = 100
    OnMouseEnter = RichEditMouseEnter
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 388
    Width = 568
    Height = 19
    Panels = <
      item
        Width = 160
      end
      item
        Width = 160
      end
      item
        Width = 150
      end
      item
        Width = 70
      end
      item
        Width = 70
      end>
  end
  object Panel1: TPanel
    Left = 0
    Top = 369
    Width = 568
    Height = 19
    Align = alBottom
    TabOrder = 2
    object ShowDebugMessagesCheckBox: TCheckBox
      Left = 1
      Top = 1
      Width = 136
      Height = 17
      Align = alLeft
      Caption = 'Show debug messages'
      TabOrder = 0
      OnClick = ShowDebugMessagesCheckBoxClick
    end
    object AutoupdatePropertiesCheckBox: TCheckBox
      Left = 137
      Top = 1
      Width = 144
      Height = 17
      Align = alLeft
      Caption = 'Autoupdate properties'
      TabOrder = 1
      OnClick = AutoupdatePropertiesCheckBoxClick
    end
  end
  object MainMenu: TMainMenu
    Left = 16
    Top = 16
    object Control1: TMenuItem
      Caption = 'Control'
    end
    object ools1: TMenuItem
      Caption = 'Tools'
      object SaveClassesDescriptions1: TMenuItem
        Caption = 'Save Classes Descriptions'
        OnClick = SaveClassesDescriptions1Click
      end
      object LoadAllClassesDescriptions1: TMenuItem
        Caption = 'Load Classes Descriptions'
        OnClick = LoadAllClassesDescriptions1Click
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
