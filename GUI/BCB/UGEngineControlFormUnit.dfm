object UGEngineControlForm: TUGEngineControlForm
  Left = 150
  Top = 0
  Caption = 'Engine Control'
  ClientHeight = 516
  ClientWidth = 839
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
    Left = 91
    Top = 48
    Width = 748
    Height = 449
    Align = alClient
    TabOrder = 0
    ExplicitTop = 37
    ExplicitHeight = 465
    object Splitter2: TSplitter
      Left = 250
      Top = 1
      Width = 7
      Height = 287
      ExplicitHeight = 303
    end
    object Splitter3: TSplitter
      Left = 1
      Top = 288
      Width = 746
      Height = 7
      Cursor = crVSplit
      Align = alBottom
      ExplicitTop = 304
    end
    object Panel2: TPanel
      Left = 1
      Top = 1
      Width = 249
      Height = 287
      Align = alLeft
      TabOrder = 0
      ExplicitHeight = 303
      inline UComponentsListFrame1: TUComponentsListFrame
        Left = 1
        Top = 1
        Width = 247
        Height = 285
        Align = alClient
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        TabOrder = 0
        ExplicitLeft = 1
        ExplicitTop = 1
        ExplicitWidth = 248
        ExplicitHeight = 301
        inherited Splitter1: TSplitter
          Left = 0
          Top = 217
          Width = 247
          Height = 8
          Cursor = crVSplit
          Align = alTop
          ExplicitLeft = 0
          ExplicitTop = 217
          ExplicitWidth = 248
          ExplicitHeight = 8
        end
        inherited Panel1: TPanel
          Left = 0
          Top = 225
          Width = 247
          Height = 60
          ExplicitLeft = 0
          ExplicitTop = 225
          ExplicitWidth = 248
          ExplicitHeight = 76
          inherited PageControl1: TPageControl
            Width = 245
            Height = 58
            ExplicitWidth = 246
            ExplicitHeight = 74
            inherited TabSheet6: TTabSheet
              inherited Splitter5: TSplitter
                Top = -45
                Width = 237
                ExplicitTop = -26
                ExplicitWidth = 240
              end
              inherited EnchancedSG1: TEnchancedSG
                Width = 237
                ExplicitWidth = 240
                ExplicitHeight = 335
                inherited BasicStringGrid: TStringGrid
                  OnMouseEnter = EnchancedSG1BasicStringGridMouseEnter
                end
                inherited txtStringEdit: TEdit
                  Height = 26
                  ExplicitHeight = 26
                end
                inherited txtDoubleEdit: TEdit
                  Height = 26
                  ExplicitHeight = 26
                end
                inherited txtIntEdit: TEdit
                  Height = 26
                  ExplicitHeight = 26
                end
                inherited txtBorderedIntEdit: TEdit
                  Height = 26
                  ExplicitHeight = 26
                end
              end
              inherited HeaderControl2: THeaderControl
                Top = -2
                Width = 237
                ExplicitTop = 17
                ExplicitWidth = 240
              end
              inherited NiceParamValRichEdit: TRichEdit
                Top = -35
                Width = 237
                OnMouseEnter = UComponentsListFrame1NiceParamValRichEditMouseEnter
                ExplicitTop = -35
                ExplicitWidth = 237
              end
            end
            inherited TabSheet7: TTabSheet
              inherited Splitter6: TSplitter
                ExplicitTop = 335
                ExplicitWidth = 288
              end
              inherited HeaderControl4: THeaderControl
                ExplicitTop = 378
                ExplicitWidth = 288
              end
              inherited NiceStateValRichEdit: TRichEdit
                OnMouseEnter = UComponentsListFrame1NiceStateValRichEditMouseEnter
              end
              inherited EnchancedSG2: TEnchancedSG
                ExplicitWidth = 288
                ExplicitHeight = 335
                inherited BasicStringGrid: TStringGrid
                  OnMouseEnter = EnchancedSG2BasicStringGridMouseEnter
                end
                inherited txtStringEdit: TEdit
                  Height = 26
                  ExplicitHeight = 26
                end
                inherited txtDoubleEdit: TEdit
                  Height = 26
                  ExplicitHeight = 26
                end
                inherited txtIntEdit: TEdit
                  Height = 26
                  ExplicitHeight = 26
                end
                inherited txtBorderedIntEdit: TEdit
                  Height = 26
                  ExplicitHeight = 26
                end
              end
            end
            inherited TabSheet3: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 4
              ExplicitWidth = 376
              ExplicitHeight = 539
              inherited Splitter2: TSplitter
                Width = 288
                ExplicitTop = 239
                ExplicitWidth = 288
              end
              inherited GroupBox: TGroupBox
                Width = 288
                ExplicitWidth = 288
                inherited OutputsStringGrid: TStringGrid
                  Width = 284
                  Height = 223
                  ExplicitWidth = 284
                  ExplicitHeight = 223
                end
              end
              inherited GroupBox2: TGroupBox
                Width = 288
                Height = 165
                ExplicitWidth = 288
                ExplicitHeight = 165
                inherited InputsStringGrid: TStringGrid
                  Width = 284
                  Height = 149
                  ExplicitWidth = 284
                  ExplicitHeight = 149
                end
              end
            end
            inherited TabSheet1: TTabSheet
              inherited ParametersHeaderControl: THeaderControl
                ExplicitWidth = 288
              end
            end
            inherited TabSheet2: TTabSheet
              inherited StateHeaderControl: THeaderControl
                ExplicitWidth = 288
              end
            end
          end
        end
        inherited Panel2: TPanel
          Width = 247
          Height = 217
          Align = alTop
          ExplicitWidth = 248
          ExplicitHeight = 217
          inherited StringGrid: TStringGrid
            Top = 21
            Width = 246
            Height = 174
            DefaultRowHeight = 18
            OnMouseEnter = UComponentsListFrame1StringGridMouseEnter
            ExplicitTop = 21
            ExplicitWidth = 246
            ExplicitHeight = 174
          end
          inherited HeaderControl: THeaderControl
            Width = 246
            Height = 20
            ExplicitWidth = 246
            ExplicitHeight = 20
          end
          inherited ClassNamePanel: TPanel
            Top = 195
            Width = 246
            Height = 21
            ExplicitTop = 195
            ExplicitWidth = 246
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
      Top = 295
      Width = 746
      Height = 153
      Align = alBottom
      TabOrder = 1
      ExplicitTop = 311
      object PageControl2: TPageControl
        Left = 1
        Top = 1
        Width = 744
        Height = 151
        ActivePage = LogsTabSheet
        Align = alClient
        MultiLine = True
        TabOrder = 0
        TabPosition = tpRight
        ExplicitWidth = 745
        object LogsTabSheet: TTabSheet
          Caption = 'Logs'
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 882
          ExplicitHeight = 0
        end
        object TabSheet2: TTabSheet
          Caption = 'Performance'
          ImageIndex = 1
          ExplicitWidth = 946
          ExplicitHeight = 190
          inline UComponentsPerformanceFrame1: TUComponentsPerformanceFrame
            Left = 0
            Top = 0
            Width = 716
            Height = 143
            Align = alClient
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
            ExplicitWidth = 720
            ExplicitHeight = 145
            inherited Chart: TChart
              Width = 577
              Height = 143
              ExplicitLeft = 0
              ExplicitTop = 0
              ExplicitWidth = 581
              ExplicitHeight = 145
              PrintMargins = (
                15
                19
                15
                19)
              inherited Series1: TBarSeries
                Data = {
                  00060000000000000000C87E400000000000187A400000000000F07940000000
                  0000A0794000000000000074400000000000807140}
              end
            end
            inherited Panel1: TPanel
              Left = 577
              Top = 0
              Width = 139
              Height = 143
              Align = alRight
              ExplicitLeft = 581
              ExplicitTop = 0
              ExplicitWidth = 139
              ExplicitHeight = 145
              inherited ShowModeRadioGroup: TRadioGroup
                Width = 137
                Height = 32
                Align = alTop
                ExplicitWidth = 137
                ExplicitHeight = 32
              end
              inherited GroupBox1: TGroupBox
                Left = 1
                Top = 33
                Width = 137
                Height = 112
                ExplicitLeft = 1
                ExplicitTop = 33
                ExplicitWidth = 137
                ExplicitHeight = 112
                inherited AverageIntervalSpinEdit: TSpinEdit
                  Height = 28
                  ExplicitHeight = 28
                end
              end
            end
          end
        end
      end
    end
    object PageControl1: TPageControl
      Left = 257
      Top = 1
      Width = 490
      Height = 287
      ActivePage = Draw
      Align = alClient
      DoubleBuffered = True
      ParentDoubleBuffered = False
      PopupMenu = PageControlPopupMenu
      TabOrder = 2
      OnChange = PageControl1Change
      ExplicitHeight = 303
      object Draw: TTabSheet
        Caption = 'Draw'
        OnShow = DrawShow
        ExplicitTop = 28
        ExplicitWidth = 633
        ExplicitHeight = 364
        inline UDrawEngineFrame1: TUDrawEngineFrame
          Left = 0
          Top = 0
          Width = 482
          Height = 259
          Align = alClient
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          ExplicitWidth = 484
          ExplicitHeight = 281
          inherited Splitter1: TSplitter
            Left = 242
            Height = 210
            ExplicitLeft = 244
            ExplicitHeight = 232
          end
          inherited Panel1: TPanel
            Left = 253
            Height = 210
            ExplicitLeft = 255
            ExplicitHeight = 232
            inherited UClassesListFrame: TUClassesListFrame
              Height = 208
              ExplicitHeight = 231
              inherited PageControl: TPageControl
                Height = 208
                ActivePage = UDrawEngineFrame1.UClassesListFrame.LibsTabSheet
                ExplicitHeight = 231
                inherited NameTabSheet: TTabSheet
                  inherited StringGrid: TStringGrid
                    Width = 167
                    Height = 301
                    ExplicitWidth = 167
                    ExplicitHeight = 301
                  end
                end
                inherited LibsTabSheet: TTabSheet
                  ExplicitWidth = 219
                  ExplicitHeight = 180
                  inherited TreeView: TTreeView
                    Width = 219
                    Height = 180
                    ExplicitWidth = 219
                    ExplicitHeight = 180
                  end
                end
                inherited LibsControlTabSheet: TTabSheet
                  inherited Splitter1: TSplitter
                    Top = 11
                    Width = 167
                    ExplicitTop = 11
                    ExplicitWidth = 167
                  end
                  inherited Panel1: TPanel
                    Top = 178
                    Width = 167
                    ExplicitTop = 178
                    ExplicitWidth = 167
                    inherited LoadLibraryButton: TButton
                      Width = 166
                      ExplicitWidth = 166
                    end
                    inherited CreateRuntimeLibraryButton: TButton
                      Width = 166
                      ExplicitWidth = 166
                    end
                    inherited DeleteLibraryButton: TButton
                      Width = 166
                      ExplicitWidth = 166
                    end
                    inherited RenameRuntimeLibraryButton: TButton
                      Width = 166
                      ExplicitWidth = 166
                    end
                    inherited AddClassButton: TButton
                      Width = 166
                      ExplicitWidth = 166
                    end
                  end
                  inherited GroupBox1: TGroupBox
                    Width = 167
                    Height = 11
                    ExplicitWidth = 167
                    ExplicitHeight = 11
                    inherited LibsListStringGrid: TStringGrid
                      Width = 164
                      ExplicitWidth = 164
                    end
                  end
                  inherited GroupBox2: TGroupBox
                    Top = 18
                    Width = 167
                    ExplicitTop = 18
                    ExplicitWidth = 167
                    inherited LibComponentListStringGrid: TStringGrid
                      Width = 164
                      ExplicitWidth = 164
                    end
                  end
                end
              end
            end
          end
          inherited Panel2: TPanel
            Width = 242
            Height = 210
            ExplicitWidth = 244
            ExplicitHeight = 232
            inherited ScrollBox: TScrollBox
              Width = 240
              Height = 208
              ExplicitWidth = 242
              ExplicitHeight = 228
              inherited Image: TImage
                Left = -1
                Top = -2
                ExplicitLeft = -1
                ExplicitTop = -2
              end
            end
          end
          inherited Panel3: TPanel
            Top = 210
            Width = 482
            ExplicitTop = 210
            ExplicitWidth = 482
            inherited RectWidthLabeledEdit: TLabeledEdit
              Height = 26
              EditLabel.ExplicitLeft = 216
              EditLabel.ExplicitTop = 8
              EditLabel.ExplicitWidth = 28
              ExplicitHeight = 26
            end
            inherited RectHeightLabeledEdit: TLabeledEdit
              Height = 26
              EditLabel.ExplicitLeft = 263
              EditLabel.ExplicitTop = 8
              EditLabel.ExplicitWidth = 31
              ExplicitHeight = 26
            end
            inherited ApplyButton: TButton
              Left = 316
              ExplicitLeft = 316
            end
            inherited RestoreButton: TButton
              Left = 397
              ExplicitLeft = 397
            end
          end
          inherited PopupMenu: TPopupMenu
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
    Top = 497
    Width = 839
    Height = 19
    Panels = <>
    SimplePanel = True
    ExplicitTop = 502
    ExplicitWidth = 1097
  end
  object ChannelsStringGrid: TStringGrid
    Left = 0
    Top = 48
    Width = 91
    Height = 449
    Align = alLeft
    ColCount = 3
    DefaultColWidth = 24
    FixedCols = 0
    FixedRows = 0
    Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goDrawFocusSelected, goRowSelect]
    PopupMenu = ChannelsPopupMenu
    ScrollBars = ssVertical
    TabOrder = 2
    Visible = False
    OnClick = ChannelsStringGridClick
    OnSelectCell = ChannelsStringGridSelectCell
    ExplicitTop = 37
    ExplicitHeight = 465
  end
  object ToolBar: TToolBar
    Left = 0
    Top = 0
    Width = 839
    Height = 48
    AutoSize = True
    ButtonHeight = 48
    ButtonWidth = 127
    Caption = 'ToolBar'
    Color = clBtnFace
    Images = ImageList
    ParentColor = False
    ShowCaptions = True
    TabOrder = 3
    ExplicitWidth = 1037
    object ToolButton17: TToolButton
      Left = 0
      Top = 0
      AutoSize = True
      Caption = 'Create Project'
      ImageIndex = 21
      MenuItem = CreateProjectItem
    end
    object ToolButton1: TToolButton
      Left = 99
      Top = 0
      AutoSize = True
      Caption = 'Load Project'
      ImageIndex = 0
      MenuItem = LoadProjectItem
    end
    object ToolButton2: TToolButton
      Left = 188
      Top = 0
      AutoSize = True
      Caption = 'Save Project'
      ImageIndex = 1
      MenuItem = SaveProjectItem
    end
    object ToolButton3: TToolButton
      Left = 277
      Top = 0
      Width = 8
      Caption = 'ToolButton3'
      ImageIndex = 2
      Style = tbsSeparator
    end
    object ToolButton19: TToolButton
      Left = 285
      Top = 0
      AutoSize = True
      Caption = 'Reload Parameters'
      ImageIndex = 22
      MenuItem = ReloadParameters1
    end
    object ToolButton14: TToolButton
      Left = 410
      Top = 0
      Width = 8
      Caption = 'ToolButton14'
      ImageIndex = 10
      Style = tbsSeparator
    end
    object ToolButton10: TToolButton
      Left = 418
      Top = 0
      AutoSize = True
      Caption = 'Start'
      ImageIndex = 7
      MenuItem = Start1
    end
    object ToolButton12: TToolButton
      Left = 459
      Top = 0
      AutoSize = True
      Caption = 'Pause'
      ImageIndex = 8
      MenuItem = Pause1
    end
    object ToolButton13: TToolButton
      Left = 506
      Top = 0
      AutoSize = True
      Caption = 'Reset'
      ImageIndex = 9
      MenuItem = Reset1
    end
    object ToolButton16: TToolButton
      Left = 551
      Top = 0
      AutoSize = True
      Caption = 'Step'
      ImageIndex = 20
      MenuItem = Step1
    end
    object ToolButton21: TToolButton
      Left = 590
      Top = 0
      Width = 8
      Caption = 'ToolButton21'
      ImageIndex = 21
      Style = tbsSeparator
    end
    object ToolButton15: TToolButton
      Left = 598
      Top = 0
      AutoSize = True
      Caption = 'Window'
      ImageIndex = 10
      MenuItem = Window1
      Style = tbsDropDown
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
        Enabled = False
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
      494C01011B003003300310001000FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
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
end
