object UCreateProjectWizardForm: TUCreateProjectWizardForm
  Left = 0
  Top = 0
  Caption = 'Create Project Wizard'
  ClientHeight = 721
  ClientWidth = 610
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 678
    Width = 610
    Height = 43
    Align = alBottom
    TabOrder = 0
    DesignSize = (
      610
      43)
    object PrevButton: TButton
      Left = 354
      Top = 7
      Width = 79
      Height = 27
      Anchors = [akTop, akRight]
      Caption = '<< Prev'
      TabOrder = 0
      OnClick = PrevButtonClick
    end
    object NextButton: TButton
      Left = 439
      Top = 7
      Width = 79
      Height = 27
      Anchors = [akTop, akRight]
      Caption = 'Next >>'
      TabOrder = 1
      OnClick = NextButtonClick
    end
    object FinishButton: TButton
      Left = 524
      Top = 7
      Width = 78
      Height = 27
      Anchors = [akTop, akRight]
      Caption = 'Finish'
      Default = True
      TabOrder = 2
      OnClick = FinishButtonClick
    end
  end
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 610
    Height = 678
    ActivePage = TabSheet3
    Align = alClient
    TabOrder = 1
    OnChange = PageControlChange
    object TabSheet1: TTabSheet
      Caption = 'Name'
      DesignSize = (
        602
        650)
      object Label1: TLabel
        Left = 3
        Top = 146
        Width = 90
        Height = 13
        Caption = 'Project Description'
      end
      object TitlePanel1: TPanel
        Left = 0
        Top = 0
        Width = 602
        Height = 43
        Align = alTop
        Alignment = taLeftJustify
        Caption = 
          'Please select project directory and set optional information suc' +
          'h as project name and description'
        TabOrder = 4
      end
      object ProjectNameLabeledEdit: TLabeledEdit
        Left = 3
        Top = 111
        Width = 595
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 64
        EditLabel.Height = 13
        EditLabel.Caption = 'Project Name'
        TabOrder = 2
        OnChange = ProjectNameLabeledEditChange
      end
      object ProjectDirectoryLabeledEdit: TLabeledEdit
        Left = 3
        Top = 62
        Width = 511
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 81
        EditLabel.Height = 13
        EditLabel.Caption = 'Project Directory'
        TabOrder = 0
        OnChange = ProjectDirectoryLabeledEditChange
      end
      object Button1: TButton
        Left = 520
        Top = 60
        Width = 78
        Height = 26
        Anchors = [akTop, akRight]
        Caption = 'Browse'
        TabOrder = 1
        OnClick = Button1Click
      end
      object ProjectDescriptionRichEdit: TRichEdit
        Left = 3
        Top = 166
        Width = 595
        Height = 457
        Anchors = [akLeft, akTop, akRight, akBottom]
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        PlainText = True
        TabOrder = 3
        Zoom = 100
        OnChange = ProjectDescriptionRichEditChange
      end
      object ProjectAutoSaveFlagCheckBox: TCheckBox
        Left = 3
        Top = 630
        Width = 73
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Autosave'
        Checked = True
        State = cbChecked
        TabOrder = 5
        OnClick = ProjectAutoSaveFlagCheckBoxClick
      end
      object ProjectAutoSaveStatesFlagCheckBox: TCheckBox
        Left = 78
        Top = 630
        Width = 107
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Autosave States'
        Checked = True
        State = cbChecked
        TabOrder = 6
        OnClick = ProjectAutoSaveStatesFlagCheckBoxClick
      end
      object EventsLogFlagCheckBox: TCheckBox
        Left = 187
        Top = 630
        Width = 107
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Log Events'
        Checked = True
        State = cbChecked
        TabOrder = 7
        OnClick = EventsLogFlagCheckBoxClick
      end
      object DisableStopVideoSourcesCheckBox: TCheckBox
        Left = 270
        Top = 630
        Width = 204
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Disable Stop Video Sources'
        Checked = True
        State = cbChecked
        TabOrder = 8
        OnClick = DisableStopVideoSourcesCheckBoxClick
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Type'
      ImageIndex = 1
      object TitlePanel2: TPanel
        Left = 0
        Top = 0
        Width = 602
        Height = 43
        Align = alTop
        Alignment = taLeftJustify
        Caption = 'Select calculation options'
        TabOrder = 0
      end
      object ProjectTypeRadioGroup: TRadioGroup
        Left = 0
        Top = 87
        Width = 602
        Height = 42
        Align = alTop
        Caption = 'Project type '
        Columns = 2
        ItemIndex = 0
        Items.Strings = (
          'Universal'
          'Video analysis')
        TabOrder = 1
        OnClick = ProjectTypeRadioGroupClick
      end
      object CalculationSourceTimeModeRadioGroup: TRadioGroup
        Left = 0
        Top = 170
        Width = 602
        Height = 42
        Align = alTop
        Caption = ' Calculation Time Source '
        Columns = 2
        ItemIndex = 0
        Items.Strings = (
          'System Time'
          'External Data Time')
        TabOrder = 2
        OnClick = CalculationSourceTimeModeRadioGroupClick
      end
      object ProjectModeRadioGroup: TRadioGroup
        Left = 0
        Top = 43
        Width = 602
        Height = 44
        Align = alTop
        Caption = ' Project Mode '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'Simple'
          'Server')
        TabOrder = 3
        OnClick = ProjectModeRadioGroupClick
      end
      object MultiThreadingModeRadioGroup: TRadioGroup
        Left = 0
        Top = 129
        Width = 602
        Height = 41
        Align = alTop
        Caption = ' Multi-Threading Mode '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'Single Thread'
          'Multi Threads')
        TabOrder = 4
        OnClick = MultiThreadingModeRadioGroupClick
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Model'
      ImageIndex = 2
      DesignSize = (
        602
        650)
      object TitlePanel3: TPanel
        Left = 0
        Top = 0
        Width = 602
        Height = 43
        Align = alTop
        Alignment = taLeftJustify
        Caption = 'Channels configuration'
        TabOrder = 0
        DesignSize = (
          602
          43)
        object ChannelsNumberLabeledEdit: TLabeledEdit
          Left = 490
          Top = 14
          Width = 91
          Height = 21
          Anchors = [akTop, akRight]
          EditLabel.Width = 84
          EditLabel.Height = 13
          EditLabel.Caption = 'Channels Number'
          TabOrder = 0
          Text = '1'
          OnChange = ChannelsNumberLabeledEditChange
        end
        object UpDown1: TUpDown
          Left = 581
          Top = 14
          Width = 17
          Height = 21
          Anchors = [akTop, akRight]
          Associate = ChannelsNumberLabeledEdit
          Min = 1
          Max = 1000
          Position = 1
          TabOrder = 1
        end
      end
      object GroupBox4: TGroupBox
        Left = 0
        Top = 43
        Width = 602
        Height = 583
        Align = alTop
        Anchors = [akLeft, akTop, akRight, akBottom]
        Caption = ' Channel Properties'
        TabOrder = 1
        object ChannelsStringGrid: TStringGrid
          Left = 2
          Top = 15
          Width = 48
          Height = 566
          Align = alLeft
          ColCount = 1
          DefaultColWidth = 24
          FixedCols = 0
          FixedRows = 0
          TabOrder = 0
          OnClick = ChannelsStringGridClick
        end
        object Panel4: TPanel
          Left = 50
          Top = 15
          Width = 550
          Height = 566
          Align = alClient
          TabOrder = 1
          object Panel2: TPanel
            Left = 1
            Top = 384
            Width = 548
            Height = 181
            Align = alBottom
            TabOrder = 0
            DesignSize = (
              548
              181)
            object ProjectTimeStepGroupBox: TGroupBox
              Left = 1
              Top = 1
              Width = 546
              Height = 57
              Align = alTop
              Caption = ' Time step duration  '
              TabOrder = 0
              object Label2: TLabel
                Left = 10
                Top = 24
                Width = 74
                Height = 13
                Caption = 'T (Default)=1./'
              end
              object Label3: TLabel
                Left = 146
                Top = 24
                Width = 68
                Height = 13
                Caption = 'T (Global)=1./'
              end
              object ProjectTimeStepEdit: TEdit
                Left = 93
                Top = 21
                Width = 41
                Height = 21
                TabOrder = 0
                Text = '2000'
                OnChange = ProjectTimeStepEditChange
                OnKeyPress = ProjectTimeStepEditKeyPress
              end
              object GlobalTimeStepEdit: TEdit
                Left = 220
                Top = 21
                Width = 44
                Height = 21
                TabOrder = 1
                Text = '2000'
                OnChange = GlobalTimeStepEditChange
                OnKeyPress = GlobalTimeStepEditKeyPress
              end
            end
            object CalculationModeRadioGroup: TRadioGroup
              Left = 1
              Top = 58
              Width = 546
              Height = 43
              Align = alTop
              Caption = ' Calculation Mode '
              Columns = 4
              ItemIndex = 0
              Items.Strings = (
                'Simple'
                'Real-Time'
                'By Signal'
                'Fastest')
              TabOrder = 1
              OnClick = CalculationModeRadioGroupClick
            end
            object InitAfterLoadCheckBox: TCheckBox
              Left = 6
              Top = 158
              Width = 101
              Height = 18
              Anchors = [akLeft, akBottom]
              Caption = 'Init After Load'
              TabOrder = 2
              OnClick = InitAfterLoadCheckBoxClick
            end
            object ResetAfterLoadCheckBox: TCheckBox
              Left = 102
              Top = 158
              Width = 102
              Height = 18
              Anchors = [akLeft, akBottom]
              Caption = 'Reset After Load'
              TabOrder = 3
              OnClick = ResetAfterLoadCheckBoxClick
            end
            object DebugModeCheckBox: TCheckBox
              Left = 215
              Top = 158
              Width = 53
              Height = 18
              Anchors = [akLeft, akBottom]
              Caption = 'Debug'
              TabOrder = 4
              OnClick = DebugModeCheckBoxClick
            end
            object GroupBox5: TGroupBox
              Left = 1
              Top = 101
              Width = 546
              Height = 56
              Align = alTop
              Caption = ' Min Intersteps Interval (ms)  '
              TabOrder = 5
              object MinInterstepsIntervalEdit: TEdit
                Left = 10
                Top = 21
                Width = 72
                Height = 21
                TabOrder = 0
                Text = '30'
                OnKeyPress = MinInterstepsIntervalEditKeyPress
              end
            end
          end
          object Panel3: TPanel
            Left = 1
            Top = 1
            Width = 548
            Height = 383
            Align = alClient
            TabOrder = 1
            object ModelPageControl: TPageControl
              Left = 1
              Top = 1
              Width = 546
              Height = 381
              ActivePage = ModelFromComponentTabSheet
              Align = alClient
              MultiLine = True
              TabOrder = 0
              OnChange = ModelPageControlChange
              object DontChangeTabSheet: TTabSheet
                Caption = 'Don'#39't Change'
                ImageIndex = 3
                object RichEdit1: TRichEdit
                  Left = 0
                  Top = 0
                  Width = 538
                  Height = 353
                  Align = alClient
                  Color = clBtnFace
                  Font.Charset = RUSSIAN_CHARSET
                  Font.Color = clWindowText
                  Font.Height = -12
                  Font.Name = 'Tahoma'
                  Font.Style = []
                  ParentFont = False
                  ReadOnly = True
                  TabOrder = 0
                  Zoom = 100
                end
                object ModelInfoRichEdit: TRichEdit
                  Left = 0
                  Top = 0
                  Width = 538
                  Height = 353
                  Margins.Left = 2
                  Margins.Top = 2
                  Margins.Right = 2
                  Margins.Bottom = 2
                  Align = alClient
                  Color = clBtnFace
                  Font.Charset = RUSSIAN_CHARSET
                  Font.Color = clWindowText
                  Font.Height = -11
                  Font.Name = 'Tahoma'
                  Font.Style = []
                  ParentFont = False
                  PlainText = True
                  TabOrder = 1
                  Zoom = 100
                end
              end
              object PredefinedModelTabSheet: TTabSheet
                Caption = 'Predefined Model'
                DesignSize = (
                  538
                  353)
                object PredefinedModelComboBox: TComboBox
                  Left = 6
                  Top = 16
                  Width = 527
                  Height = 21
                  Anchors = [akLeft, akTop, akRight]
                  TabOrder = 0
                  Text = '0'
                  OnChange = PredefinedModelComboBoxChange
                end
              end
              object ModelFromFileTabSheet: TTabSheet
                Caption = 'From File'
                ImageIndex = 1
                TabVisible = False
                DesignSize = (
                  538
                  353)
                object ProjectModelFileNameLabeledEdit: TLabeledEdit
                  Left = 3
                  Top = 19
                  Width = 448
                  Height = 21
                  Anchors = [akLeft, akTop, akRight]
                  EditLabel.Width = 74
                  EditLabel.Height = 13
                  EditLabel.Caption = 'Model file name'
                  TabOrder = 0
                end
                object OpenModelButton: TButton
                  Left = 457
                  Top = 17
                  Width = 79
                  Height = 26
                  Anchors = [akTop, akRight]
                  Caption = 'Browse'
                  TabOrder = 1
                  OnClick = OpenModelButtonClick
                end
              end
              object ModelFromComponentTabSheet: TTabSheet
                Caption = 'From Component'
                ImageIndex = 2
                object GroupBox2: TGroupBox
                  Left = 0
                  Top = 0
                  Width = 538
                  Height = 353
                  Align = alClient
                  Caption = ' Root Model Component Name '
                  TabOrder = 0
                  inline UClassesListFrame1: TUClassesListFrame
                    Left = 2
                    Top = 15
                    Width = 534
                    Height = 336
                    Align = alClient
                    DoubleBuffered = True
                    Font.Charset = DEFAULT_CHARSET
                    Font.Color = clWindowText
                    Font.Height = -12
                    Font.Name = 'Tahoma'
                    Font.Style = []
                    ParentDoubleBuffered = False
                    ParentFont = False
                    TabOrder = 0
                    ExplicitLeft = 2
                    ExplicitTop = 15
                    ExplicitWidth = 534
                    ExplicitHeight = 336
                    inherited PageControl: TPageControl
                      Width = 534
                      Height = 336
                      ActivePage = UClassesListFrame1.LibsTabSheet
                      ExplicitWidth = 534
                      ExplicitHeight = 336
                      inherited NameTabSheet: TTabSheet
                        ExplicitTop = 25
                        ExplicitWidth = 526
                        ExplicitHeight = 307
                        inherited StringGrid: TStringGrid
                          Width = 526
                          Height = 307
                          OnClick = UClassesListFrame1StringGridClick
                          ExplicitWidth = 526
                          ExplicitHeight = 307
                        end
                      end
                      inherited LibsTabSheet: TTabSheet
                        ExplicitLeft = 4
                        ExplicitTop = 25
                        ExplicitWidth = 526
                        ExplicitHeight = 307
                        inherited TreeView: TTreeView
                          Width = 526
                          Height = 307
                          OnChange = UClassesListFrame1TreeViewChange
                          ExplicitWidth = 526
                          ExplicitHeight = 307
                        end
                      end
                      inherited LibsControlTabSheet: TTabSheet
                        TabVisible = False
                        inherited Splitter1: TSplitter
                          Top = 291
                          ExplicitTop = 188
                          ExplicitWidth = 399
                        end
                        inherited Panel1: TPanel
                          Top = 386
                          Height = 129
                          Visible = False
                          ExplicitTop = 386
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
                          Height = 291
                          ExplicitHeight = 291
                          inherited LibsListStringGrid: TStringGrid
                            Top = 16
                            Height = 273
                            ExplicitTop = 16
                            ExplicitHeight = 273
                          end
                        end
                        inherited GroupBox2: TGroupBox
                          Top = 298
                          Height = 88
                          ExplicitTop = 298
                          ExplicitHeight = 88
                          inherited LibComponentListStringGrid: TStringGrid
                            Top = 16
                            Height = 70
                            OnClick = UClassesListFrame1LibComponentListStringGridClick
                            ExplicitTop = 16
                            ExplicitHeight = 70
                          end
                        end
                      end
                    end
                  end
                end
              end
            end
          end
        end
      end
      object ShowChannelsStateCheckBox: TCheckBox
        Left = 3
        Top = 632
        Width = 132
        Height = 18
        Anchors = [akLeft, akBottom]
        Caption = 'Show Channels State'
        Checked = True
        State = cbChecked
        TabOrder = 2
      end
    end
  end
  object OpenTextFileDialog: TOpenTextFileDialog
    DefaultExt = 'xml'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Select model file name'
    Left = 448
    Top = 32
  end
end
