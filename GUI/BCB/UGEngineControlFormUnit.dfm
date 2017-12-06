object UGEngineControlForm: TUGEngineControlForm
  Left = 150
  Top = 0
  Caption = 'Engine Control'
  ClientHeight = 769
  ClientWidth = 1343
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu
  OldCreateOrder = False
  Position = poDesigned
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 17
  object Panel1: TPanel
    Left = 119
    Top = 44
    Width = 1224
    Height = 676
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    TabOrder = 0
    ExplicitTop = 55
    ExplicitHeight = 659
    object Splitter2: TSplitter
      Left = 328
      Top = 1
      Width = 8
      Height = 497
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      ExplicitHeight = 480
    end
    object Splitter3: TSplitter
      Left = 1
      Top = 498
      Width = 1222
      Height = 9
      Cursor = crVSplit
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alBottom
      ExplicitTop = 481
    end
    object Panel2: TPanel
      Left = 1
      Top = 1
      Width = 327
      Height = 497
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alLeft
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 0
      inline UComponentsListFrame1: TUComponentsListFrame
        Left = 1
        Top = 1
        Width = 325
        Height = 495
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alClient
        DoubleBuffered = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -15
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentDoubleBuffered = False
        ParentFont = False
        TabOrder = 0
        ExplicitLeft = 1
        ExplicitTop = 1
        ExplicitWidth = 325
        ExplicitHeight = 495
        inherited Splitter1: TSplitter
          Left = 0
          Top = 284
          Width = 325
          Height = 10
          Cursor = crVSplit
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alTop
          ExplicitLeft = 0
          ExplicitTop = 284
          ExplicitWidth = 324
          ExplicitHeight = 10
        end
        inherited Panel1: TPanel
          Left = 0
          Top = 294
          Width = 325
          Height = 201
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          ExplicitLeft = 0
          ExplicitTop = 294
          ExplicitWidth = 325
          ExplicitHeight = 201
          inherited PageControl1: TPageControl
            Width = 323
            Height = 199
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            ExplicitWidth = 323
            ExplicitHeight = 199
            inherited TabSheet6: TTabSheet
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ExplicitWidth = 315
              ExplicitHeight = 168
              inherited Splitter5: TSplitter
                Top = 66
                Width = 315
                Height = 13
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitTop = 46
                ExplicitWidth = 311
                ExplicitHeight = 13
              end
              inherited EnchancedSG1: TEnchancedSG
                Width = 315
                Height = 66
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitWidth = 315
                ExplicitHeight = 66
                inherited BasicStringGrid: TStringGrid
                  Left = 21
                  Top = 73
                  Width = 210
                  Height = 127
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  OnMouseEnter = EnchancedSG1BasicStringGridMouseEnter
                  ExplicitLeft = 21
                  ExplicitTop = 73
                  ExplicitWidth = 210
                  ExplicitHeight = 127
                end
                inherited txtStringEdit: TEdit
                  Left = 43
                  Top = 324
                  Width = 157
                  Height = 27
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 43
                  ExplicitTop = 324
                  ExplicitWidth = 157
                  ExplicitHeight = 27
                end
                inherited chbBoolEdit: TCheckBox
                  Left = 46
                  Top = 479
                  Width = 127
                  Height = 22
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 46
                  ExplicitTop = 479
                  ExplicitWidth = 127
                  ExplicitHeight = 22
                end
                inherited udBorderedInt: TUpDown
                  Left = 199
                  Top = 424
                  Width = 22
                  Height = 27
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 199
                  ExplicitTop = 424
                  ExplicitWidth = 22
                  ExplicitHeight = 27
                end
                inherited txtDoubleEdit: TEdit
                  Left = 39
                  Top = 273
                  Width = 159
                  Height = 27
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 39
                  ExplicitTop = 273
                  ExplicitWidth = 159
                  ExplicitHeight = 27
                end
                inherited txtIntEdit: TEdit
                  Left = 43
                  Top = 509
                  Width = 157
                  Height = 27
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 43
                  ExplicitTop = 509
                  ExplicitWidth = 157
                  ExplicitHeight = 27
                end
                inherited txtBorderedIntEdit: TEdit
                  Left = 43
                  Top = 424
                  Width = 157
                  Height = 27
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 43
                  ExplicitTop = 424
                  ExplicitWidth = 157
                  ExplicitHeight = 27
                end
                inherited cmbListEdit: TComboBox
                  Left = 43
                  Top = 375
                  Width = 188
                  Height = 26
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  Font.Height = -15
                  ExplicitLeft = 43
                  ExplicitTop = 375
                  ExplicitWidth = 188
                  ExplicitHeight = 26
                end
              end
              inherited HeaderControl2: THeaderControl
                Top = 123
                Width = 315
                Height = 45
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitTop = 103
                ExplicitWidth = 311
                ExplicitHeight = 45
              end
              inherited NiceParamValRichEdit: TRichEdit
                Top = 79
                Width = 315
                Height = 44
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Font.Height = -15
                OnMouseEnter = UComponentsListFrame1NiceParamValRichEditMouseEnter
                ExplicitTop = 79
                ExplicitWidth = 315
                ExplicitHeight = 44
              end
            end
            inherited TabSheet7: TTabSheet
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              inherited Splitter6: TSplitter
                Top = 438
                Height = 13
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitTop = 573
                ExplicitWidth = 470
                ExplicitHeight = 13
              end
              inherited HeaderControl4: THeaderControl
                Top = 494
                Height = 45
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitTop = 629
                ExplicitWidth = 470
                ExplicitHeight = 45
              end
              inherited NiceStateValRichEdit: TRichEdit
                Top = 451
                Height = 43
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Font.Height = -15
                OnMouseEnter = UComponentsListFrame1NiceStateValRichEditMouseEnter
                ExplicitTop = 451
                ExplicitHeight = 43
              end
              inherited EnchancedSG2: TEnchancedSG
                Height = 438
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitHeight = 438
                inherited BasicStringGrid: TStringGrid
                  Left = 21
                  Top = 73
                  Width = 210
                  Height = 127
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  OnMouseEnter = EnchancedSG2BasicStringGridMouseEnter
                  ExplicitLeft = 21
                  ExplicitTop = 73
                  ExplicitWidth = 210
                  ExplicitHeight = 127
                end
                inherited txtStringEdit: TEdit
                  Left = 43
                  Top = 324
                  Width = 157
                  Height = 27
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 43
                  ExplicitTop = 324
                  ExplicitWidth = 157
                  ExplicitHeight = 27
                end
                inherited chbBoolEdit: TCheckBox
                  Left = 46
                  Top = 479
                  Width = 127
                  Height = 22
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 46
                  ExplicitTop = 479
                  ExplicitWidth = 127
                  ExplicitHeight = 22
                end
                inherited udBorderedInt: TUpDown
                  Left = 199
                  Top = 424
                  Width = 22
                  Height = 27
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 199
                  ExplicitTop = 424
                  ExplicitWidth = 22
                  ExplicitHeight = 27
                end
                inherited txtDoubleEdit: TEdit
                  Left = 39
                  Top = 273
                  Width = 159
                  Height = 27
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 39
                  ExplicitTop = 273
                  ExplicitWidth = 159
                  ExplicitHeight = 27
                end
                inherited txtIntEdit: TEdit
                  Left = 43
                  Top = 509
                  Width = 157
                  Height = 27
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 43
                  ExplicitTop = 509
                  ExplicitWidth = 157
                  ExplicitHeight = 27
                end
                inherited txtBorderedIntEdit: TEdit
                  Left = 43
                  Top = 424
                  Width = 157
                  Height = 27
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 43
                  ExplicitTop = 424
                  ExplicitWidth = 157
                  ExplicitHeight = 27
                end
                inherited cmbListEdit: TComboBox
                  Left = 43
                  Top = 375
                  Width = 188
                  Height = 26
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  Font.Height = -15
                  ExplicitLeft = 43
                  ExplicitTop = 375
                  ExplicitWidth = 188
                  ExplicitHeight = 26
                end
              end
            end
            inherited TabSheet3: TTabSheet
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ExplicitLeft = 4
              ExplicitTop = 4
              ExplicitWidth = 376
              ExplicitHeight = 539
              inherited Splitter2: TSplitter
                Top = 313
                Width = 470
                Height = 10
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitTop = 313
                ExplicitWidth = 470
                ExplicitHeight = 10
              end
              inherited GroupBox: TGroupBox
                Width = 470
                Height = 313
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitWidth = 470
                ExplicitHeight = 313
                inherited OutputsStringGrid: TStringGrid
                  Left = 3
                  Top = 20
                  Width = 465
                  Height = 290
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 3
                  ExplicitTop = 20
                  ExplicitWidth = 465
                  ExplicitHeight = 290
                end
              end
              inherited GroupBox2: TGroupBox
                Top = 323
                Width = 470
                Height = 351
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitTop = 323
                ExplicitWidth = 470
                ExplicitHeight = 351
                inherited InputsStringGrid: TStringGrid
                  Left = 3
                  Top = 20
                  Width = 465
                  Height = 329
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 3
                  ExplicitTop = 20
                  ExplicitWidth = 465
                  ExplicitHeight = 329
                end
              end
            end
            inherited TabSheet1: TTabSheet
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              inherited ParametersHeaderControl: THeaderControl
                Top = 495
                Height = 44
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitTop = 630
                ExplicitWidth = 470
                ExplicitHeight = 44
              end
              inherited ParametersRichEdit: TRichEdit
                Height = 495
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Font.Height = -15
                ExplicitHeight = 495
              end
            end
            inherited TabSheet2: TTabSheet
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              inherited StateHeaderControl: THeaderControl
                Top = 495
                Height = 44
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitTop = 630
                ExplicitWidth = 470
                ExplicitHeight = 44
              end
              inherited StateRichEdit: TRichEdit
                Height = 495
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Font.Height = -15
                ExplicitHeight = 495
              end
            end
          end
        end
        inherited Panel2: TPanel
          Width = 325
          Height = 284
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alTop
          ExplicitWidth = 324
          ExplicitHeight = 284
          inherited StringGrid: TStringGrid
            Top = 28
            Width = 322
            Height = 228
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            DefaultRowHeight = 18
            OnMouseEnter = UComponentsListFrame1StringGridMouseEnter
            ExplicitTop = 28
            ExplicitWidth = 322
            ExplicitHeight = 228
          end
          inherited HeaderControl: THeaderControl
            Width = 322
            Height = 27
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            ExplicitWidth = 322
            ExplicitHeight = 27
          end
          inherited ClassNamePanel: TPanel
            Top = 256
            Width = 322
            Height = 27
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            ExplicitTop = 256
            ExplicitWidth = 322
            ExplicitHeight = 27
          end
        end
        inherited PopupMenu: TPopupMenu
          inherited GUI1: TMenuItem
            OnClick = UComponentsListFrame1GUI1Click
          end
        end
      end
    end
    object Panel4: TPanel
      Left = 1
      Top = 507
      Width = 1222
      Height = 168
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alBottom
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 1
      ExplicitTop = 490
      object PageControl2: TPageControl
        Left = 1
        Top = 1
        Width = 1220
        Height = 166
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        ActivePage = LogsTabSheet
        Align = alClient
        MultiLine = True
        TabOrder = 0
        TabPosition = tpRight
        ExplicitWidth = 1219
        ExplicitHeight = 165
        object LogsTabSheet: TTabSheet
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Logs'
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
        end
        object TabSheet2: TTabSheet
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'Performance'
          ImageIndex = 1
          inline UComponentsPerformanceFrame1: TUComponentsPerformanceFrame
            Left = 0
            Top = 0
            Width = 1188
            Height = 158
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Align = alClient
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -15
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
            ExplicitWidth = 1188
            ExplicitHeight = 158
            inherited PageControl1: TPageControl
              Width = 1007
              Height = 158
              ExplicitWidth = 1007
              ExplicitHeight = 158
              inherited TabSheet1: TTabSheet
                ExplicitLeft = 4
                ExplicitTop = 25
                ExplicitWidth = 794
                ExplicitHeight = 95
                inherited Chart: TChart
                  Width = 993
                  Height = 119
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitWidth = 993
                  ExplicitHeight = 119
                  PrintMargins = (
                    15
                    19
                    15
                    19)
                end
              end
              inherited TabSheet3: TTabSheet
                ExplicitLeft = 4
                ExplicitTop = 24
                ExplicitWidth = 446
                ExplicitHeight = 252
                inherited GroupBox2: TGroupBox
                  Width = 558
                  Height = 315
                  ExplicitWidth = 558
                  ExplicitHeight = 315
                  inherited ComponentsStringGrid: TStringGrid
                    Left = 3
                    Top = 20
                    Width = 552
                    Height = 293
                    ExplicitLeft = 3
                    ExplicitTop = 20
                    ExplicitWidth = 552
                    ExplicitHeight = 293
                  end
                end
              end
              inherited TabSheet2: TTabSheet
                ExplicitLeft = 4
                ExplicitTop = 24
                ExplicitWidth = 446
                ExplicitHeight = 252
                inherited GroupBox3: TGroupBox
                  Width = 558
                  Height = 315
                  ExplicitWidth = 558
                  ExplicitHeight = 315
                  inherited InterfacesStringGrid: TStringGrid
                    Left = 3
                    Top = 20
                    Width = 552
                    Height = 293
                    ExplicitLeft = 3
                    ExplicitTop = 20
                    ExplicitWidth = 552
                    ExplicitHeight = 293
                  end
                end
              end
            end
            inherited Panel1: TPanel
              Left = 1007
              Top = 0
              Width = 181
              Height = 158
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              Align = alRight
              ExplicitLeft = 1003
              ExplicitTop = 0
              ExplicitWidth = 181
              ExplicitHeight = 155
              inherited ShowModeRadioGroup: TRadioGroup
                Width = 179
                Height = 42
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Align = alTop
                ExplicitWidth = 179
                ExplicitHeight = 42
              end
              inherited GroupBox1: TGroupBox
                Left = 1
                Top = 43
                Width = 179
                Height = 111
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitLeft = 1
                ExplicitTop = 43
                ExplicitWidth = 179
                ExplicitHeight = 111
                inherited AverageIntervalSpinEdit: TSpinEdit
                  Left = 8
                  Top = 21
                  Width = 56
                  Height = 23
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 8
                  ExplicitTop = 21
                  ExplicitWidth = 56
                  ExplicitHeight = 23
                end
              end
            end
          end
        end
      end
    end
    object PageControl1: TPageControl
      Left = 336
      Top = 1
      Width = 887
      Height = 497
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      ActivePage = Draw
      Align = alClient
      DoubleBuffered = True
      ParentDoubleBuffered = False
      PopupMenu = PageControlPopupMenu
      TabOrder = 2
      OnChange = PageControl1Change
      ExplicitHeight = 480
      object Draw: TTabSheet
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Draw'
        OnShow = DrawShow
        inline UDrawEngineFrame1: TUDrawEngineFrame
          Left = 0
          Top = 0
          Width = 879
          Height = 465
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alClient
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -15
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          ExplicitWidth = 879
          ExplicitHeight = 465
          inherited Splitter1: TSplitter
            Left = 567
            Width = 15
            Height = 400
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            ExplicitLeft = 564
            ExplicitWidth = 15
            ExplicitHeight = 375
          end
          inherited Panel1: TPanel
            Left = 582
            Width = 297
            Height = 400
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            ExplicitLeft = 582
            ExplicitWidth = 297
            ExplicitHeight = 400
            inherited UClassesListFrame: TUClassesListFrame
              Width = 295
              Height = 398
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              Font.Height = -15
              ExplicitWidth = 295
              ExplicitHeight = 398
              inherited PageControl: TPageControl
                Width = 295
                Height = 398
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ActivePage = UDrawEngineFrame1.UClassesListFrame.LibsTabSheet
                ExplicitWidth = 295
                ExplicitHeight = 398
                inherited NameTabSheet: TTabSheet
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 4
                  ExplicitTop = 24
                  ExplicitWidth = 499
                  ExplicitHeight = 515
                  inherited StringGrid: TStringGrid
                    Width = 624
                    Height = 644
                    Margins.Left = 4
                    Margins.Top = 4
                    Margins.Right = 4
                    Margins.Bottom = 4
                    ExplicitWidth = 624
                    ExplicitHeight = 644
                  end
                end
                inherited LibsTabSheet: TTabSheet
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitTop = 29
                  ExplicitWidth = 287
                  ExplicitHeight = 365
                  inherited TreeView: TTreeView
                    Width = 287
                    Height = 365
                    Margins.Left = 4
                    Margins.Top = 4
                    Margins.Right = 4
                    Margins.Bottom = 4
                    ExplicitWidth = 287
                    ExplicitHeight = 365
                  end
                end
                inherited LibsControlTabSheet: TTabSheet
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  ExplicitLeft = 4
                  ExplicitTop = 24
                  inherited Splitter1: TSplitter
                    Top = 113
                    Width = 274
                    Height = 10
                    Margins.Left = 4
                    Margins.Top = 4
                    Margins.Right = 4
                    Margins.Bottom = 4
                    ExplicitTop = 113
                    ExplicitWidth = 274
                    ExplicitHeight = 10
                  end
                  inherited Panel1: TPanel
                    Top = 331
                    Width = 274
                    Height = 162
                    Margins.Left = 4
                    Margins.Top = 4
                    Margins.Right = 4
                    Margins.Bottom = 4
                    ExplicitTop = 331
                    ExplicitWidth = 274
                    ExplicitHeight = 162
                    inherited LoadLibraryButton: TButton
                      Width = 272
                      Height = 33
                      Margins.Left = 4
                      Margins.Top = 4
                      Margins.Right = 4
                      Margins.Bottom = 4
                      ExplicitWidth = 272
                      ExplicitHeight = 33
                    end
                    inherited CreateRuntimeLibraryButton: TButton
                      Top = 34
                      Width = 272
                      Height = 34
                      Margins.Left = 4
                      Margins.Top = 4
                      Margins.Right = 4
                      Margins.Bottom = 4
                      ExplicitTop = 34
                      ExplicitWidth = 272
                      ExplicitHeight = 34
                    end
                    inherited DeleteLibraryButton: TButton
                      Top = 68
                      Width = 272
                      Height = 31
                      Margins.Left = 4
                      Margins.Top = 4
                      Margins.Right = 4
                      Margins.Bottom = 4
                      ExplicitTop = 68
                      ExplicitWidth = 272
                      ExplicitHeight = 31
                    end
                    inherited RenameRuntimeLibraryButton: TButton
                      Top = 99
                      Width = 272
                      Height = 34
                      Margins.Left = 4
                      Margins.Top = 4
                      Margins.Right = 4
                      Margins.Bottom = 4
                      ExplicitTop = 99
                      ExplicitWidth = 272
                      ExplicitHeight = 34
                    end
                    inherited AddClassButton: TButton
                      Top = 133
                      Width = 272
                      Height = 32
                      Margins.Left = 4
                      Margins.Top = 4
                      Margins.Right = 4
                      Margins.Bottom = 4
                      ExplicitTop = 133
                      ExplicitWidth = 272
                      ExplicitHeight = 32
                    end
                  end
                  inherited GroupBox1: TGroupBox
                    Width = 274
                    Height = 113
                    Margins.Left = 4
                    Margins.Top = 4
                    Margins.Right = 4
                    Margins.Bottom = 4
                    ExplicitWidth = 274
                    ExplicitHeight = 113
                    inherited LibsListStringGrid: TStringGrid
                      Left = 3
                      Top = 20
                      Width = 268
                      Height = 90
                      Margins.Left = 4
                      Margins.Top = 4
                      Margins.Right = 4
                      Margins.Bottom = 4
                      ExplicitLeft = 3
                      ExplicitTop = 20
                      ExplicitWidth = 268
                      ExplicitHeight = 90
                    end
                  end
                  inherited GroupBox2: TGroupBox
                    Top = 123
                    Width = 274
                    Height = 208
                    Margins.Left = 4
                    Margins.Top = 4
                    Margins.Right = 4
                    Margins.Bottom = 4
                    ExplicitTop = 123
                    ExplicitWidth = 274
                    ExplicitHeight = 208
                    inherited LibComponentListStringGrid: TStringGrid
                      Left = 3
                      Top = 20
                      Width = 268
                      Height = 186
                      Margins.Left = 4
                      Margins.Top = 4
                      Margins.Right = 4
                      Margins.Bottom = 4
                      ExplicitLeft = 3
                      ExplicitTop = 20
                      ExplicitWidth = 268
                      ExplicitHeight = 186
                    end
                  end
                end
              end
            end
          end
          inherited Panel2: TPanel
            Width = 567
            Height = 400
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            ExplicitWidth = 564
            ExplicitHeight = 375
            inherited ScrollBox: TScrollBox
              Width = 565
              Height = 398
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ExplicitWidth = 562
              ExplicitHeight = 378
              inherited Image: TImage
                Left = -1
                Top = -3
                Width = 399
                Height = 316
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                ExplicitLeft = -1
                ExplicitTop = -3
                ExplicitWidth = 399
                ExplicitHeight = 316
              end
            end
          end
          inherited Panel3: TPanel
            Top = 400
            Width = 879
            Height = 65
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            ExplicitTop = 400
            ExplicitWidth = 879
            ExplicitHeight = 65
            inherited Label1: TLabel
              Left = 4
              Top = 8
              Width = 68
              Height = 18
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ExplicitLeft = 4
              ExplicitTop = 8
              ExplicitWidth = 68
              ExplicitHeight = 18
            end
            inherited Label2: TLabel
              Left = 199
              Top = 8
              Width = 58
              Height = 18
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ExplicitLeft = 199
              ExplicitTop = 8
              ExplicitWidth = 58
              ExplicitHeight = 18
            end
            inherited RectWidthLabeledEdit: TLabeledEdit
              Left = 283
              Top = 31
              Width = 53
              Height = 22
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              EditLabel.Width = 37
              EditLabel.Height = 18
              EditLabel.Margins.Left = 4
              EditLabel.Margins.Top = 4
              EditLabel.Margins.Right = 4
              EditLabel.Margins.Bottom = 4
              EditLabel.ExplicitLeft = 283
              EditLabel.ExplicitTop = 10
              EditLabel.ExplicitWidth = 37
              EditLabel.ExplicitHeight = 18
              ExplicitLeft = 283
              ExplicitTop = 31
              ExplicitWidth = 53
              ExplicitHeight = 22
            end
            inherited FontTypeComboBox: TComboBox
              Left = 4
              Top = 31
              Width = 186
              Height = 26
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ExplicitLeft = 4
              ExplicitTop = 31
              ExplicitWidth = 186
              ExplicitHeight = 26
            end
            inherited FontSizeComboBox: TComboBox
              Left = 199
              Top = 31
              Width = 74
              Height = 26
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ExplicitLeft = 199
              ExplicitTop = 31
              ExplicitWidth = 74
              ExplicitHeight = 26
            end
            inherited RectHeightLabeledEdit: TLabeledEdit
              Left = 344
              Top = 31
              Width = 54
              Height = 22
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              EditLabel.Width = 41
              EditLabel.Height = 18
              EditLabel.Margins.Left = 4
              EditLabel.Margins.Top = 4
              EditLabel.Margins.Right = 4
              EditLabel.Margins.Bottom = 4
              EditLabel.ExplicitLeft = 344
              EditLabel.ExplicitTop = 10
              EditLabel.ExplicitWidth = 41
              EditLabel.ExplicitHeight = 18
              ExplicitLeft = 344
              ExplicitTop = 31
              ExplicitWidth = 54
              ExplicitHeight = 22
            end
            inherited ApplyButton: TButton
              Left = 549
              Top = 19
              Width = 99
              Height = 31
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ExplicitLeft = 549
              ExplicitTop = 19
              ExplicitWidth = 99
              ExplicitHeight = 31
            end
            inherited RestoreButton: TButton
              Left = 655
              Top = 19
              Width = 99
              Height = 31
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ExplicitLeft = 655
              ExplicitTop = 19
              ExplicitWidth = 99
              ExplicitHeight = 31
            end
            inherited ShowLinkDetailCheckBox: TCheckBox
              Left = 406
              Top = 35
              Width = 134
              Height = 21
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ExplicitLeft = 406
              ExplicitTop = 35
              ExplicitWidth = 134
              ExplicitHeight = 21
            end
          end
          inherited PopupMenu: TPopupMenu
            inherited Reset1: TMenuItem
              OnClick = UDrawEngineFrame1Reset1Click
            end
            inherited Calculate1: TMenuItem
              OnClick = UDrawEngineFrame1Calculate1Click
            end
            inherited GUI1: TMenuItem
              OnClick = UDrawEngineFrame1GUI1Click
            end
          end
        end
      end
    end
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 744
    Width = 1343
    Height = 25
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Panels = <
      item
        Width = 50
      end
      item
        Width = 50
      end
      item
        Width = 50
      end
      item
        Width = 50
      end
      item
        Width = 50
      end>
    ExplicitTop = 738
    ExplicitWidth = 1074
  end
  object ChannelsStringGrid: TStringGrid
    Left = 0
    Top = 44
    Width = 119
    Height = 676
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alLeft
    ColCount = 3
    DefaultColWidth = 24
    DoubleBuffered = True
    FixedCols = 0
    FixedRows = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goDrawFocusSelected, goRowSelect]
    ParentDoubleBuffered = False
    PopupMenu = ChannelsPopupMenu
    ScrollBars = ssVertical
    TabOrder = 2
    Visible = False
    OnClick = ChannelsStringGridClick
    OnSelectCell = ChannelsStringGridSelectCell
    ExplicitTop = 55
    ExplicitHeight = 659
  end
  object ToolBar: TToolBar
    Left = 0
    Top = 0
    Width = 1343
    Height = 44
    AutoSize = True
    ButtonHeight = 44
    ButtonWidth = 104
    Caption = 'ToolBar'
    Color = clBtnFace
    DoubleBuffered = True
    Images = ImageList
    ParentColor = False
    ParentDoubleBuffered = False
    ShowCaptions = True
    TabOrder = 3
    ExplicitTop = -3
    ExplicitWidth = 1074
    object ToolButton17: TToolButton
      Left = 0
      Top = 0
      AutoSize = True
      Caption = 'Create Project'
      ImageIndex = 21
      MenuItem = CreateProjectItem
    end
    object ToolButton1: TToolButton
      Left = 81
      Top = 0
      AutoSize = True
      Caption = 'Load Project'
      ImageIndex = 0
      MenuItem = LoadProjectItem
    end
    object ToolButton2: TToolButton
      Left = 152
      Top = 0
      AutoSize = True
      Caption = 'Save Project'
      ImageIndex = 1
      MenuItem = SaveProjectItem
    end
    object ToolButton3: TToolButton
      Left = 224
      Top = 0
      Width = 8
      Caption = 'ToolButton3'
      ImageIndex = 2
      Style = tbsSeparator
    end
    object ToolButton19: TToolButton
      Left = 232
      Top = 0
      AutoSize = True
      Caption = 'Reload Parameters'
      ImageIndex = 22
      MenuItem = ReloadParameters1
    end
    object ToolButton14: TToolButton
      Left = 334
      Top = 0
      Width = 8
      Caption = 'ToolButton14'
      ImageIndex = 10
      Style = tbsSeparator
    end
    object ToolButton10: TToolButton
      Left = 342
      Top = 0
      AutoSize = True
      Caption = 'Start'
      ImageIndex = 7
      MenuItem = Start1
    end
    object ToolButton12: TToolButton
      Left = 377
      Top = 0
      AutoSize = True
      Caption = 'Pause'
      ImageIndex = 8
      MenuItem = Pause1
    end
    object ToolButton13: TToolButton
      Left = 417
      Top = 0
      AutoSize = True
      Caption = 'Reset'
      ImageIndex = 9
      MenuItem = Reset1
    end
    object ToolButton16: TToolButton
      Left = 456
      Top = 0
      AutoSize = True
      Caption = 'Step'
      ImageIndex = 20
      MenuItem = Step1
    end
    object ToolButton21: TToolButton
      Left = 489
      Top = 0
      Width = 8
      Caption = 'ToolButton21'
      ImageIndex = 21
      Style = tbsSeparator
    end
    object ToolButton15: TToolButton
      Left = 497
      Top = 0
      AutoSize = True
      Caption = 'Window'
      ImageIndex = 10
      MenuItem = Window1
      Style = tbsDropDown
      OnClick = ToolButton15Click
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 720
    Width = 1343
    Height = 24
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alBottom
    TabOrder = 4
    ExplicitTop = 714
    object ShowDebugMessagesCheckBox: TCheckBox
      Left = 1
      Top = 1
      Width = 170
      Height = 22
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alLeft
      Caption = 'Show debug messages'
      TabOrder = 0
      OnClick = ShowDebugMessagesCheckBoxClick
    end
    object AutoupdatePropertiesCheckBox: TCheckBox
      Left = 271
      Top = 1
      Width = 180
      Height = 22
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alLeft
      Caption = 'Autoupdate properties'
      TabOrder = 1
      OnClick = AutoupdatePropertiesCheckBoxClick
    end
    object AutosaveStatesCheckBox: TCheckBox
      Left = 1080
      Top = 1
      Width = 130
      Height = 22
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alRight
      Caption = 'Autosave states'
      TabOrder = 2
      OnClick = AutosaveStatesCheckBoxClick
    end
    object AutosaveProjectCheckBox: TCheckBox
      Left = 1210
      Top = 1
      Width = 131
      Height = 22
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alRight
      Caption = 'Autosave project'
      TabOrder = 3
      OnClick = AutosaveProjectCheckBoxClick
    end
    object MultiThreadedRadioButton: TRadioButton
      Left = 950
      Top = 1
      Width = 130
      Height = 22
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alRight
      Caption = 'Multi-threaded'
      TabOrder = 4
      OnClick = MultiThreadedRadioButtonClick
    end
    object SingleThreadedRadioButton: TRadioButton
      Left = 815
      Top = 1
      Width = 135
      Height = 22
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alRight
      Caption = 'Single-threaded'
      TabOrder = 5
      OnClick = SingleThreadedRadioButtonClick
    end
    object DetailedDebugLogCheckBox: TCheckBox
      Left = 171
      Top = 1
      Width = 100
      Height = 22
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alLeft
      Caption = 'Detailed log'
      TabOrder = 6
      OnClick = DetailedDebugLogCheckBoxClick
    end
  end
  object MainMenu: TMainMenu
    Images = ImageList
    Left = 312
    Top = 65528
    object File1: TMenuItem
      Caption = 'File'
      object CreateProjectItem: TMenuItem
        Caption = 'Create Project'
        ImageIndex = 21
        OnClick = CreateProjectItemClick
      end
      object LoadProjectItem: TMenuItem
        Caption = 'Load Project'
        ImageIndex = 0
        OnClick = LoadProjectItemClick
      end
      object SaveProjectItem: TMenuItem
        Caption = 'Save Project'
        ImageIndex = 1
        OnClick = SaveProjectItemClick
      end
      object CopyProject1: TMenuItem
        Caption = 'Copy Project'
        OnClick = CopyProject1Click
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object ProjectOptions1: TMenuItem
        Caption = 'Project Options'
        OnClick = ProjectOptions1Click
      end
      object ApplicationOptions1: TMenuItem
        Caption = 'Application Options'
        OnClick = ApplicationOptions1Click
      end
      object ClassesDescription1: TMenuItem
        Caption = 'Classes Description'
        OnClick = ClassesDescription1Click
      end
      object OpenProjectFolder1: TMenuItem
        Caption = 'Open Project Folder'
        OnClick = OpenProjectFolder1Click
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object CreateModel: TMenuItem
        Caption = 'Create Model'
        ImageIndex = 24
        OnClick = CreateModelClick
      end
      object LoadModel1: TMenuItem
        Caption = 'Load Model'
        ImageIndex = 2
        OnClick = LoadModel1Click
      end
      object SaveModel1: TMenuItem
        Caption = 'Save Model'
        ImageIndex = 3
        OnClick = SaveModel1Click
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object ReloadParameters1: TMenuItem
        Caption = 'Reload Parameters'
        ImageIndex = 22
        OnClick = ReloadParameters1Click
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object CaptureVideo1: TMenuItem
        Caption = 'Capture Video'
        ImageIndex = 4
        OnClick = CaptureVideo1Click
      end
      object OpenVideo1: TMenuItem
        Caption = 'Open Video File'
        ImageIndex = 5
        OnClick = OpenVideo1Click
      end
      object OpenImage1: TMenuItem
        Caption = 'Open Image'
        ImageIndex = 6
        OnClick = OpenImage1Click
      end
      object N9: TMenuItem
        Caption = '-'
      end
      object SaveCommonDescriptions1: TMenuItem
        Caption = 'Save Common Descriptions'
        OnClick = SaveCommonDescriptions1Click
      end
      object SaveClassesDescriptions1: TMenuItem
        Caption = 'Save Classes Descriptions'
        OnClick = SaveClassesDescriptions1Click
      end
    end
    object Channels1: TMenuItem
      Caption = 'Channels'
      object AddNew1: TMenuItem
        Caption = 'Add New'
        OnClick = AddNew1Click
      end
      object Insert1: TMenuItem
        Caption = 'Insert'
        OnClick = Insert1Click
      end
      object DeleteLast1: TMenuItem
        Caption = 'Delete Last'
        OnClick = DeleteLast1Click
      end
      object DeleteAll1: TMenuItem
        Caption = 'Delete All'
        OnClick = DeleteAll1Click
      end
      object DeleteSelected1: TMenuItem
        Caption = 'Delete Selected'
        OnClick = DeleteSelected1Click
      end
      object ClonetoNewChannel1: TMenuItem
        Caption = 'Clone to New Channel'
        OnClick = ClonetoNewChannel1Click
      end
    end
    object Calculate1: TMenuItem
      Caption = 'Calculate'
      object Start1: TMenuItem
        Caption = 'Start'
        ImageIndex = 7
        OnClick = Start1Click
      end
      object Pause1: TMenuItem
        Caption = 'Pause'
        ImageIndex = 8
        OnClick = Pause1Click
      end
      object Reset1: TMenuItem
        Caption = 'Reset'
        ImageIndex = 9
        OnClick = Reset1Click
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object Step1: TMenuItem
        Caption = 'Step'
        ImageIndex = 20
        OnClick = Step1Click
      end
    end
    object Window1: TMenuItem
      Caption = 'Window'
      ImageIndex = 10
      object Images1: TMenuItem
        Caption = 'Images'
        OnClick = Images1Click
      end
      object EngineMonitor1: TMenuItem
        Caption = 'Engine Monitor'
        OnClick = EngineMonitor1Click
      end
      object VideoSource1: TMenuItem
        Caption = 'Video Sources'
        OnClick = VideoSource1Click
      end
      object VideoRegistration1: TMenuItem
        Caption = 'Video Registration'
        OnClick = VideoRegistration1Click
      end
      object ComponentsControl1: TMenuItem
        Caption = 'Components Control'
        OnClick = ComponentsControl1Click
      end
      object ComponentsLinks1: TMenuItem
        Caption = 'Components Links'
        OnClick = ComponentsLinks1Click
      end
      object Performance1: TMenuItem
        Caption = 'Performance'
        OnClick = Performance1Click
      end
      object FavoriteInformation1: TMenuItem
        Caption = 'Favorite Information'
        OnClick = FavoriteInformation1Click
      end
      object DrawEngine1: TMenuItem
        Caption = 'Draw Engine'
        OnClick = DrawEngine1Click
      end
      object WatchWindow1: TMenuItem
        Caption = 'Watch Window'
        OnClick = WatchWindow1Click
      end
      object N6: TMenuItem
        Caption = '-'
      end
      object Broadcasters1: TMenuItem
        Caption = 'Broadcasters'
      end
      object Servercontrol1: TMenuItem
        Caption = 'Server control'
        OnClick = Servercontrol1Click
      end
    end
    object New1: TMenuItem
      Caption = 'New Window'
      object Images2: TMenuItem
        Caption = 'Images'
        OnClick = Images2Click
      end
      object Watches1: TMenuItem
        Caption = 'Watches'
        OnClick = Watches1Click
      end
    end
  end
  object ImageList: TImageList
    Left = 360
    Top = 65528
    Bitmap = {
      494C01011B005C032C0410001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000400000007000000001002000000000000070
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000080808000808080008080
      8000808080008080800080808000808080008080800080808000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000008080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF0000FFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF0000FFFF000000000000FFFF00FFFFFF000000
      0000FFFFFF000000000000000000000000000000000000000000000000008080
      8000000000000000000000000000808080000000000000FFFF00808080000000
      0000000000000000000000000000000000000000000000000000000000008080
      8000000000000000000000000000808080000000000000FFFF00808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000000000FFFF
      FF0000FFFF000000000000000000000000000000000000000000000000008080
      800080808000808080008080800080808000000000000000000000FFFF000000
      0000000000000000000000000000000000000000000000000000000000008080
      800080808000808080008080800080808000000000000000000000FFFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000FFFF00FFFFFF0000FFFF00FFFFFF0000000000FFFFFF0000FF
      FF0000000000000000000000000000000000000000000000000080808000FFFF
      FF00C0C0C000FFFFFF00C0C0C000FFFFFF008080800000000000000000000000
      000000000000000000000000000000000000000000000000000080808000C0C0
      C000FFFFFF00C0C0C000FFFFFF00C0C0C0008080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF000000
      0000008080000000000000000000000000008080800080808000FFFFFF00C0C0
      C000FFFFFF000000FF00FFFFFF00C0C0C000FFFFFF0080808000808080000000
      0000000000000000000000000000000000008080800080808000C0C0C000FFFF
      FF00C0C0C000FFFFFF00C0C0C000FFFFFF00C0C0C00080808000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      00000000000000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00000000000000
      0000008080000000000000000000000000000000000080808000C0C0C000FFFF
      FF00C0C0C0000000FF00C0C0C000FFFFFF00C0C0C00080808000000000000000
      0000000000000000000000000000000000000000000080808000FFFFFF00C0C0
      C000FFFFFF00C0C0C000FFFFFF00C0C0C000FFFFFF0080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFF0000FFFF
      0000FFFF00000000000000FFFF00FFFFFF0000FFFF000000000000FFFF000000
      0000008080000000000000000000000000000000000080808000FFFFFF000000
      FF000000FF000000FF000000FF000000FF00FFFFFF0080808000000000000000
      0000000000000000000000000000000000000000000080808000C0C0C0000000
      FF000000FF000000FF000000FF000000FF00C0C0C00080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      0000FFFF0000FFFF00000000000000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000008080000000000000000000000000000000000080808000C0C0C000FFFF
      FF00C0C0C0000000FF00C0C0C000FFFFFF00C0C0C00080808000000000000000
      0000000000000000000000000000000000000000000080808000FFFFFF00C0C0
      C000FFFFFF00C0C0C000FFFFFF00C0C0C000FFFFFF0080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFF0000FFFF0000FFFF0000000000000000000000000000000000000000
      0000008080000000000000000000000000008080800080808000FFFFFF00C0C0
      C000FFFFFF000000FF00FFFFFF00C0C0C000FFFFFF0080808000808080000000
      0000000000000000000000000000000000008080800080808000C0C0C000FFFF
      FF00C0C0C000FFFFFF00C0C0C000FFFFFF00C0C0C00080808000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFF0000000000000000000000000000000000000000000000FF
      FF0000FFFF00000000000000000000000000000000000000000080808000FFFF
      FF00C0C0C000FFFFFF00C0C0C000FFFFFF008080800000000000000000000000
      000000000000000000000000000000000000000000000000000080808000C0C0
      C000FFFFFF00C0C0C000FFFFFF00C0C0C0008080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000008080000000000000000000000000000000000000000000000000008080
      8000808080008080800080808000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008080
      8000808080008080800080808000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF0000000000000000000000000000000000000000008080
      8000000000000000000000000000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008080
      8000000000000000000000000000808080000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000FFFFFF000000000000000000000000000000
      0000000000000000000000000000800000000000000000000000000000000000
      0000000000000000000080000000000000000000000000000000000000000000
      000000000000C0C0C000C0C0C000808080008080800080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      FF000000FF000000FF0000000000FFFFFF000000000000000000000000008000
      0000000000000000000000000000000000008000000000000000000000000000
      000080000000000000000000000000000000000000000000000000000000C0C0
      C000C0C0C0008080800000000000000000000000000080808000808080008080
      800000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      FF000000FF000000FF0000000000FFFFFF000000000080000000800000000000
      0000000000000000000080000000000000000000000080000000000000000000
      0000000000008000000000000000000000000000000000000000C0C0C000C0C0
      C00000000000FFFFFF0000FF0000FFFFFF0000FF0000FFFFFF00000000008080
      800080808000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      FF000000FF000000FF0000000000FFFFFF000000000000000000000000008000
      0000800000000000000000000000000000000000000000000000800000000000
      0000000000008000000080000000000000000000000000000000C0C0C0000000
      0000FFFFFF000000000000000000000000000000000000000000FFFFFF000000
      000080808000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000
      0000000000000000000000000000FFFFFF000000000000000000000000008000
      0000000000000000000080000000800000000000000000000000000000000000
      00008000000080000000000000000000000000000000C0C0C00080808000FFFF
      FF00000000000000000000FF000000800000008000000000000000000000FFFF
      FF0080808000808080000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000000000008000
      0000000000000000000000000000000000000000000000000000000000008000
      00008000000000000000000000000000000000000000C0C0C0000000000000FF
      00000000000000FF00000080000000FF000000800000008000000000000000FF
      000000000000808080000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF00FFFFFF00FFFFFF000000000000000000800000000000
      0000000000000000000080000000800000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000000000FFFF
      FF000000000000FF000000FF000000FF000000FF00000080000000000000FFFF
      FF0000000000808080000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF000000
      00000000000000000000FFFFFF00FFFFFF008000000080000000000000000000
      0000000000000000000000000000000000000000000080000000000000000000
      00000000000000000000000000000000000000000000FFFFFF000000000000FF
      000000000000FFFFFF0000FF000000FF00000080000000FF00000000000000FF
      000000000000C0C0C0000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      0000000000000000000000000000FFFFFF000000000000000000000000008000
      0000000000000000000000000000000000008000000000000000000000008000
      00000000000000000000000000000000000000000000FFFFFF0080808000FFFF
      FF000000000000000000FFFFFF00FFFFFF0000FF00000000000000000000FFFF
      FF0080808000C0C0C0000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000080000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000C0C0C0000000
      0000FFFFFF000000000000000000000000000000000000000000FFFFFF000000
      0000C0C0C000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFF0000FFFF
      0000FFFF00000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00FFFFFF000000000080000000000000000000
      0000000000000000000000000000000000000000000080000000000000000000
      0000000000008000000000000000000000000000000000000000FFFFFF00C0C0
      C00000000000FFFFFF0000FF0000FFFFFF0000FF0000FFFFFF0000000000C0C0
      C000C0C0C000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF000000
      0000000000000000000000000000000000000000000000000000FFFF0000FFFF
      0000FFFF00000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00FFFFFF000000000000000000800000000000
      0000000000008000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00C0C0C0008080800000000000000000000000000080808000C0C0C000C0C0
      C00000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000FFFF0000FFFF
      0000FFFF00000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00FFFFFF000000000080000000000000008000
      0000800000000000000000000000000000000000000000000000000000008000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF00FFFFFF00FFFFFF00C0C0C000C0C0C000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF00FFFFFF000000000000000000000000008000
      0000000000000000000000000000000000008000000000000000000000000000
      0000000000008000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF000080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000C0C0C000C0C0C000808080008080800080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF000080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00000000000000000000000000000000000000000000000000C0C0
      C000C0C0C0008080800000000000000000000000000080808000808080008080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF000000000000000000FFFFFF000000
      00000000000000000000FFFFFF0000000000FFFFFF0000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000C0C0C000C0C0
      C00000000000FFFFFF000000FF00FFFFFF000000FF00FFFFFF00000000008080
      8000808080000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF000080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000C0C0C0000000
      0000FFFFFF000000000000000000000000000000000000000000FFFFFF000000
      0000808080000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF000080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF00000000000000000000000000FFFFFF0000000000C0C0C000000000000000
      0000FFFFFF0000000000FFFFFF0000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0000000000000000000000000000000000C0C0C00080808000FFFF
      FF0000000000000000000000FF0000008000000080000000000000000000FFFF
      FF00808080008080800000000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF000080800000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF000000FF000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF0000000000FFFFFF00FFFF
      FF0080000000800000008000000080000000FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0000000000000000000000000000000000C0C0C000000000000000
      FF00000000000000FF00000080000000FF000000800000008000000000000000
      FF00000000008080800000000000000000000000000000000000000000000000
      000000000000000000000000000000FFFF0000FFFF0000808000000000000000
      000000000000000000000000000000000000000000000000FF000000FF000000
      FF000000FF000000FF0000000000FFFFFF000000000000000000FFFFFF000000
      000000000000000000000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0000000000000000000000000000000000FFFFFF0000000000FFFF
      FF00000000000000FF000000FF000000FF000000FF000000800000000000FFFF
      FF00000000008080800000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000FFFF0000FFFF00008080000000
      000000000000000000000000000000000000000000000000FF000000FF000000
      FF000000FF000000FF000000FF0000000000FFFFFF00FFFFFF00FFFFFF000000
      0000FFFFFF00FFFFFF000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF0080000000800000008000000080000000FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0000000000000000000000000000000000FFFFFF00000000000000
      FF0000000000FFFFFF000000FF000000FF00000080000000FF00000000000000
      FF0000000000C0C0C00000000000000000000000000000000000000000000000
      000000FFFF000080800000000000000000000000000000FFFF0000FFFF000080
      800000000000000000000000000000000000000000000000FF000000FF000000
      FF000000FF000000FF000000FF000000FF0000000000C0C0C000FFFFFF000000
      0000FFFFFF00000000000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFF
      FF00FFFFFF0000000000000000000000000000000000FFFFFF0080808000FFFF
      FF000000000000000000FFFFFF00FFFFFF000000FF000000000000000000FFFF
      FF0080808000C0C0C00000000000000000000000000000000000000000000000
      000000FFFF00008080000000000000000000000000000000000000FFFF000080
      800000000000000000000000000000000000000000000000FF000000FF000000
      FF000000FF000000FF000000FF0000000000FFFFFF00FFFFFF00FFFFFF000000
      000000000000000000000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF00800000008000000080000000FFFFFF00000000000000000000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000C0C0C0000000
      0000FFFFFF000000000000000000000000000000000000000000FFFFFF000000
      0000C0C0C0000000000000000000000000000000000000000000000000000000
      000000FFFF0000FFFF0000808000000000000000000000FFFF0000FFFF000080
      800000000000000000000000000000000000000000000000FF000000FF000000
      FF000000FF000000FF0000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000C0C0C00000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000FFFFFF00C0C0
      C00000000000FFFFFF000000FF00FFFFFF000000FF00FFFFFF0000000000C0C0
      C000C0C0C0000000000000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF0000FFFF0000FFFF0000FFFF00008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF000000FF000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF0000000000FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000000000000000FFFFFF00FFFF
      FF00FFFFFF00000000000000000000000000000000000000000000000000FFFF
      FF00C0C0C0008080800000000000000000000000000080808000C0C0C000C0C0
      C000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF00000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF0000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF00FFFFFF00FFFFFF00C0C0C000C0C0C000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00006633660066336600663366006666660066336600663366005F5F5F006633
      66005F5F5F006633660066336600666666000000000000000000000000000000
      8000000000000000000000000000000000000000000000000000000000000000
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000066336600663366005F5F5F00663366005F5F5F0066336600663366005F5F
      5F00663366006633660066336600666666000000000000000000000080000000
      000000000000000000000000FF00000000000000FF0000000000000000000000
      0000000080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000000000000000
      0000000000006633660066336600663366006633660066336600663366006633
      66005F5F5F00663366005F5F5F00663366000000000000000000000000000000
      00000000FF00C0C0C000FFFFFF0000000000FFFFFF00C0C0C0000000FF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000000000000000
      0000000000006633660000000000663366006633660066336600663366005F5F
      5F005F5F5F005F5F5F00663366005F5F5F000000000080808000000000000000
      FF00C0C0C00000000000FFFFFF00FFFFFF00FFFFFF0000000000C0C0C0000000
      FF0000000000808080000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000000000000000
      0000000000000000000066336600663366006633660066336600663366006633
      660066336600663366005F5F5F005F5F5F0000000000000000000000FF000000
      FF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
      FF000000FF00000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000006633660066336600663366006633
      66006633660066666600666666005F5F5F0000000000000000000000FF000000
      0000FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00000000000000
      00000000FF00000000000000000000000000FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FFFFFF000000000000000000000000000000000000000000008080000080
      8000008080000080800000808000008080000080800000808000008080000080
      8000008080000080800000000000000000000000000000000000000000000000
      0000000000000000000000000000663366006633660000000000663366006633
      660066336600663366005F5F5F006633660000000000000000000000FF000000
      FF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF000000
      FF000000FF0000000000000000000000000000000000C0C0C000C0C0C000C0C0
      C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C0000000
      0000FFFFFF000000000000000000000000000000000000000000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      0000008080000080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000066336600663366006633
      6600663366006633660066336600663366000000000080808000000000000000
      FF00C0C0C00000000000FFFFFF0000000000FFFFFF0000000000C0C0C0000000
      FF00000000008080800000000000000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000FFFFFF000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000663366006633
      6600000000006633660066336600663366000000000000000000000000000000
      00000000FF00C0C0C000FFFFFF0000000000FFFFFF00C0C0C0000000FF000000
      00000000000000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF000000
      0000FFFFFF000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000663366006633660066336600663366000000000000000000000000000000
      0000000000000000FF000000FF000000FF000000FF000000FF00000000000000
      0000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000FFFFFF000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00006633660000000000000000006633660000000000C0C0C000C0C0C0008080
      800000000000000000000000000000000000000000000000000000000000C0C0
      C0008080800080808000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF000000
      0000FFFFFF000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000663366006633660000000000C0C0C000C0C0C000C0C0
      C000C0C0C00080808000808080000000000080808000C0C0C000C0C0C000C0C0
      C000C0C0C0008080800000000000000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF000000
      0000FFFFFF000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000005F5F5F0066336600000000000000
      0000000000006633660066336600663366000000000000000000000000000000
      000000000000000000006633660000000000000000000000000080808000C0C0
      C000C0C0C0000000000080808000000000008080800000000000C0C0C000C0C0
      C000808080000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF000000000000000000000000000000000000000000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000006666660066666600666666006666
      6600666666009966660066666600666666005F5F5F005F5F5F005F5F5F006633
      6600663366000000000000000000000000000000000000000000808080000000
      0000000000000000000000000000000000000000000000000000000000000000
      000080808000000000000000000000000000FFFFFF000000000000FFFF00FFFF
      FF0000FFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000007777770077777700777777007777
      7700777777007777770066666600666666009966660066666600666666005F5F
      5F00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00000000000000
      00000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000008080800080808000808080008080
      80007777770077777700996666006666660066666600666666005F5F5F006633
      6600663366006633660066336600663366000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000008080
      8000808080008080800000000000000000000000000000000000808080008080
      8000808080000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000FF000000FF000000FF000000FF000000FF00000000000000
      0000000000000000000000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      000080000000808080000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF0000000000000000008080800080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF000000
      FF00000000000000000000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF0000000000000000000000000000000000000000000000
      00000000FF000000FF000000FF000000000000FFFF0000FFFF0000FFFF000000
      0000FF000000FF000000FF0000000000000000000000000000000000FF000000
      FF000000000000000000808080000000000080808000000000000000FF000000
      FF000000FF000000000000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000080808000000000000000
      00000000FF000000FF000000FF000000000000FFFF0000FFFF0000FFFF000000
      0000FF000000FF000000FF00000000000000000000000000FF000000FF000000
      FF000000FF000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF0000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000000000000000
      00000000FF000000FF000000FF000000000000FFFF0000FFFF0000FFFF000000
      0000FF000000FF000000FF00000000000000000000000000FF00000000000000
      FF000000FF000000FF0080808000000000008080800000000000000000000000
      00000000FF000000FF0000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FF000000FF000000FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000008080800080808000000000008080
      80000000FF000000FF000000FF008080800000FFFF0000FFFF0000FFFF008080
      8000FF000000FF000000FF000000808080000000FF000000FF00000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000000000000000FF000000FF00000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FF000000FFFFFF00FFFFFF00FF000000FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000FF000000FF000000FF000000000000000000FF000000FF00000000000000
      0000000000000000FF000000FF00000000000000000000000000000000000000
      0000000000000000FF000000FF00000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FF000000FFFFFF00FFFFFF00FF000000FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000080808000000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000FF000000FF000000FF000000000000000000FF000000FF00000000000000
      0000000000000000000080808000000000008080800000000000000000000000
      0000000000000000FF000000FF00000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FF000000FF000000FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000000000000000
      00000000FF000000FF000000FF00000000000000000000000000000000000000
      0000FF000000FF000000FF000000000000000000FF000000FF00000000000000
      000000000000000000000000800000000000000080000000FF00000000000000
      0000000000000000FF000000FF00000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000008080800080808000000000008080
      8000000000000000000000000000808080008080800080808000808080008080
      8000FF000000FF000000FF000000808080000000FF000000FF00000000000000
      000000000000000000000000000000000000000000000000FF000000FF000000
      0000000000000000FF000000FF00000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000FF000000FF000000FF00000000000000000000000000FF000000FF000000
      000000000000000000000000000000000000000000000000FF000000FF000000
      FF00000000000000FF0000000000000000000000000000000000800000008000
      0000800000008000000080808000000000000000000080000000800000008000
      0000800000008080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000FFFFFF0000000000000000000000000080808000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000FF000000FF000000
      FF000000000000000000000000000000000000000000000000000000FF000000
      FF000000FF000000FF0000000000000000000000000000000000800000008000
      0000800000008000000000000000000000000000000080000000800000008000
      0000800000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000FF000000
      FF000000FF000000000080808000000000008080800000000000000000000000
      FF000000FF000000000000000000000000000000000000000000800000008000
      0000800000000000000000000000000000000000000080000000800000008000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000008080800080808000000000008080
      8000808080008080800080808000808080008080800080808000808080008080
      8000808080008080800080808000808080000000000000000000000000000000
      FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF000000
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000FF000000FF000000FF000000FF000000FF00000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000FF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF000000000000000000000000000000000000000000000000000000000000FF
      FF00FFFFFF0000FFFF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000FF000000
      FF000000000000000000000000000000000000000000000000000000000000FF
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000000000000000000000000000000000000000000000000000000000FFFF
      FF0000FFFF00FFFFFF0000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF0000FFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000FF000000FF000000FF000000FF000000FF000000
      FF000000FF000000000000000000000000000000000000000000000000000000
      000000FF00000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF00000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00000000000000FF000000FF000000FF000000FF000000FF000000
      FF000000FF000000FF00000000000000000000000000000000000000000000FF
      00000000000000FF000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00000000000000FF000000FF000000FF000000FF000000FF000000
      FF000000FF000000FF000000FF00000000000000000000000000000000000000
      000000FF00000000000000FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFF00000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      0000FFFFFF00000000000000FF000000FF000000FF000000FF000000FF000000
      FF000000FF000000FF00000000000000000000000000000000000000000000FF
      00000000000000FF00000000000000FF00000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFF000000000000FFFF000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00000000000000FF000000FF000000FF000000FF000000FF000000
      FF000000FF000000000000000000000000000000000000000000000000000000
      000000FF00000000000000FF00000000000000FF000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFF00000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF0000FFFF000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      0000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000FF000000
      FF000000000000000000000000000000000000000000000000000000000000FF
      00000000000000FF00000000000000FF00000000000000FF0000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000FFFF0000FFFF00000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00000000000000FF000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FF00000000000000FF00000000000000FF000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00000000000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000000000FF
      00000000000000FF00000000000000FF00000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0000000000FFFFFF00FFFFFF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FF00000000000000FF0000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF0000FFFF00FFFFFF0000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000000000C0C0
      C000FFFFFF0000000000FFFFFF00000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000000000FF
      00000000000000FF000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000FFFF00FFFFFF0000FFFF0000000000000000000000
      0000000000000000000000000000000000000000000000FFFF0000FFFF0000FF
      FF00000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000FF00000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF0000FFFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000000000000000000FF
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000808080000000
      0000808080008080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000008080800080808000808080008080800080808000808080008080
      8000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000C0C0C0000000
      0000C0C0C000C0C0C00000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000080808000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C00000000000000000000000000000000000000000000000000000FFFF00C0C0
      C00000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FFFF000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000C0C0C000C0C0
      C000C0C0C000C0C0C00000000000000000000000000000000000008080000080
      8000008080000080800000808000008080000080800000808000008080000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000080808000C0C0C0000000000000000000C0C0C000C0C0C000C0C0
      C000000000000000000000000000000000000000000000FFFF000000000000FF
      FF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FF
      FF0000000000FFFFFF00FFFFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF00000000000080
      8000008080000080800000808000008080000080800000808000008080000080
      8000000000000000000000000000000000000000000000000000000000000000
      00000000000080808000C0C0C0000000000000000000C0C0C000C0C0C000C0C0
      C0000000000000000000000000000000000000000000FFFFFF0000FFFF000000
      000000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0C00000FFFF00C0C0
      C00000FFFF0000000000FFFFFF00000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000FFFF000000
      0000008080000080800000808000008080000080800000808000008080000080
      8000008080000000000000000000000000000000000000000000000000000000
      00000000000080808000C0C0C000C0C0C000C0C0C000C0C0C000C0C0C000C0C0
      C000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF00000000000000000000000000000000000000000000000000000000000000
      00000000000000000000FFFFFF000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00000000000000000000FFFF00FFFFFF0000FF
      FF00000000000080800000808000008080000080800000808000008080000080
      8000008080000080800000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF0000000000FFFFFF000000000000000000000000000000
      00000000000000000000FFFFFF000000000000000000FFFFFF00000000000000
      0000FFFFFF000000000000000000C0C0C00000000000FF000000FF000000FF00
      00000000FF00FF000000FF000000000000000000000000FFFF00FFFFFF0000FF
      FF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF000000000000000000000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF0000000000000000000000
      FF000000FF000000FF00000000000000000000000000FFFFFF0000FFFF00FFFF
      FF0000FFFF00FFFFFF0000FFFF00FFFFFF0000FFFF00FFFFFF00000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000FFFFFF0000FF
      FF00FFFFFF0000FFFF0000000000FFFFFF000000000000000000000000000000
      00000000000000000000FFFFFF000000000000000000FFFFFF00000000000000
      00000000000000000000FFFFFF0000000000FFFFFF00000000000000FF000000
      FF000000FF000000FF000000FF00000000000000000000FFFF00FFFFFF0000FF
      FF00000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000080808000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000FF000000FF000000
      FF000000FF000000FF000000FF000000FF000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF000000000000000000FFFFFF00FFFF
      FF000000000000000000000000000000000000000000FFFFFF00000000000000
      0000FFFFFF000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF0000000000FFFFFF00000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF0000000000FFFFFF00FFFFFF000000000000000000000000000000
      FF000000FF000000FF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000008080800000000000000000000000000000000000000000000000
      0000000000000000000000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF000000000000000000000000000000000000000000FFFFFF0000000000C0C0
      C000FFFFFF0000000000FFFFFF00000000000000000000000000808080000000
      FF000000FF000000FF0000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000000000000FFFF
      FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000FF000000FF000000FF000000
      FF000000FF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      FF000000FF000000FF000000FF000000FF000000FF000000FF000000FF000000
      FF00000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000040000000700000000100010000000000800300000000000000000000
      000000000000000000000000FFFFFF00FFFFFFFDFFFD00008017FFF8FFF80000
      FFF7FFF1FFF10000FE63FFE3FFE30000FC03FFC7FFC70000F803E08FE08F0000
      F003C01FC01F0000F003803F803F0000E003001F001F0000C003001F001F0000
      8003001F001F00000003001F001F00000003001F001F000001E3803F803F0000
      83E3C07FC07F0000C7E3E0FFE0FF0000FFFFFFFF8000FFFFF83FFFFF8000FEFD
      E00FC007C000EF77C007C007E0009DBB8003C007F000E7D98003C007F800ECF3
      0001C007FC00EFE70001C007FE00DCFF0001C007FF003FBF0001C007FF80EF6F
      0001C0078380FDFF8003C00783E0BFBB8003C00F83E0DBFFC007C01F83E0A7EF
      E00FC03F8384EF7BF83FFFFFFFFEFFEFFFFFFFFFFFFFFFFFFE7FFC00FFFFF83F
      FC3FFC00FFFFE00FFC3FFC000007C007FE7FFC007FE78003FC3FEC0040278003
      FC3FE40040270001FC3FE00040270001FC1F000040270001F20F000140270001
      E107000340270001E187000740E78003E007000F40A78003F00FE3FF40C7C007
      F81FE7FF7F87E00FFFFFEFFF0007F83FFFFFF0008823FFFFC001F000C007FFFF
      8031F800C007FFFF8031FA00800300078031FC00800300078001FF0080037FF7
      8001FE40800380178001FF80800380178FF1FFC8C00780178FF1FFF000018017
      8FF1FFF6000180178FF1FFFC000180178FF138FD8003FFE78FF50007C6C74207
      8001000FFC7F3C07FFFF0000FFFF0007FFFFFFFFF111FFFFE3C7FFFFFFFFF83F
      C183C0030000E00FC183CFFBD111CC47C183C00B91118463C183C00BD111A073
      C183C00B000031F9C183C00BD11138F9C183C00B91F13C79C183C00BD1F13C39
      C183C00B00003C19C183C00BDFF19C0BC183C0039FF18C43C387C003DFFFC467
      C78FFFFF0000E00FFFFFFFFFFFFFF83FFFFFBFFFFFDFFFFF07C1BFFFFFCFE7FF
      07C1B049FFC7EBFF07C1807F0003F5FF06C1B07F0001EAFFDD77B9FF0000F57F
      DAB7BFFF0001EABFC547B0490003F55FFABF807F0007EABFFD7FB07F000FF57F
      FEFFB9FF001FEAFFF83FBFFF007FF5FFF83F048F00FFEBFFF83F07FF01FFF7FF
      F83F07FF03FFEFFFFFFF9FFFFFFFFFFFFFFFFFFFFFFFFF00FFFF8003801FFF00
      001F00010000FF00000F00010000FF0000070001000000000003000100000000
      00010001000000000000000100000000001F000100000023001F000180000001
      001F0001800000008FF10001FC000023FFF90001FC010063FF750001FC0300C3
      FF8F0001FC070107FFFF8003FFFF03FF00000000000000000000000000000000
      000000000000}
  end
  object OpenDialog: TOpenDialog
    DefaultExt = 'ini'
    Filter = 'Project Ini File (*.ini)|*.ini'
    Left = 256
    Top = 65528
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'ini'
    Filter = 'Project Ini File (*.ini)|*.ini'
    Left = 176
    Top = 65528
  end
  object HideTimer: TTimer
    Interval = 10
    OnTimer = HideTimerTimer
    Left = 592
    Top = 64
  end
  object PageControlPopupMenu: TPopupMenu
    Left = 632
    Top = 8
    object AddPage1: TMenuItem
      Caption = 'Add Page'
      object Images3: TMenuItem
        Caption = 'Images'
        OnClick = Images3Click
      end
      object Watches2: TMenuItem
        Caption = 'Watches'
        OnClick = Watches2Click
      end
    end
    object DeletePage1: TMenuItem
      Caption = 'Delete Page'
      OnClick = DeletePage1Click
    end
    object RenamePage1: TMenuItem
      Caption = 'Rename Page'
      Enabled = False
    end
  end
  object TrayIcon: TTrayIcon
    PopupMenu = TrayPopupMenu
    Visible = True
    OnDblClick = TrayIconDblClick
    Left = 112
    Top = 88
  end
  object TrayPopupMenu: TPopupMenu
    Left = 32
    Top = 136
    object Open1: TMenuItem
      Caption = 'Open'
      Default = True
      OnClick = Open1Click
    end
    object Hide1: TMenuItem
      Caption = 'Hide'
      OnClick = Hide1Click
    end
    object N7: TMenuItem
      Caption = '-'
    end
    object Start2: TMenuItem
      Caption = 'Start'
      OnClick = Start2Click
    end
    object Pause2: TMenuItem
      Caption = 'Pause'
      OnClick = Pause2Click
    end
    object Reset2: TMenuItem
      Caption = 'Reset'
      OnClick = Reset2Click
    end
    object N8: TMenuItem
      Caption = '-'
    end
    object Close1: TMenuItem
      Caption = 'Exit'
      OnClick = Close1Click
    end
  end
  object ChannelsPopupMenu: TPopupMenu
    Left = 240
    Top = 120
    object AddChannel1: TMenuItem
      Caption = 'Add Channel'
      OnClick = AddChannel1Click
    end
    object InsertChannel1: TMenuItem
      Caption = 'Insert Channel'
      OnClick = InsertChannel1Click
    end
    object DelChannel1: TMenuItem
      Caption = 'Del Last Channel'
      OnClick = DelChannel1Click
    end
    object DeleteSelectedChannel1: TMenuItem
      Caption = 'Delete Selected Channel'
      OnClick = DeleteSelectedChannel1Click
    end
    object N10: TMenuItem
      Caption = '-'
    end
    object StartChannel1: TMenuItem
      Caption = 'Start Channel'
      OnClick = StartChannel1Click
    end
    object PauseChannel1: TMenuItem
      Caption = 'Pause Channel'
      OnClick = PauseChannel1Click
    end
    object ResetChannel1: TMenuItem
      Caption = 'Reset Channel'
      OnClick = ResetChannel1Click
    end
  end
  object ApplicationEvents: TApplicationEvents
    OnException = ApplicationEventsException
    Left = 528
    Top = 320
  end
end
