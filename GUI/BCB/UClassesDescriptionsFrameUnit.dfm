inherited UClassesDescriptionsFrame: TUClassesDescriptionsFrame
  Width = 1431
  Height = 671
  ExplicitWidth = 1431
  ExplicitHeight = 671
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1431
    Height = 616
    Align = alClient
    TabOrder = 0
    inline ClassesListFrame: TUClassesListFrame
      Left = 1
      Top = 1
      Width = 272
      Height = 614
      Align = alLeft
      DoubleBuffered = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 0
      ExplicitLeft = 1
      ExplicitTop = 1
      ExplicitWidth = 272
      ExplicitHeight = 614
      inherited PageControl: TPageControl
        Width = 272
        Height = 614
        ActivePage = ClassesListFrame.LibsTabSheet
        ExplicitWidth = 272
        ExplicitHeight = 614
        inherited NameTabSheet: TTabSheet
          ExplicitLeft = 4
          ExplicitTop = 24
          ExplicitWidth = 264
          ExplicitHeight = 586
          inherited StringGrid: TStringGrid
            Width = 264
            Height = 586
            ExplicitWidth = 264
            ExplicitHeight = 586
          end
        end
        inherited LibsTabSheet: TTabSheet
          ExplicitLeft = 4
          ExplicitTop = 24
          ExplicitWidth = 264
          ExplicitHeight = 586
          inherited TreeView: TTreeView
            Width = 264
            Height = 586
            OnClick = ClassesListFrameTreeViewClick
            ExplicitWidth = 264
            ExplicitHeight = 586
          end
        end
        inherited LibsControlTabSheet: TTabSheet
          TabVisible = False
          ExplicitLeft = 4
          ExplicitTop = 24
          ExplicitWidth = 499
          ExplicitHeight = 515
          inherited Splitter1: TSplitter
            ExplicitTop = 296
          end
        end
      end
    end
    object Panel3: TPanel
      Left = 273
      Top = 1
      Width = 583
      Height = 614
      Align = alClient
      TabOrder = 1
      DesignSize = (
        583
        614)
      object ClassNameLabeledEdit: TLabeledEdit
        Left = 6
        Top = 24
        Width = 571
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 55
        EditLabel.Height = 13
        EditLabel.Caption = 'Class Name'
        TabOrder = 0
      end
      object ClassFullNameLabeledEdit: TLabeledEdit
        Left = 6
        Top = 72
        Width = 571
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 74
        EditLabel.Height = 13
        EditLabel.Caption = 'Class Full Name'
        TabOrder = 1
      end
      object ClassDescriptionGroupBox: TGroupBox
        Left = 1
        Top = 109
        Width = 581
        Height = 134
        Align = alBottom
        Caption = 'Class Description'
        TabOrder = 2
        ExplicitLeft = 6
        ExplicitTop = 99
        ExplicitWidth = 571
        object ClassDescriptionRichEdit: TRichEdit
          Left = 2
          Top = 15
          Width = 577
          Height = 117
          Align = alClient
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          PlainText = True
          ScrollBars = ssBoth
          TabOrder = 0
          ExplicitWidth = 567
        end
      end
      object GroupBox1: TGroupBox
        Left = 1
        Top = 243
        Width = 581
        Height = 370
        Align = alBottom
        Caption = ' Properties List '
        TabOrder = 3
        ExplicitLeft = 6
        ExplicitTop = 239
        ExplicitWidth = 571
        object PropertiesListStringGrid: TStringGrid
          Left = 2
          Top = 15
          Width = 577
          Height = 353
          Align = alClient
          TabOrder = 0
          ExplicitWidth = 567
        end
      end
    end
    object Panel4: TPanel
      Left = 856
      Top = 1
      Width = 574
      Height = 614
      Align = alRight
      TabOrder = 2
      object GroupBox2: TGroupBox
        Left = 1
        Top = 239
        Width = 572
        Height = 374
        Align = alClient
        Caption = ' Property Description '
        TabOrder = 0
        object PropertyDescriptionRichEdit: TRichEdit
          Left = 2
          Top = 15
          Width = 568
          Height = 357
          Align = alClient
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          PlainText = True
          ScrollBars = ssBoth
          TabOrder = 0
        end
      end
      object GroupBox3: TGroupBox
        Left = 1
        Top = 1
        Width = 572
        Height = 238
        Align = alTop
        Caption = ' Classes Additional info  '
        TabOrder = 1
        object RichEdit1: TRichEdit
          Left = 2
          Top = 15
          Width = 568
          Height = 221
          Align = alClient
          Font.Charset = RUSSIAN_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          ScrollBars = ssBoth
          TabOrder = 0
        end
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 616
    Width = 1431
    Height = 55
    Align = alBottom
    TabOrder = 1
    DesignSize = (
      1431
      55)
    object ApplyButton: TButton
      Left = 1263
      Top = 16
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Apply'
      TabOrder = 0
    end
    object RestoreButton: TButton
      Left = 1344
      Top = 16
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Restore'
      TabOrder = 1
    end
    object SaveDescriptionsButton: TButton
      Left = 16
      Top = 16
      Width = 105
      Height = 25
      Caption = 'Save Descriptions'
      TabOrder = 2
    end
  end
end
