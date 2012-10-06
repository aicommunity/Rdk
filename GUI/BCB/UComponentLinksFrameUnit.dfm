inherited UComponentLinksFrame: TUComponentLinksFrame
  Width = 620
  Height = 428
  OnResize = FrameResize
  ExplicitWidth = 620
  ExplicitHeight = 428
  object Splitter1: TSplitter
    Left = 0
    Top = 175
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
    Height = 175
    Align = alRight
    Caption = ' '#1042#1093#1086#1076#1099' '
    TabOrder = 0
    inline NANetFrameInputs: TUComponentIOFrame
      Left = 2
      Top = 15
      Width = 301
      Height = 158
      Align = alClient
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 301
      ExplicitHeight = 158
      inherited StringGrid: TStringGrid
        Width = 301
        Height = 158
        ExplicitWidth = 301
        ExplicitHeight = 158
      end
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 0
    Width = 315
    Height = 175
    Align = alClient
    Caption = ' '#1042#1099#1093#1086#1076#1099' '
    TabOrder = 1
    inline NANetFrameOutputs: TUComponentIOFrame
      Left = 2
      Top = 15
      Width = 311
      Height = 158
      Align = alClient
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 311
      ExplicitHeight = 158
      inherited StringGrid: TStringGrid
        Width = 311
        Height = 158
        ExplicitWidth = 311
        ExplicitHeight = 158
      end
    end
  end
  object GroupBox3: TGroupBox
    Left = 0
    Top = 182
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
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
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
    Top = 350
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
  object ButtonPanel: TPanel
    Left = 0
    Top = 387
    Width = 620
    Height = 41
    Align = alBottom
    TabOrder = 4
    Visible = False
    DesignSize = (
      620
      41)
    object OkButton: TButton
      Left = 449
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      Default = True
      ModalResult = 1
      TabOrder = 0
      OnClick = OkButtonClick
    end
    object CancelButton: TButton
      Left = 532
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
  end
end
