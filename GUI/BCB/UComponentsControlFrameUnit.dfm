inherited UComponentsControlFrame: TUComponentsControlFrame
  Width = 987
  Height = 526
  ExplicitWidth = 987
  ExplicitHeight = 526
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
        end
        item
          Alignment = taCenter
          AllowClick = False
          ImageIndex = -1
          Text = 'Load Parameters'
          Width = 100
        end
        item
          Alignment = taCenter
          Checked = True
          ImageIndex = -1
          Text = 'Save Parameters'
          Width = 100
        end
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Load States'
          Width = 80
        end
        item
          Alignment = taCenter
          ImageIndex = -1
          Text = 'Save States'
          Width = 80
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
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        ExplicitLeft = 2
        ExplicitTop = 15
        ExplicitWidth = 606
        ExplicitHeight = 469
        inherited Splitter1: TSplitter
          Height = 469
          ExplicitHeight = 469
        end
        inherited Panel1: TPanel
          Width = 237
          Height = 469
          ExplicitWidth = 237
          ExplicitHeight = 469
          inherited PageControl1: TPageControl
            Width = 235
            Height = 467
            ExplicitWidth = 235
            ExplicitHeight = 467
            inherited TabSheet4: TTabSheet
              ExplicitWidth = 227
              ExplicitHeight = 441
              inherited Splitter3: TSplitter
                Top = 364
                Width = 227
              end
              inherited ParametersListStringGrid: TStringGrid
                Width = 227
                Height = 364
              end
              inherited ParameterValueRichEdit: TRichEdit
                Top = 374
                Width = 227
                ExplicitTop = 374
                ExplicitWidth = 227
              end
              inherited HeaderControl3: THeaderControl
                Top = 407
                Width = 227
              end
            end
            inherited TabSheet3: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 4
              ExplicitWidth = 376
              ExplicitHeight = 539
            end
            inherited TabSheet1: TTabSheet
              inherited ParametersHeaderControl: THeaderControl
                ExplicitWidth = 227
              end
            end
          end
        end
        inherited Panel2: TPanel
          Height = 469
          ExplicitHeight = 469
          inherited StringGrid: TStringGrid
            Height = 433
            ExplicitHeight = 433
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
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentDoubleBuffered = False
        ParentFont = False
        TabOrder = 0
        ExplicitLeft = 2
        ExplicitTop = 15
        ExplicitWidth = 300
        ExplicitHeight = 469
        inherited StringGrid: TStringGrid
          Width = 300
          Height = 469
          OnDblClick = ClassesListFrameStringGridDblClick
          OnKeyPress = ClassesListFrameStringGridKeyPress
          ExplicitWidth = 300
          ExplicitHeight = 469
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
    DefaultExt = 'xml'
    Filter = 'XML '#1092#1072#1081#1083#1099' '#1086#1087#1080#1089#1072#1085#1080#1103' '#1089#1090#1088#1091#1082#1090#1091#1088#1099' '#1084#1086#1076#1077#1083#1080'|*.xml'
    FilterIndex = 0
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 312
    Top = 16
  end
  object SaveTextFileDialog: TSaveTextFileDialog
    DefaultExt = 'xml'
    Filter = 'XML '#1092#1072#1081#1083#1099' '#1086#1087#1080#1089#1072#1085#1080#1103' '#1089#1090#1088#1091#1082#1090#1091#1088#1099' '#1084#1086#1076#1077#1083#1080'|*.xml'
    FilterIndex = 0
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofEnableSizing]
    Left = 312
    Top = 72
  end
end
