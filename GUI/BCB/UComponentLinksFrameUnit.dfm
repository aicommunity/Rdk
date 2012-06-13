object UComponentLinksFrame: TUComponentLinksFrame
  Left = 0
  Top = 0
  Width = 620
  Height = 428
  TabOrder = 0
  OnResize = FrameResize
  object Splitter1: TSplitter
    Left = 0
    Top = 216
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
    Height = 216
    Align = alRight
    Caption = ' '#1042#1093#1086#1076#1099' '
    TabOrder = 0
    inline NANetFrameInputs: TUComponentIOFrame
      Left = 2
      Top = 15
      Width = 301
      Height = 199
      Align = alClient
      TabOrder = 0
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 301
      ExplicitHeight = 199
      inherited StringGrid: TStringGrid
        Width = 301
        Height = 199
        ExplicitWidth = 301
        ExplicitHeight = 199
      end
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 0
    Width = 315
    Height = 216
    Align = alClient
    Caption = ' '#1042#1099#1093#1086#1076#1099' '
    TabOrder = 1
    inline NANetFrameOutputs: TUComponentIOFrame
      Left = 2
      Top = 15
      Width = 311
      Height = 199
      Align = alClient
      TabOrder = 0
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 311
      ExplicitHeight = 199
      inherited StringGrid: TStringGrid
        Width = 311
        Height = 199
        ExplicitWidth = 311
        ExplicitHeight = 199
      end
    end
  end
  object GroupBox3: TGroupBox
    Left = 0
    Top = 223
    Width = 620
    Height = 168
    Align = alBottom
    Caption = ' '#1057#1074#1103#1079#1080' '
    TabOrder = 2
    inline NANetFrameLinks: TUComponentIOFrame
      Left = 2
      Top = 15
      Width = 616
      Height = 151
      Align = alClient
      TabOrder = 0
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 616
      ExplicitHeight = 151
      inherited StringGrid: TStringGrid
        Width = 616
        Height = 151
        ExplicitWidth = 616
        ExplicitHeight = 151
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 391
    Width = 620
    Height = 37
    Align = alBottom
    TabOrder = 3
    object HeaderControl1: THeaderControl
      Left = 1
      Top = 1
      Width = 618
      Height = 35
      Align = alClient
      Sections = <
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Create link'
          Width = 80
        end
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Break link'
          Width = 80
        end
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Break all'
          Width = 80
        end
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Chain linking'
          Width = 80
        end
        item
          ImageIndex = -1
          Text = 'Parallel linking'
          Width = 80
        end>
      OnSectionClick = HeaderControl1SectionClick
    end
  end
end
