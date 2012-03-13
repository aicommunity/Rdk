object UComponentsControlFrame: TUComponentsControlFrame
  Left = 0
  Top = 0
  Width = 987
  Height = 526
  TabOrder = 0
  object Panel1: TPanel
    Left = 0
    Top = 488
    Width = 987
    Height = 38
    Align = alBottom
    TabOrder = 0
    object HeaderControl: THeaderControl
      Left = 1
      Top = 1
      Width = 985
      Height = 36
      Align = alClient
      Sections = <
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Create Model'
          Width = 90
        end
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Destroy Model'
          Width = 90
        end
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Load Model'
          Width = 90
        end
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Save Model'
          Width = 90
        end>
      OnSectionClick = HeaderControlSectionClick
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 987
    Height = 488
    Align = alClient
    TabOrder = 1
    object Splitter1: TSplitter
      Left = 305
      Top = 1
      Width = 7
      Height = 486
      ExplicitLeft = 298
      ExplicitTop = 2
    end
    object GroupBox1: TGroupBox
      Left = 376
      Top = 1
      Width = 610
      Height = 486
      Align = alClient
      Caption = ' Model '
      TabOrder = 0
      inline ComponentsListFrame: TUComponentsListFrame
        Left = 2
        Top = 15
        Width = 606
        Height = 469
        Align = alClient
        TabOrder = 0
        ExplicitLeft = -145
        ExplicitTop = -81
        inherited Splitter1: TSplitter
          Height = 469
        end
        inherited Panel1: TPanel
          Width = 237
          Height = 469
          inherited ParametersRichEdit: TRichEdit
            Width = 235
            Height = 433
          end
          inherited ParametersHeaderControl: THeaderControl
            Width = 235
          end
        end
        inherited Panel2: TPanel
          Height = 469
          inherited StringGrid: TStringGrid
            Height = 433
          end
        end
      end
    end
    object GroupBox2: TGroupBox
      Left = 1
      Top = 1
      Width = 304
      Height = 486
      Align = alLeft
      Caption = ' Classes list '
      TabOrder = 1
      inline ClassesListFrame: TUClassesListFrame
        Left = 2
        Top = 15
        Width = 300
        Height = 469
        Align = alClient
        DoubleBuffered = True
        ParentDoubleBuffered = False
        TabOrder = 0
        ExplicitLeft = -203
        ExplicitTop = 39
        inherited StringGrid: TStringGrid
          Width = 300
          Height = 469
        end
      end
    end
    object Panel3: TPanel
      Left = 312
      Top = 1
      Width = 64
      Height = 486
      Align = alLeft
      TabOrder = 2
      object TakeObjectButton: TButton
        Left = 6
        Top = 184
        Width = 51
        Height = 25
        Caption = '>>>'
        TabOrder = 0
        OnClick = TakeObjectButtonClick
      end
      object ReturnObjectButton: TButton
        Left = 6
        Top = 215
        Width = 51
        Height = 25
        Caption = '<<<'
        TabOrder = 1
        OnClick = ReturnObjectButtonClick
      end
    end
  end
  object OpenTextFileDialog: TOpenTextFileDialog
    Left = 312
    Top = 16
  end
  object SaveTextFileDialog: TSaveTextFileDialog
    Left = 312
    Top = 72
  end
end
