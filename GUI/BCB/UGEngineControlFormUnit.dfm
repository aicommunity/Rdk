object UGEngineControlForm: TUGEngineControlForm
  Left = 150
  Top = 0
  Caption = 'Engine Control'
  ClientHeight = 615
  ClientWidth = 1074
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu
  OldCreateOrder = False
  Position = poDesigned
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 95
    Top = 44
    Width = 979
    Height = 527
    Align = alClient
    TabOrder = 0
    object Splitter2: TSplitter
      Left = 262
      Top = 1
      Width = 7
      Height = 384
      ExplicitHeight = 414
    end
    object Splitter3: TSplitter
      Left = 1
      Top = 385
      Width = 977
      Height = 7
      Cursor = crVSplit
      Align = alBottom
      ExplicitTop = 415
    end
    object Panel2: TPanel
      Left = 1
      Top = 1
      Width = 261
      Height = 384
      Align = alLeft
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 0
      inline UComponentsListFrame1: TUComponentsListFrame
        Left = 1
        Top = 1
        Width = 259
        Height = 382
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        ExplicitLeft = 1
        ExplicitTop = 1
        ExplicitWidth = 259
        ExplicitHeight = 382
        inherited Splitter1: TSplitter
          Left = 0
          Top = 227
          Width = 259
          Height = 8
          Cursor = crVSplit
          Align = alTop
          ExplicitLeft = 0
          ExplicitTop = 227
          ExplicitWidth = 260
          ExplicitHeight = 8
        end
        inherited Panel1: TPanel
          Left = 0
          Top = 235
          Width = 259
          Height = 147
          ExplicitLeft = 0
          ExplicitTop = 235
          ExplicitWidth = 259
          ExplicitHeight = 147
          inherited PageControl1: TPageControl
            Width = 257
            Height = 145
            ExplicitWidth = 257
            ExplicitHeight = 145
            inherited TabSheet6: TTabSheet
              ExplicitWidth = 249
              ExplicitHeight = 118
              inherited Splitter5: TSplitter
                Top = 37
                Width = 249
                ExplicitTop = 70
                ExplicitWidth = 252
              end
              inherited EnchancedSG1: TEnchancedSG
                Width = 249
                Height = 37
                ExplicitWidth = 249
                ExplicitHeight = 37
                inherited BasicStringGrid: TStringGrid
                  Left = 17
                  Top = 58
                  Width = 168
                  Height = 102
                  OnMouseEnter = EnchancedSG1BasicStringGridMouseEnter
                  ExplicitLeft = 17
                  ExplicitTop = 58
                  ExplicitWidth = 168
                  ExplicitHeight = 102
                end
                inherited txtStringEdit: TEdit
                  Left = 34
                  Top = 259
                  Width = 126
                  Height = 22
                  ExplicitLeft = 34
                  ExplicitTop = 259
                  ExplicitWidth = 126
                  ExplicitHeight = 22
                end
                inherited chbBoolEdit: TCheckBox
                  Left = 37
                  Top = 383
                  Width = 101
                  Height = 18
                  ExplicitLeft = 37
                  ExplicitTop = 383
                  ExplicitWidth = 101
                  ExplicitHeight = 18
                end
                inherited udBorderedInt: TUpDown
                  Left = 159
                  Top = 339
                  Width = 18
                  Height = 22
                  ExplicitLeft = 159
                  ExplicitTop = 339
                  ExplicitWidth = 18
                  ExplicitHeight = 22
                end
                inherited txtDoubleEdit: TEdit
                  Left = 31
                  Top = 218
                  Width = 127
                  Height = 22
                  ExplicitLeft = 31
                  ExplicitTop = 218
                  ExplicitWidth = 127
                  ExplicitHeight = 22
                end
                inherited txtIntEdit: TEdit
                  Left = 34
                  Top = 407
                  Width = 126
                  Height = 22
                  ExplicitLeft = 34
                  ExplicitTop = 407
                  ExplicitWidth = 126
                  ExplicitHeight = 22
                end
                inherited txtBorderedIntEdit: TEdit
                  Left = 34
                  Top = 339
                  Width = 126
                  Height = 22
                  ExplicitLeft = 34
                  ExplicitTop = 339
                  ExplicitWidth = 126
                  ExplicitHeight = 22
                end
                inherited cmbListEdit: TComboBox
                  Left = 34
                  Top = 300
                  Width = 151
                  ExplicitLeft = 34
                  ExplicitTop = 300
                  ExplicitWidth = 151
                end
              end
              inherited HeaderControl2: THeaderControl
                Top = 82
                Width = 249
                Height = 36
                ExplicitTop = 82
                ExplicitWidth = 249
                ExplicitHeight = 36
              end
              inherited NiceParamValRichEdit: TRichEdit
                Top = 47
                Width = 249
                Height = 35
                Font.Height = -12
                Constraints.MinHeight = 20
                OnMouseEnter = UComponentsListFrame1NiceParamValRichEditMouseEnter
                ExplicitTop = 47
                ExplicitWidth = 249
                ExplicitHeight = 35
              end
            end
            inherited TabSheet7: TTabSheet
              inherited Splitter6: TSplitter
                Top = 458
                Height = 11
                ExplicitTop = 350
                ExplicitWidth = 301
                ExplicitHeight = 11
              end
              inherited HeaderControl4: THeaderControl
                Top = 503
                Height = 36
                ExplicitTop = 503
                ExplicitHeight = 36
              end
              inherited NiceStateValRichEdit: TRichEdit
                Top = 469
                Height = 34
                Font.Height = -12
                Constraints.MinHeight = 20
                OnMouseEnter = UComponentsListFrame1NiceStateValRichEditMouseEnter
                ExplicitTop = 469
                ExplicitHeight = 34
              end
              inherited EnchancedSG2: TEnchancedSG
                Height = 458
                ExplicitHeight = 458
                inherited BasicStringGrid: TStringGrid
                  Left = 17
                  Top = 58
                  Width = 168
                  Height = 102
                  OnMouseEnter = EnchancedSG2BasicStringGridMouseEnter
                  ExplicitLeft = 17
                  ExplicitTop = 58
                  ExplicitWidth = 168
                  ExplicitHeight = 102
                end
                inherited txtStringEdit: TEdit
                  Left = 34
                  Top = 259
                  Width = 126
                  Height = 22
                  ExplicitLeft = 34
                  ExplicitTop = 259
                  ExplicitWidth = 126
                  ExplicitHeight = 22
                end
                inherited chbBoolEdit: TCheckBox
                  Left = 37
                  Top = 383
                  Width = 101
                  Height = 18
                  ExplicitLeft = 37
                  ExplicitTop = 383
                  ExplicitWidth = 101
                  ExplicitHeight = 18
                end
                inherited udBorderedInt: TUpDown
                  Left = 159
                  Top = 339
                  Width = 18
                  Height = 22
                  ExplicitLeft = 159
                  ExplicitTop = 339
                  ExplicitWidth = 18
                  ExplicitHeight = 22
                end
                inherited txtDoubleEdit: TEdit
                  Left = 31
                  Top = 218
                  Width = 127
                  Height = 22
                  ExplicitLeft = 31
                  ExplicitTop = 218
                  ExplicitWidth = 127
                  ExplicitHeight = 22
                end
                inherited txtIntEdit: TEdit
                  Left = 34
                  Top = 407
                  Width = 126
                  Height = 22
                  ExplicitLeft = 34
                  ExplicitTop = 407
                  ExplicitWidth = 126
                  ExplicitHeight = 22
                end
                inherited txtBorderedIntEdit: TEdit
                  Left = 34
                  Top = 339
                  Width = 126
                  Height = 22
                  ExplicitLeft = 34
                  ExplicitTop = 339
                  ExplicitWidth = 126
                  ExplicitHeight = 22
                end
                inherited cmbListEdit: TComboBox
                  Left = 34
                  Top = 300
                  Width = 151
                  ExplicitLeft = 34
                  ExplicitTop = 300
                  ExplicitWidth = 151
                end
              end
            end
            inherited TabSheet3: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 4
              ExplicitWidth = 376
              ExplicitHeight = 539
              inherited Splitter2: TSplitter
                Top = 250
                ExplicitTop = 250
                ExplicitWidth = 301
              end
              inherited GroupBox: TGroupBox
                Height = 250
                ExplicitHeight = 250
                inherited OutputsStringGrid: TStringGrid
                  Top = 16
                  Height = 232
                  ExplicitTop = 16
                  ExplicitHeight = 232
                end
              end
              inherited GroupBox2: TGroupBox
                Top = 258
                Height = 281
                ExplicitTop = 258
                ExplicitHeight = 281
                inherited InputsStringGrid: TStringGrid
                  Top = 16
                  Height = 263
                  ExplicitTop = 16
                  ExplicitHeight = 263
                end
              end
            end
            inherited TabSheet1: TTabSheet
              inherited ParametersHeaderControl: THeaderControl
                Top = 504
                Height = 35
                ExplicitTop = 504
                ExplicitHeight = 35
              end
              inherited ParametersRichEdit: TRichEdit
                Height = 504
                Font.Height = -12
                ExplicitHeight = 504
              end
            end
            inherited TabSheet2: TTabSheet
              inherited StateHeaderControl: THeaderControl
                Top = 504
                Height = 35
                ExplicitTop = 504
                ExplicitHeight = 35
              end
              inherited StateRichEdit: TRichEdit
                Height = 504
                Font.Height = -12
                ExplicitHeight = 504
              end
            end
          end
        end
        inherited Panel2: TPanel
          Width = 259
          Height = 227
          Align = alTop
          ExplicitWidth = 259
          ExplicitHeight = 227
          inherited StringGrid: TStringGrid
            Top = 22
            Width = 257
            Height = 183
            DefaultRowHeight = 18
            OnMouseEnter = UComponentsListFrame1StringGridMouseEnter
            ExplicitTop = 22
            ExplicitWidth = 257
            ExplicitHeight = 183
          end
          inherited HeaderControl: THeaderControl
            Width = 257
            Height = 21
            ExplicitWidth = 257
            ExplicitHeight = 21
          end
          inherited ClassNamePanel: TPanel
            Top = 205
            Width = 257
            Height = 21
            ExplicitTop = 205
            ExplicitWidth = 257
            ExplicitHeight = 21
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
      Top = 392
      Width = 977
      Height = 134
      Align = alBottom
      DoubleBuffered = True
      ParentDoubleBuffered = False
      TabOrder = 1
      object PageControl2: TPageControl
        Left = 1
        Top = 1
        Width = 975
        Height = 132
        ActivePage = LogsTabSheet
        Align = alClient
        MultiLine = True
        TabOrder = 0
        TabPosition = tpRight
        object LogsTabSheet: TTabSheet
          Caption = 'Logs'
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
        end
        object TabSheet2: TTabSheet
          Caption = 'Performance'
          ImageIndex = 1
          inline UComponentsPerformanceFrame1: TUComponentsPerformanceFrame
            Left = 0
            Top = 0
            Width = 947
            Height = 124
            Align = alClient
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
            ExplicitWidth = 947
            ExplicitHeight = 124
            inherited PageControl1: TPageControl
              Width = 802
              Height = 124
              Margins.Left = 2
              Margins.Top = 2
              Margins.Right = 2
              Margins.Bottom = 2
              ExplicitWidth = 802
              ExplicitHeight = 124
              inherited TabSheet1: TTabSheet
                Margins.Left = 2
                Margins.Top = 2
                Margins.Right = 2
                Margins.Bottom = 2
                ExplicitLeft = 4
                ExplicitTop = 25
                ExplicitWidth = 794
                ExplicitHeight = 95
                inherited Chart: TChart
                  Width = 794
                  Height = 95
                  ExplicitWidth = 794
                  ExplicitHeight = 95
                  PrintMargins = (
                    15
                    19
                    15
                    19)
                end
              end
              inherited TabSheet3: TTabSheet
                Margins.Left = 2
                Margins.Top = 2
                Margins.Right = 2
                Margins.Bottom = 2
                ExplicitLeft = 4
                ExplicitTop = 24
                ExplicitWidth = 446
                ExplicitHeight = 252
                inherited GroupBox2: TGroupBox
                  Margins.Left = 2
                  Margins.Top = 2
                  Margins.Right = 2
                  Margins.Bottom = 2
                  inherited ComponentsStringGrid: TStringGrid
                    Top = 16
                    Height = 234
                    Margins.Left = 2
                    Margins.Top = 2
                    Margins.Right = 2
                    Margins.Bottom = 2
                    ExplicitTop = 16
                    ExplicitHeight = 234
                  end
                end
              end
              inherited TabSheet2: TTabSheet
                Margins.Left = 2
                Margins.Top = 2
                Margins.Right = 2
                Margins.Bottom = 2
                ExplicitLeft = 4
                ExplicitTop = 24
                ExplicitWidth = 446
                ExplicitHeight = 252
                inherited GroupBox3: TGroupBox
                  Margins.Left = 2
                  Margins.Top = 2
                  Margins.Right = 2
                  Margins.Bottom = 2
                  inherited InterfacesStringGrid: TStringGrid
                    Top = 16
                    Height = 234
                    Margins.Left = 2
                    Margins.Top = 2
                    Margins.Right = 2
                    Margins.Bottom = 2
                    ExplicitTop = 16
                    ExplicitHeight = 234
                  end
                end
              end
            end
            inherited Panel1: TPanel
              Left = 802
              Top = 0
              Width = 145
              Height = 124
              Align = alRight
              ExplicitLeft = 802
              ExplicitTop = 0
              ExplicitWidth = 145
              ExplicitHeight = 124
              inherited ShowModeRadioGroup: TRadioGroup
                Width = 143
                Height = 33
                Align = alTop
                ExplicitWidth = 143
                ExplicitHeight = 33
              end
              inherited GroupBox1: TGroupBox
                Left = 1
                Top = 34
                Width = 143
                Height = 89
                ExplicitLeft = 1
                ExplicitTop = 34
                ExplicitWidth = 143
                ExplicitHeight = 89
                inherited AverageIntervalSpinEdit: TSpinEdit
                  Top = 17
                  Width = 45
                  Height = 23
                  ExplicitTop = 17
                  ExplicitWidth = 45
                  ExplicitHeight = 23
                end
              end
            end
          end
        end
      end
    end
    object PageControl1: TPageControl
      Left = 269
      Top = 1
      Width = 709
      Height = 384
      ActivePage = Draw
      Align = alClient
      DoubleBuffered = True
      ParentDoubleBuffered = False
      PopupMenu = PageControlPopupMenu
      TabOrder = 2
      OnChange = PageControl1Change
      object Draw: TTabSheet
        Caption = 'Draw'
        OnShow = DrawShow
        inline UDrawEngineFrame1: TUDrawEngineFrame
          Left = 0
          Top = 0
          Width = 701
          Height = 356
          Align = alClient
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          ExplicitWidth = 701
          ExplicitHeight = 356
          inherited Splitter1: TSplitter
            Left = 451
            Width = 12
            Height = 304
            ExplicitLeft = 453
            ExplicitWidth = 12
            ExplicitHeight = 340
          end
          inherited Panel1: TPanel
            Left = 463
            Width = 238
            Height = 304
            ExplicitLeft = 463
            ExplicitWidth = 238
            ExplicitHeight = 304
            inherited UClassesListFrame: TUClassesListFrame
              Width = 236
              Height = 302
              Font.Height = -12
              ExplicitWidth = 236
              ExplicitHeight = 302
              inherited PageControl: TPageControl
                Width = 236
                Height = 302
                ActivePage = UDrawEngineFrame1.UClassesListFrame.LibsTabSheet
                ExplicitWidth = 236
                ExplicitHeight = 302
                inherited LibsTabSheet: TTabSheet
                  ExplicitTop = 25
                  ExplicitWidth = 228
                  ExplicitHeight = 273
                  inherited TreeView: TTreeView
                    Width = 228
                    Height = 273
                    ExplicitWidth = 228
                    ExplicitHeight = 273
                  end
                end
                inherited LibsControlTabSheet: TTabSheet
                  inherited Splitter1: TSplitter
                    Top = 90
                    Height = 8
                    ExplicitTop = 11
                    ExplicitWidth = 175
                    ExplicitHeight = 8
                  end
                  inherited Panel1: TPanel
                    Top = 265
                    Height = 129
                    ExplicitTop = 265
                    ExplicitHeight = 129
                    inherited LoadLibraryButton: TButton
                      Height = 26
                      ExplicitHeight = 26
                    end
                    inherited CreateRuntimeLibraryButton: TButton
                      Top = 27
                      Height = 27
                      ExplicitTop = 27
                      ExplicitHeight = 27
                    end
                    inherited DeleteLibraryButton: TButton
                      Top = 54
                      ExplicitTop = 54
                    end
                    inherited RenameRuntimeLibraryButton: TButton
                      Top = 79
                      Height = 27
                      ExplicitTop = 79
                      ExplicitHeight = 27
                    end
                    inherited AddClassButton: TButton
                      Top = 106
                      Height = 26
                      ExplicitTop = 106
                      ExplicitHeight = 26
                    end
                  end
                  inherited GroupBox1: TGroupBox
                    Height = 90
                    ExplicitHeight = 90
                    inherited LibsListStringGrid: TStringGrid
                      Top = 16
                      Height = 72
                      ExplicitTop = 16
                      ExplicitHeight = 72
                    end
                  end
                  inherited GroupBox2: TGroupBox
                    Top = 98
                    Height = 167
                    ExplicitTop = 98
                    ExplicitHeight = 167
                    inherited LibComponentListStringGrid: TStringGrid
                      Top = 16
                      Height = 149
                      ExplicitTop = 16
                      ExplicitHeight = 149
                    end
                  end
                end
              end
            end
          end
          inherited Panel2: TPanel
            Width = 451
            Height = 304
            ExplicitWidth = 451
            ExplicitHeight = 304
            inherited ScrollBox: TScrollBox
              Width = 449
              Height = 302
              ExplicitWidth = 449
              ExplicitHeight = 302
              inherited Image: TImage
                Left = -1
                Top = -2
                Width = 319
                Height = 252
                ExplicitLeft = -1
                ExplicitTop = -2
                ExplicitWidth = 319
                ExplicitHeight = 252
              end
            end
          end
          inherited Panel3: TPanel
            Top = 304
            Width = 701
            Height = 52
            ExplicitTop = 304
            ExplicitWidth = 701
            ExplicitHeight = 52
            inherited Label1: TLabel
              Top = 6
              Width = 57
              Height = 14
              ExplicitTop = 6
              ExplicitWidth = 57
              ExplicitHeight = 14
            end
            inherited Label2: TLabel
              Left = 159
              Top = 6
              Width = 48
              Height = 14
              ExplicitLeft = 159
              ExplicitTop = 6
              ExplicitWidth = 48
              ExplicitHeight = 14
            end
            inherited RectWidthLabeledEdit: TLabeledEdit
              Left = 226
              Top = 25
              Width = 43
              Height = 22
              EditLabel.Width = 33
              EditLabel.Height = 14
              EditLabel.ExplicitLeft = 226
              EditLabel.ExplicitTop = 8
              EditLabel.ExplicitWidth = 33
              EditLabel.ExplicitHeight = 14
              ExplicitLeft = 226
              ExplicitTop = 25
              ExplicitWidth = 43
              ExplicitHeight = 22
            end
            inherited FontTypeComboBox: TComboBox
              Top = 25
              Width = 149
              Height = 22
              ExplicitTop = 25
              ExplicitWidth = 149
              ExplicitHeight = 22
            end
            inherited FontSizeComboBox: TComboBox
              Left = 159
              Top = 25
              Width = 59
              Height = 22
              ExplicitLeft = 159
              ExplicitTop = 25
              ExplicitWidth = 59
              ExplicitHeight = 22
            end
            inherited RectHeightLabeledEdit: TLabeledEdit
              Left = 275
              Top = 25
              Width = 43
              Height = 22
              EditLabel.Width = 36
              EditLabel.Height = 14
              EditLabel.ExplicitLeft = 275
              EditLabel.ExplicitTop = 8
              EditLabel.ExplicitWidth = 36
              EditLabel.ExplicitHeight = 14
              ExplicitLeft = 275
              ExplicitTop = 25
              ExplicitWidth = 43
              ExplicitHeight = 22
            end
            inherited ApplyButton: TButton
              Left = 526
              Top = 13
              Width = 79
              ExplicitLeft = 526
              ExplicitTop = 13
              ExplicitWidth = 79
            end
            inherited RestoreButton: TButton
              Left = 611
              Top = 13
              Width = 79
              ExplicitLeft = 611
              ExplicitTop = 13
              ExplicitWidth = 79
            end
            inherited ShowLinkDetailCheckBox: TCheckBox
              Left = 325
              Top = 28
              ExplicitLeft = 325
              ExplicitTop = 28
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
    Top = 590
    Width = 1074
    Height = 25
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
  end
  object ChannelsStringGrid: TStringGrid
    Left = 0
    Top = 44
    Width = 95
    Height = 527
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
  end
  object ToolBar: TToolBar
    Left = 0
    Top = 0
    Width = 1074
    Height = 44
    AutoSize = True
    ButtonHeight = 44
    ButtonWidth = 104
    Caption = 'ToolBar'
    Color = clBtnFace
    Images = ImageList
    ParentColor = False
    ShowCaptions = True
    TabOrder = 3
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
    Top = 571
    Width = 1074
    Height = 19
    Align = alBottom
    TabOrder = 4
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
      Left = 217
      Top = 1
      Width = 144
      Height = 17
      Align = alLeft
      Caption = 'Autoupdate properties'
      TabOrder = 1
      OnClick = AutoupdatePropertiesCheckBoxClick
    end
    object AutosaveStatesCheckBox: TCheckBox
      Left = 864
      Top = 1
      Width = 104
      Height = 17
      Align = alRight
      Caption = 'Autosave states'
      TabOrder = 2
      OnClick = AutosaveStatesCheckBoxClick
    end
    object AutosaveProjectCheckBox: TCheckBox
      Left = 968
      Top = 1
      Width = 105
      Height = 17
      Align = alRight
      Caption = 'Autosave project'
      TabOrder = 3
      OnClick = AutosaveProjectCheckBoxClick
    end
    object MultiThreadedRadioButton: TRadioButton
      Left = 760
      Top = 1
      Width = 104
      Height = 17
      Align = alRight
      Caption = 'Multi-threaded'
      TabOrder = 4
      OnClick = MultiThreadedRadioButtonClick
    end
    object SingleThreadedRadioButton: TRadioButton
      Left = 652
      Top = 1
      Width = 108
      Height = 17
      Align = alRight
      Caption = 'Single-threaded'
      TabOrder = 5
      OnClick = SingleThreadedRadioButtonClick
    end
    object DetailedDebugLogCheckBox: TCheckBox
      Left = 137
      Top = 1
      Width = 80
      Height = 17
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
      494C01011B005C03D00310001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
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
    Left = 120
    Top = 152
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
    Left = 344
    Top = 128
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
