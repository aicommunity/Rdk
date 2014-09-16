object UCreateProjectWizardForm: TUCreateProjectWizardForm
  Left = 0
  Top = 0
  Caption = 'Create Project Wizard'
  ClientHeight = 689
  ClientWidth = 583
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
    Top = 648
    Width = 583
    Height = 41
    Align = alBottom
    TabOrder = 0
    ExplicitTop = 450
    ExplicitWidth = 527
    DesignSize = (
      583
      41)
    object PrevButton: TButton
      Left = 339
      Top = 7
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = '<< Prev'
      TabOrder = 0
      OnClick = PrevButtonClick
    end
    object NextButton: TButton
      Left = 420
      Top = 7
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Next >>'
      TabOrder = 1
      OnClick = NextButtonClick
    end
    object FinishButton: TButton
      Left = 501
      Top = 7
      Width = 75
      Height = 25
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
    Width = 583
    Height = 648
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 1
    OnChange = PageControlChange
    ExplicitWidth = 527
    ExplicitHeight = 450
    object TabSheet1: TTabSheet
      Caption = 'Name'
      ExplicitWidth = 519
      ExplicitHeight = 422
      DesignSize = (
        575
        620)
      object Label1: TLabel
        Left = 3
        Top = 139
        Width = 90
        Height = 13
        Caption = 'Project Description'
      end
      object TitlePanel1: TPanel
        Left = 0
        Top = 0
        Width = 575
        Height = 41
        Align = alTop
        Alignment = taLeftJustify
        Caption = 
          'Please select project directory and set optional information suc' +
          'h as project name and description'
        TabOrder = 4
        ExplicitWidth = 519
      end
      object ProjectNameLabeledEdit: TLabeledEdit
        Left = 3
        Top = 106
        Width = 569
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 64
        EditLabel.Height = 13
        EditLabel.Caption = 'Project Name'
        TabOrder = 2
        ExplicitWidth = 513
      end
      object ProjectDirectoryLabeledEdit: TLabeledEdit
        Left = 3
        Top = 59
        Width = 488
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 81
        EditLabel.Height = 13
        EditLabel.Caption = 'Project Directory'
        TabOrder = 0
        ExplicitWidth = 432
      end
      object Button1: TButton
        Left = 441
        Top = 57
        Width = 75
        Height = 25
        Caption = 'Browse'
        TabOrder = 1
        OnClick = Button1Click
      end
      object ProjectDescriptionRichEdit: TRichEdit
        Left = 3
        Top = 158
        Width = 569
        Height = 438
        Anchors = [akLeft, akTop, akRight, akBottom]
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        PlainText = True
        TabOrder = 3
        ExplicitWidth = 513
        ExplicitHeight = 240
      end
      object ProjectAutoSaveFlagCheckBox: TCheckBox
        Left = 3
        Top = 602
        Width = 70
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Autosave'
        Checked = True
        State = cbChecked
        TabOrder = 5
        ExplicitTop = 404
      end
      object ProjectAutoSaveStatesFlagCheckBox: TCheckBox
        Left = 75
        Top = 602
        Width = 102
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Autosave States'
        Checked = True
        State = cbChecked
        TabOrder = 6
        ExplicitTop = 404
      end
      object EventsLogFlagCheckBox: TCheckBox
        Left = 179
        Top = 602
        Width = 102
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Log Events'
        Checked = True
        State = cbChecked
        TabOrder = 7
        ExplicitTop = 404
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Type'
      ImageIndex = 1
      ExplicitWidth = 519
      ExplicitHeight = 422
      object TitlePanel2: TPanel
        Left = 0
        Top = 0
        Width = 575
        Height = 41
        Align = alTop
        Alignment = taLeftJustify
        Caption = 'Select calculation options'
        TabOrder = 0
        ExplicitWidth = 519
      end
      object ProjectTypeRadioGroup: TRadioGroup
        Left = 0
        Top = 83
        Width = 575
        Height = 40
        Align = alTop
        Caption = 'Project type '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'Universal'
          'Video analysis')
        TabOrder = 1
        OnClick = ProjectTypeRadioGroupClick
        ExplicitWidth = 519
      end
      object CalculationSourceTimeModeRadioGroup: TRadioGroup
        Left = 0
        Top = 163
        Width = 575
        Height = 40
        Align = alTop
        Caption = ' Calculation Time Source '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'System Time'
          'External Data Time')
        TabOrder = 2
        OnClick = ProjectTypeRadioGroupClick
        ExplicitWidth = 519
      end
      object ProjectModeRadioGroup: TRadioGroup
        Left = 0
        Top = 41
        Width = 575
        Height = 42
        Align = alTop
        Caption = ' Project Mode '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'Simple'
          'Server')
        TabOrder = 3
        ExplicitWidth = 519
      end
      object MultiThreadingModeRadioGroup: TRadioGroup
        Left = 0
        Top = 123
        Width = 575
        Height = 40
        Align = alTop
        Caption = ' Multi-Threading Mode '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'Single Thread'
          'Multi Threads')
        TabOrder = 4
        OnClick = ProjectTypeRadioGroupClick
        ExplicitWidth = 519
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Model'
      ImageIndex = 2
      ExplicitWidth = 519
      ExplicitHeight = 422
      DesignSize = (
        575
        620)
      object TitlePanel3: TPanel
        Left = 0
        Top = 0
        Width = 575
        Height = 41
        Align = alTop
        Alignment = taLeftJustify
        Caption = 'Channels configuration'
        TabOrder = 0
        ExplicitWidth = 519
        DesignSize = (
          575
          41)
        object ChannelsNumberLabeledEdit: TLabeledEdit
          Left = 469
          Top = 14
          Width = 86
          Height = 21
          Anchors = [akTop, akRight]
          EditLabel.Width = 84
          EditLabel.Height = 13
          EditLabel.Caption = 'Channels Number'
          TabOrder = 0
          Text = '1'
          ExplicitLeft = 413
        end
        object UpDown1: TUpDown
          Left = 555
          Top = 14
          Width = 16
          Height = 21
          Anchors = [akTop, akRight]
          Associate = ChannelsNumberLabeledEdit
          Min = 1
          Max = 1000
          Position = 1
          TabOrder = 1
          ExplicitLeft = 499
        end
      end
      object GroupBox4: TGroupBox
        Left = 0
        Top = 41
        Width = 575
        Height = 557
        Align = alTop
        Anchors = [akLeft, akTop, akRight, akBottom]
        Caption = ' Channel Properties'
        TabOrder = 1
        ExplicitWidth = 519
        ExplicitHeight = 359
        object StringGrid1: TStringGrid
          Left = 2
          Top = 15
          Width = 47
          Height = 540
          Align = alLeft
          ColCount = 1
          DefaultColWidth = 24
          FixedCols = 0
          FixedRows = 0
          TabOrder = 0
          ExplicitHeight = 342
        end
        object Panel4: TPanel
          Left = 49
          Top = 15
          Width = 524
          Height = 540
          Align = alClient
          TabOrder = 1
          ExplicitWidth = 468
          ExplicitHeight = 342
          object Panel2: TPanel
            Left = 1
            Top = 366
            Width = 522
            Height = 173
            Align = alBottom
            TabOrder = 0
            ExplicitTop = 168
            ExplicitWidth = 466
            DesignSize = (
              522
              173)
            object ProjectTimeStepGroupBox: TGroupBox
              Left = 1
              Top = 1
              Width = 520
              Height = 54
              Align = alTop
              Caption = ' Time step duration  '
              TabOrder = 0
              ExplicitWidth = 464
              object Label2: TLabel
                Left = 10
                Top = 23
                Width = 74
                Height = 13
                Caption = 'T (Default)=1./'
              end
              object Label3: TLabel
                Left = 139
                Top = 23
                Width = 68
                Height = 13
                Caption = 'T (Global)=1./'
              end
              object ProjectTimeStepEdit: TEdit
                Left = 89
                Top = 20
                Width = 39
                Height = 21
                TabOrder = 0
                Text = '30'
              end
              object Edit1: TEdit
                Left = 216
                Top = 20
                Width = 42
                Height = 21
                TabOrder = 1
                Text = '30'
              end
            end
            object CalculationModeRadioGroup: TRadioGroup
              Left = 1
              Top = 55
              Width = 520
              Height = 41
              Align = alTop
              Caption = ' Calculation Mode '
              Columns = 3
              ItemIndex = 0
              Items.Strings = (
                'Simple'
                'Real-Time'
                'By Signal')
              TabOrder = 1
              ExplicitWidth = 464
            end
            object InitAfterLoadCheckBox: TCheckBox
              Left = 6
              Top = 151
              Width = 97
              Height = 17
              Anchors = [akLeft, akBottom]
              Caption = 'Init After Load'
              TabOrder = 2
            end
            object ResetAfterLoadCheckBox: TCheckBox
              Left = 98
              Top = 151
              Width = 97
              Height = 17
              Anchors = [akLeft, akBottom]
              Caption = 'Reset After Load'
              TabOrder = 3
            end
            object DebugModeCheckBox: TCheckBox
              Left = 206
              Top = 151
              Width = 50
              Height = 17
              Anchors = [akLeft, akBottom]
              Caption = 'Debug'
              TabOrder = 4
            end
            object GroupBox5: TGroupBox
              Left = 1
              Top = 96
              Width = 520
              Height = 54
              Align = alTop
              Caption = ' Min Intersteps Interval (ms)  '
              TabOrder = 5
              ExplicitWidth = 464
              object MinInterstepsIntervalEdit: TEdit
                Left = 10
                Top = 20
                Width = 69
                Height = 21
                TabOrder = 0
                Text = '30'
              end
            end
          end
          object Panel3: TPanel
            Left = 1
            Top = 1
            Width = 522
            Height = 365
            Align = alClient
            TabOrder = 1
            ExplicitWidth = 466
            ExplicitHeight = 167
            object ModelPageControl: TPageControl
              Left = 1
              Top = 1
              Width = 520
              Height = 363
              ActivePage = ModelFromComponentTabSheet
              Align = alClient
              MultiLine = True
              TabOrder = 0
              ExplicitWidth = 641
              object DontChangeTabSheet: TTabSheet
                Caption = 'Don'#39't Change'
                ImageIndex = 3
                ExplicitWidth = 633
                object RichEdit1: TRichEdit
                  Left = 0
                  Top = 0
                  Width = 512
                  Height = 335
                  Align = alClient
                  Color = clBtnFace
                  Font.Charset = RUSSIAN_CHARSET
                  Font.Color = clWindowText
                  Font.Height = -11
                  Font.Name = 'Tahoma'
                  Font.Style = []
                  ParentFont = False
                  ReadOnly = True
                  TabOrder = 0
                  ExplicitLeft = 152
                  ExplicitTop = 112
                  ExplicitWidth = 185
                  ExplicitHeight = 89
                end
              end
              object PredefinedModelTabSheet: TTabSheet
                Caption = 'Predefined Model'
                ExplicitWidth = 633
                DesignSize = (
                  512
                  335)
                object PredefinedModelComboBox: TComboBox
                  Left = 6
                  Top = 15
                  Width = 503
                  Height = 21
                  Anchors = [akLeft, akTop, akRight]
                  TabOrder = 0
                  Text = '0'
                  ExplicitWidth = 624
                end
              end
              object ModelFromFileTabSheet: TTabSheet
                Caption = 'From File'
                ImageIndex = 1
                ExplicitWidth = 633
                DesignSize = (
                  512
                  335)
                object ProjectModelFileNameLabeledEdit: TLabeledEdit
                  Left = 3
                  Top = 18
                  Width = 428
                  Height = 21
                  Anchors = [akLeft, akTop, akRight]
                  EditLabel.Width = 74
                  EditLabel.Height = 13
                  EditLabel.Caption = 'Model file name'
                  TabOrder = 0
                end
                object OpenModelButton: TButton
                  Left = 437
                  Top = 16
                  Width = 75
                  Height = 25
                  Anchors = [akTop, akRight]
                  Caption = 'Browse'
                  TabOrder = 1
                  OnClick = OpenModelButtonClick
                  ExplicitLeft = 558
                end
              end
              object ModelFromComponentTabSheet: TTabSheet
                Caption = 'From Component'
                ImageIndex = 2
                ExplicitWidth = 633
                object GroupBox2: TGroupBox
                  Left = 0
                  Top = 0
                  Width = 512
                  Height = 335
                  Align = alClient
                  Caption = ' Root Model Component Name '
                  TabOrder = 0
                  ExplicitWidth = 633
                  inline UClassesListFrame1: TUClassesListFrame
                    Left = 2
                    Top = 15
                    Width = 508
                    Height = 318
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
                    ExplicitWidth = 629
                    ExplicitHeight = 318
                    inherited PageControl: TPageControl
                      Width = 508
                      Height = 318
                      ActivePage = UClassesListFrame1.NameTabSheet
                      ExplicitWidth = 629
                      ExplicitHeight = 318
                      inherited NameTabSheet: TTabSheet
                        ExplicitLeft = 4
                        ExplicitTop = 24
                        ExplicitWidth = 621
                        ExplicitHeight = 290
                        inherited StringGrid: TStringGrid
                          Width = 500
                          Height = 290
                          ExplicitWidth = 621
                          ExplicitHeight = 290
                        end
                      end
                      inherited LibsTabSheet: TTabSheet
                        ExplicitLeft = 4
                        ExplicitTop = 24
                        ExplicitWidth = 499
                        ExplicitHeight = 515
                        inherited TreeView: TTreeView
                          Width = 500
                          Height = 290
                        end
                      end
                      inherited LibsControlTabSheet: TTabSheet
                        ExplicitLeft = 4
                        ExplicitTop = 24
                        ExplicitWidth = 499
                        ExplicitHeight = 515
                        inherited Splitter1: TSplitter
                          Top = 0
                          Width = 500
                          ExplicitTop = -203
                          ExplicitWidth = 507
                        end
                        inherited Panel1: TPanel
                          Top = 167
                          Width = 500
                          inherited LoadLibraryButton: TButton
                            Width = 498
                          end
                          inherited CreateRuntimeLibraryButton: TButton
                            Width = 498
                          end
                          inherited DeleteLibraryButton: TButton
                            Width = 498
                          end
                          inherited RenameRuntimeLibraryButton: TButton
                            Width = 498
                          end
                          inherited AddClassButton: TButton
                            Width = 498
                            ExplicitLeft = 1
                            ExplicitTop = 101
                          end
                        end
                        inherited GroupBox1: TGroupBox
                          Width = 500
                          Height = 0
                          ExplicitHeight = 225
                          inherited LibsListStringGrid: TStringGrid
                            Width = 496
                            ExplicitHeight = 208
                          end
                        end
                        inherited GroupBox2: TGroupBox
                          Top = 7
                          Width = 500
                          ExplicitTop = 232
                          inherited LibComponentListStringGrid: TStringGrid
                            Width = 496
                            ExplicitHeight = 143
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
        Top = 604
        Width = 126
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Show Channels State'
        Checked = True
        State = cbChecked
        TabOrder = 2
        ExplicitTop = 406
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
