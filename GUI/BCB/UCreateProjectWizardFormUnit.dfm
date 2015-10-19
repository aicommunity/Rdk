object UCreateProjectWizardForm: TUCreateProjectWizardForm
  Left = 0
  Top = 0
  Caption = 'Create Project Wizard'
  ClientHeight = 901
  ClientWidth = 762
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 17
  object Panel1: TPanel
    Left = 0
    Top = 847
    Width = 762
    Height = 54
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alBottom
    TabOrder = 0
    DesignSize = (
      762
      54)
    object PrevButton: TButton
      Left = 443
      Top = 9
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akTop, akRight]
      Caption = '<< Prev'
      TabOrder = 0
      OnClick = PrevButtonClick
    end
    object NextButton: TButton
      Left = 549
      Top = 9
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akTop, akRight]
      Caption = 'Next >>'
      TabOrder = 1
      OnClick = NextButtonClick
    end
    object FinishButton: TButton
      Left = 655
      Top = 9
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
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
    Width = 762
    Height = 847
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 1
    OnChange = PageControlChange
    object TabSheet1: TTabSheet
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Name'
      DesignSize = (
        754
        815)
      object Label1: TLabel
        Left = 4
        Top = 182
        Width = 116
        Height = 17
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Caption = 'Project Description'
      end
      object TitlePanel1: TPanel
        Left = 0
        Top = 0
        Width = 754
        Height = 54
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alTop
        Alignment = taLeftJustify
        Caption = 
          'Please select project directory and set optional information suc' +
          'h as project name and description'
        TabOrder = 4
      end
      object ProjectNameLabeledEdit: TLabeledEdit
        Left = 4
        Top = 139
        Width = 744
        Height = 25
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 83
        EditLabel.Height = 17
        EditLabel.Margins.Left = 4
        EditLabel.Margins.Top = 4
        EditLabel.Margins.Right = 4
        EditLabel.Margins.Bottom = 4
        EditLabel.Caption = 'Project Name'
        TabOrder = 2
        OnChange = ProjectNameLabeledEditChange
      end
      object ProjectDirectoryLabeledEdit: TLabeledEdit
        Left = 4
        Top = 77
        Width = 638
        Height = 25
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 105
        EditLabel.Height = 17
        EditLabel.Margins.Left = 4
        EditLabel.Margins.Top = 4
        EditLabel.Margins.Right = 4
        EditLabel.Margins.Bottom = 4
        EditLabel.Caption = 'Project Directory'
        TabOrder = 0
        OnChange = ProjectDirectoryLabeledEditChange
      end
      object Button1: TButton
        Left = 650
        Top = 74
        Width = 98
        Height = 32
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Anchors = [akTop, akRight]
        Caption = 'Browse'
        TabOrder = 1
        OnClick = Button1Click
      end
      object ProjectDescriptionRichEdit: TRichEdit
        Left = 4
        Top = 207
        Width = 744
        Height = 572
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Anchors = [akLeft, akTop, akRight, akBottom]
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -15
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        PlainText = True
        TabOrder = 3
        Zoom = 100
        OnChange = ProjectDescriptionRichEditChange
      end
      object ProjectAutoSaveFlagCheckBox: TCheckBox
        Left = 4
        Top = 787
        Width = 91
        Height = 22
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Anchors = [akLeft, akBottom]
        Caption = 'Autosave'
        Checked = True
        State = cbChecked
        TabOrder = 5
        OnClick = ProjectAutoSaveFlagCheckBoxClick
      end
      object ProjectAutoSaveStatesFlagCheckBox: TCheckBox
        Left = 98
        Top = 787
        Width = 133
        Height = 22
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Anchors = [akLeft, akBottom]
        Caption = 'Autosave States'
        Checked = True
        State = cbChecked
        TabOrder = 6
        OnClick = ProjectAutoSaveStatesFlagCheckBoxClick
      end
      object EventsLogFlagCheckBox: TCheckBox
        Left = 234
        Top = 787
        Width = 133
        Height = 22
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Anchors = [akLeft, akBottom]
        Caption = 'Log Events'
        Checked = True
        State = cbChecked
        TabOrder = 7
        OnClick = EventsLogFlagCheckBoxClick
      end
      object DisableStopVideoSourcesCheckBox: TCheckBox
        Left = 338
        Top = 787
        Width = 255
        Height = 22
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Anchors = [akLeft, akBottom]
        Caption = 'Disable Stop Video Sources'
        Checked = True
        State = cbChecked
        TabOrder = 8
        OnClick = DisableStopVideoSourcesCheckBoxClick
      end
    end
    object TabSheet2: TTabSheet
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Type'
      ImageIndex = 1
      object TitlePanel2: TPanel
        Left = 0
        Top = 0
        Width = 754
        Height = 54
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alTop
        Alignment = taLeftJustify
        Caption = 'Select calculation options'
        TabOrder = 0
      end
      object ProjectTypeRadioGroup: TRadioGroup
        Left = 0
        Top = 109
        Width = 754
        Height = 52
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alTop
        Caption = 'Project type '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'Universal'
          'Video analysis')
        TabOrder = 1
        OnClick = ProjectTypeRadioGroupClick
      end
      object CalculationSourceTimeModeRadioGroup: TRadioGroup
        Left = 0
        Top = 213
        Width = 754
        Height = 52
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alTop
        Caption = ' Calculation Time Source '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'System Time'
          'External Data Time')
        TabOrder = 2
        OnClick = CalculationSourceTimeModeRadioGroupClick
      end
      object ProjectModeRadioGroup: TRadioGroup
        Left = 0
        Top = 54
        Width = 754
        Height = 55
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
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
        Top = 161
        Width = 754
        Height = 52
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
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
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = 'Model'
      ImageIndex = 2
      DesignSize = (
        754
        815)
      object TitlePanel3: TPanel
        Left = 0
        Top = 0
        Width = 754
        Height = 54
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alTop
        Alignment = taLeftJustify
        Caption = 'Channels configuration'
        TabOrder = 0
        DesignSize = (
          754
          54)
        object ChannelsNumberLabeledEdit: TLabeledEdit
          Left = 613
          Top = 18
          Width = 113
          Height = 25
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Anchors = [akTop, akRight]
          EditLabel.Width = 108
          EditLabel.Height = 17
          EditLabel.Margins.Left = 4
          EditLabel.Margins.Top = 4
          EditLabel.Margins.Right = 4
          EditLabel.Margins.Bottom = 4
          EditLabel.Caption = 'Channels Number'
          TabOrder = 0
          Text = '1'
          OnChange = ChannelsNumberLabeledEditChange
        end
        object UpDown1: TUpDown
          Left = 726
          Top = 18
          Width = 21
          Height = 25
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
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
        Top = 54
        Width = 754
        Height = 728
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alTop
        Anchors = [akLeft, akTop, akRight, akBottom]
        Caption = ' Channel Properties'
        TabOrder = 1
        object ChannelsStringGrid: TStringGrid
          Left = 2
          Top = 19
          Width = 61
          Height = 707
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alLeft
          ColCount = 1
          DefaultColWidth = 24
          FixedCols = 0
          FixedRows = 0
          TabOrder = 0
          OnClick = ChannelsStringGridClick
        end
        object Panel4: TPanel
          Left = 63
          Top = 19
          Width = 689
          Height = 707
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Align = alClient
          TabOrder = 1
          object Panel2: TPanel
            Left = 1
            Top = 480
            Width = 687
            Height = 226
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Align = alBottom
            TabOrder = 0
            DesignSize = (
              687
              226)
            object ProjectTimeStepGroupBox: TGroupBox
              Left = 1
              Top = 1
              Width = 685
              Height = 71
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              Align = alTop
              Caption = ' Time step duration  '
              TabOrder = 0
              object Label2: TLabel
                Left = 13
                Top = 30
                Width = 92
                Height = 17
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Caption = 'T (Default)=1./'
              end
              object Label3: TLabel
                Left = 182
                Top = 30
                Width = 85
                Height = 17
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Caption = 'T (Global)=1./'
              end
              object ProjectTimeStepEdit: TEdit
                Left = 116
                Top = 26
                Width = 51
                Height = 25
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                TabOrder = 0
                Text = '30'
                OnKeyPress = ProjectTimeStepEditKeyPress
              end
              object GlobalTimeStepEdit: TEdit
                Left = 275
                Top = 26
                Width = 55
                Height = 25
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                TabOrder = 1
                Text = '30'
                OnKeyPress = GlobalTimeStepEditKeyPress
              end
            end
            object CalculationModeRadioGroup: TRadioGroup
              Left = 1
              Top = 72
              Width = 685
              Height = 54
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              Align = alTop
              Caption = ' Calculation Mode '
              Columns = 3
              ItemIndex = 0
              Items.Strings = (
                'Simple'
                'Real-Time'
                'By Signal')
              TabOrder = 1
              OnClick = CalculationModeRadioGroupClick
            end
            object InitAfterLoadCheckBox: TCheckBox
              Left = 7
              Top = 197
              Width = 127
              Height = 23
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              Anchors = [akLeft, akBottom]
              Caption = 'Init After Load'
              TabOrder = 2
              OnClick = InitAfterLoadCheckBoxClick
            end
            object ResetAfterLoadCheckBox: TCheckBox
              Left = 128
              Top = 197
              Width = 127
              Height = 23
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              Anchors = [akLeft, akBottom]
              Caption = 'Reset After Load'
              TabOrder = 3
              OnClick = ResetAfterLoadCheckBoxClick
            end
            object DebugModeCheckBox: TCheckBox
              Left = 269
              Top = 197
              Width = 66
              Height = 23
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              Anchors = [akLeft, akBottom]
              Caption = 'Debug'
              TabOrder = 4
              OnClick = DebugModeCheckBoxClick
            end
            object GroupBox5: TGroupBox
              Left = 1
              Top = 126
              Width = 685
              Height = 70
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              Align = alTop
              Caption = ' Min Intersteps Interval (ms)  '
              TabOrder = 5
              object MinInterstepsIntervalEdit: TEdit
                Left = 13
                Top = 26
                Width = 90
                Height = 25
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                TabOrder = 0
                Text = '30'
                OnKeyPress = MinInterstepsIntervalEditKeyPress
              end
            end
          end
          object Panel3: TPanel
            Left = 1
            Top = 1
            Width = 687
            Height = 479
            Margins.Left = 4
            Margins.Top = 4
            Margins.Right = 4
            Margins.Bottom = 4
            Align = alClient
            TabOrder = 1
            object ModelPageControl: TPageControl
              Left = 1
              Top = 1
              Width = 685
              Height = 477
              Margins.Left = 4
              Margins.Top = 4
              Margins.Right = 4
              Margins.Bottom = 4
              ActivePage = ModelFromComponentTabSheet
              Align = alClient
              MultiLine = True
              TabOrder = 0
              OnChange = ModelPageControlChange
              object DontChangeTabSheet: TTabSheet
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Caption = 'Don'#39't Change'
                ImageIndex = 3
                object RichEdit1: TRichEdit
                  Left = 0
                  Top = 0
                  Width = 677
                  Height = 445
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  Align = alClient
                  Color = clBtnFace
                  Font.Charset = RUSSIAN_CHARSET
                  Font.Color = clWindowText
                  Font.Height = -15
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
                  Width = 677
                  Height = 445
                  Align = alClient
                  Color = clBtnFace
                  Font.Charset = RUSSIAN_CHARSET
                  Font.Color = clWindowText
                  Font.Height = -14
                  Font.Name = 'Tahoma'
                  Font.Style = []
                  ParentFont = False
                  PlainText = True
                  TabOrder = 1
                  Zoom = 100
                end
              end
              object PredefinedModelTabSheet: TTabSheet
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Caption = 'Predefined Model'
                DesignSize = (
                  677
                  445)
                object PredefinedModelComboBox: TComboBox
                  Left = 8
                  Top = 20
                  Width = 658
                  Height = 25
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  Anchors = [akLeft, akTop, akRight]
                  TabOrder = 0
                  Text = '0'
                  OnChange = PredefinedModelComboBoxChange
                end
              end
              object ModelFromFileTabSheet: TTabSheet
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Caption = 'From File'
                ImageIndex = 1
                DesignSize = (
                  677
                  445)
                object ProjectModelFileNameLabeledEdit: TLabeledEdit
                  Left = 4
                  Top = 24
                  Width = 560
                  Height = 25
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  Anchors = [akLeft, akTop, akRight]
                  EditLabel.Width = 92
                  EditLabel.Height = 17
                  EditLabel.Margins.Left = 4
                  EditLabel.Margins.Top = 4
                  EditLabel.Margins.Right = 4
                  EditLabel.Margins.Bottom = 4
                  EditLabel.Caption = 'Model file name'
                  TabOrder = 0
                end
                object OpenModelButton: TButton
                  Left = 571
                  Top = 21
                  Width = 99
                  Height = 33
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  Anchors = [akTop, akRight]
                  Caption = 'Browse'
                  TabOrder = 1
                  OnClick = OpenModelButtonClick
                end
              end
              object ModelFromComponentTabSheet: TTabSheet
                Margins.Left = 4
                Margins.Top = 4
                Margins.Right = 4
                Margins.Bottom = 4
                Caption = 'From Component'
                ImageIndex = 2
                object GroupBox2: TGroupBox
                  Left = 0
                  Top = 0
                  Width = 677
                  Height = 445
                  Margins.Left = 4
                  Margins.Top = 4
                  Margins.Right = 4
                  Margins.Bottom = 4
                  Align = alClient
                  Caption = ' Root Model Component Name '
                  TabOrder = 0
                  inline UClassesListFrame1: TUClassesListFrame
                    Left = 2
                    Top = 19
                    Width = 673
                    Height = 424
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
                    ExplicitLeft = 2
                    ExplicitTop = 19
                    ExplicitWidth = 673
                    ExplicitHeight = 424
                    inherited PageControl: TPageControl
                      Width = 673
                      Height = 424
                      Margins.Left = 4
                      Margins.Top = 4
                      Margins.Right = 4
                      Margins.Bottom = 4
                      ExplicitWidth = 673
                      ExplicitHeight = 424
                      inherited NameTabSheet: TTabSheet
                        Margins.Left = 4
                        Margins.Top = 4
                        Margins.Right = 4
                        Margins.Bottom = 4
                        ExplicitLeft = 4
                        ExplicitTop = 29
                        ExplicitWidth = 665
                        ExplicitHeight = 391
                        inherited StringGrid: TStringGrid
                          Width = 665
                          Height = 391
                          Margins.Left = 4
                          Margins.Top = 4
                          Margins.Right = 4
                          Margins.Bottom = 4
                          OnClick = UClassesListFrame1StringGridClick
                          ExplicitWidth = 665
                          ExplicitHeight = 391
                        end
                      end
                      inherited LibsTabSheet: TTabSheet
                        Margins.Left = 4
                        Margins.Top = 4
                        Margins.Right = 4
                        Margins.Bottom = 4
                        TabVisible = False
                        inherited TreeView: TTreeView
                          Margins.Left = 4
                          Margins.Top = 4
                          Margins.Right = 4
                          Margins.Bottom = 4
                        end
                      end
                      inherited LibsControlTabSheet: TTabSheet
                        Margins.Left = 4
                        Margins.Top = 4
                        Margins.Right = 4
                        Margins.Bottom = 4
                        ExplicitLeft = 4
                        ExplicitTop = 24
                        ExplicitWidth = 499
                        ExplicitHeight = 515
                        inherited Splitter1: TSplitter
                          Top = 235
                          Height = 9
                          Margins.Left = 4
                          Margins.Top = 4
                          Margins.Right = 4
                          Margins.Bottom = 4
                          ExplicitTop = 0
                          ExplicitWidth = 654
                          ExplicitHeight = 9
                        end
                        inherited Panel1: TPanel
                          Top = 354
                          Height = 161
                          Margins.Left = 4
                          Margins.Top = 4
                          Margins.Right = 4
                          Margins.Bottom = 4
                          Visible = False
                          ExplicitTop = 354
                          ExplicitHeight = 161
                          inherited LoadLibraryButton: TButton
                            Height = 33
                            Margins.Left = 4
                            Margins.Top = 4
                            Margins.Right = 4
                            Margins.Bottom = 4
                            ExplicitHeight = 33
                          end
                          inherited CreateRuntimeLibraryButton: TButton
                            Top = 34
                            Height = 33
                            Margins.Left = 4
                            Margins.Top = 4
                            Margins.Right = 4
                            Margins.Bottom = 4
                            ExplicitTop = 34
                            ExplicitHeight = 33
                          end
                          inherited DeleteLibraryButton: TButton
                            Top = 67
                            Height = 32
                            Margins.Left = 4
                            Margins.Top = 4
                            Margins.Right = 4
                            Margins.Bottom = 4
                            ExplicitTop = 67
                            ExplicitHeight = 32
                          end
                          inherited RenameRuntimeLibraryButton: TButton
                            Top = 99
                            Height = 33
                            Margins.Left = 4
                            Margins.Top = 4
                            Margins.Right = 4
                            Margins.Bottom = 4
                            ExplicitTop = 99
                            ExplicitHeight = 33
                          end
                          inherited AddClassButton: TButton
                            Top = 132
                            Height = 33
                            Margins.Left = 4
                            Margins.Top = 4
                            Margins.Right = 4
                            Margins.Bottom = 4
                            ExplicitTop = 132
                            ExplicitHeight = 33
                          end
                        end
                        inherited GroupBox1: TGroupBox
                          Height = 235
                          Margins.Left = 4
                          Margins.Top = 4
                          Margins.Right = 4
                          Margins.Bottom = 4
                          ExplicitHeight = 235
                          inherited LibsListStringGrid: TStringGrid
                            Top = 20
                            Height = 213
                            Margins.Left = 4
                            Margins.Top = 4
                            Margins.Right = 4
                            Margins.Bottom = 4
                            ExplicitTop = 20
                            ExplicitHeight = 213
                          end
                        end
                        inherited GroupBox2: TGroupBox
                          Top = 244
                          Height = 110
                          Margins.Left = 4
                          Margins.Top = 4
                          Margins.Right = 4
                          Margins.Bottom = 4
                          ExplicitTop = 244
                          ExplicitHeight = 110
                          inherited LibComponentListStringGrid: TStringGrid
                            Top = 20
                            Height = 88
                            Margins.Left = 4
                            Margins.Top = 4
                            Margins.Right = 4
                            Margins.Bottom = 4
                            OnClick = UClassesListFrame1LibComponentListStringGridClick
                            ExplicitTop = 20
                            ExplicitHeight = 88
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
        Left = 4
        Top = 790
        Width = 165
        Height = 22
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
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
