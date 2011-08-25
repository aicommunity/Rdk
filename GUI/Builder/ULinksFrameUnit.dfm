object NLinksFrame: TNLinksFrame
  Left = 0
  Top = 0
  Width = 620
  Height = 428
  TabOrder = 0
  OnResize = FrameResize
  object Splitter1: TSplitter
    Left = 0
    Top = 218
    Width = 620
    Height = 7
    Cursor = crVSplit
    Align = alBottom
    ExplicitLeft = -62
    ExplicitTop = 265
    ExplicitWidth = 614
  end
  object GroupBox1: TGroupBox
    Left = 315
    Top = 0
    Width = 305
    Height = 218
    Align = alRight
    Caption = ' '#1042#1093#1086#1076#1099' '
    TabOrder = 0
    inline NANetFrameInputs: TNANetFrame
      Left = 2
      Top = 15
      Width = 301
      Height = 201
      Align = alClient
      TabOrder = 0
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 301
      ExplicitHeight = 201
      inherited StringGrid: TStringGrid
        Width = 301
        Height = 201
        ExplicitWidth = 301
        ExplicitHeight = 201
      end
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 0
    Width = 315
    Height = 218
    Align = alClient
    Caption = ' '#1042#1099#1093#1086#1076#1099' '
    TabOrder = 1
    inline NANetFrameOutputs: TNANetFrame
      Left = 2
      Top = 15
      Width = 311
      Height = 201
      Align = alClient
      TabOrder = 0
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 311
      ExplicitHeight = 201
      inherited StringGrid: TStringGrid
        Width = 311
        Height = 201
        ExplicitWidth = 311
        ExplicitHeight = 201
      end
    end
  end
  object GroupBox3: TGroupBox
    Left = 0
    Top = 225
    Width = 620
    Height = 162
    Align = alBottom
    Caption = ' '#1057#1074#1103#1079#1080' '
    TabOrder = 2
    inline NANetFrameLinks: TNANetFrame
      Left = 2
      Top = 15
      Width = 616
      Height = 145
      Align = alClient
      TabOrder = 0
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 616
      ExplicitHeight = 145
      inherited StringGrid: TStringGrid
        Width = 616
        Height = 145
        ExplicitWidth = 616
        ExplicitHeight = 145
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 387
    Width = 620
    Height = 41
    Align = alBottom
    TabOrder = 3
    object CreateLinkButton: TButton
      Left = 9
      Top = 8
      Width = 75
      Height = 25
      Caption = #1057#1074#1103#1079#1072#1090#1100
      TabOrder = 0
      OnClick = CreateLinkButtonClick
    end
    object BreakLinkButton: TButton
      Left = 90
      Top = 8
      Width = 75
      Height = 25
      Caption = #1056#1072#1079#1086#1088#1074#1072#1090#1100
      TabOrder = 1
      OnClick = BreakLinkButtonClick
    end
  end
end
