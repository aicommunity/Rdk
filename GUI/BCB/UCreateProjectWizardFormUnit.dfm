object UCreateProjectWizardForm: TUCreateProjectWizardForm
  Left = 0
  Top = 0
  Caption = 'Create Project Wizard'
  ClientHeight = 491
  ClientWidth = 527
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
    Top = 450
    Width = 527
    Height = 41
    Align = alBottom
    TabOrder = 0
    object PrevButton: TButton
      Left = 278
      Top = 6
      Width = 75
      Height = 25
      Caption = '<< Prev'
      TabOrder = 0
      OnClick = PrevButtonClick
    end
    object NextButton: TButton
      Left = 359
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Next >>'
      TabOrder = 1
      OnClick = NextButtonClick
    end
    object FinishButton: TButton
      Left = 440
      Top = 6
      Width = 75
      Height = 25
      Caption = 'Finish'
      Default = True
      TabOrder = 2
      OnClick = FinishButtonClick
    end
  end
  object PageControl: TPageControl
    Left = 0
    Top = 0
    Width = 527
    Height = 450
    ActivePage = TabSheet1
    Align = alClient
    TabOrder = 1
    OnChange = PageControlChange
    object TabSheet1: TTabSheet
      Caption = 'Name'
      DesignSize = (
        519
        422)
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
        Width = 519
        Height = 41
        Align = alTop
        Alignment = taLeftJustify
        Caption = 
          'Please select project directory and set optional information suc' +
          'h as project name and description'
        TabOrder = 4
      end
      object ProjectNameLabeledEdit: TLabeledEdit
        Left = 3
        Top = 106
        Width = 513
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 64
        EditLabel.Height = 13
        EditLabel.Caption = 'Project Name'
        TabOrder = 2
      end
      object ProjectDirectoryLabeledEdit: TLabeledEdit
        Left = 3
        Top = 59
        Width = 432
        Height = 21
        Anchors = [akLeft, akTop, akRight]
        EditLabel.Width = 81
        EditLabel.Height = 13
        EditLabel.Caption = 'Project Directory'
        TabOrder = 0
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
        Width = 513
        Height = 240
        Anchors = [akLeft, akTop, akRight, akBottom]
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        PlainText = True
        TabOrder = 3
      end
      object ProjectAutoSaveFlagCheckBox: TCheckBox
        Left = 3
        Top = 404
        Width = 70
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Autosave'
        Checked = True
        State = cbChecked
        TabOrder = 5
      end
      object ProjectAutoSaveStatesFlagCheckBox: TCheckBox
        Left = 75
        Top = 404
        Width = 102
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Autosave States'
        Checked = True
        State = cbChecked
        TabOrder = 6
      end
      object EventsLogFlagCheckBox: TCheckBox
        Left = 179
        Top = 404
        Width = 102
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Log Events'
        Checked = True
        State = cbChecked
        TabOrder = 7
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Type'
      ImageIndex = 1
      object TitlePanel2: TPanel
        Left = 0
        Top = 0
        Width = 519
        Height = 41
        Align = alTop
        Alignment = taLeftJustify
        Caption = 'Select calculation options'
        TabOrder = 0
      end
      object ProjectTypeRadioGroup: TRadioGroup
        Left = 0
        Top = 83
        Width = 519
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
      end
      object ProjectCalculationModeRadioGroup: TRadioGroup
        Left = 0
        Top = 203
        Width = 519
        Height = 40
        Align = alTop
        Caption = ' Calculation mode '
        Columns = 3
        ItemIndex = 2
        Items.Strings = (
          'Sequential'
          'Real-time simulation'
          'By "Data Ready" signal')
        TabOrder = 2
      end
      object VideoAnalysisGroupBox: TGroupBox
        Left = 0
        Top = 243
        Width = 519
        Height = 94
        Align = alTop
        Caption = ' Video Analysis Additional Parameters '
        TabOrder = 3
        Visible = False
        DesignSize = (
          519
          94)
        object ImageWidthLabeledEdit: TLabeledEdit
          Left = 3
          Top = 34
          Width = 70
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          EditLabel.Width = 61
          EditLabel.Height = 13
          EditLabel.Caption = 'Image Width'
          TabOrder = 0
          Text = '640'
        end
        object ImageHeightLabeledEdit: TLabeledEdit
          Left = 82
          Top = 34
          Width = 70
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          EditLabel.Width = 64
          EditLabel.Height = 13
          EditLabel.Caption = 'Image Height'
          TabOrder = 1
          Text = '480'
        end
        object NumInputsLabeledEdit: TLabeledEdit
          Left = 164
          Top = 34
          Width = 70
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          EditLabel.Width = 55
          EditLabel.Height = 13
          EditLabel.Caption = 'Num Inputs'
          TabOrder = 2
          Text = '1'
        end
        object NumOutputsLabeledEdit: TLabeledEdit
          Left = 242
          Top = 34
          Width = 70
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          EditLabel.Width = 63
          EditLabel.Height = 13
          EditLabel.Caption = 'Num Outputs'
          TabOrder = 3
          Text = '1'
        end
        object UpendInputImageCheckBox: TCheckBox
          Left = 5
          Top = 63
          Width = 316
          Height = 17
          Caption = 'Reflect Input Image'
          Checked = True
          State = cbChecked
          TabOrder = 4
        end
      end
      object CalculationSourceTimeModeRadioGroup: TRadioGroup
        Left = 0
        Top = 163
        Width = 519
        Height = 40
        Align = alTop
        Caption = ' Calculation time source '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'System Time'
          'External Data Time')
        TabOrder = 4
        OnClick = ProjectTypeRadioGroupClick
      end
      object ProjectModeRadioGroup: TRadioGroup
        Left = 0
        Top = 41
        Width = 519
        Height = 42
        Align = alTop
        Caption = ' Project Mode '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'Simple'
          'Server')
        TabOrder = 5
      end
      object MultiThreadingModeRadioGroup: TRadioGroup
        Left = 0
        Top = 123
        Width = 519
        Height = 40
        Align = alTop
        Caption = ' Multi-Threading Mode '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'Single Thread'
          'Multi Threads')
        TabOrder = 6
        OnClick = ProjectTypeRadioGroupClick
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Model'
      ImageIndex = 2
      DesignSize = (
        519
        422)
      object TitlePanel3: TPanel
        Left = 0
        Top = 0
        Width = 519
        Height = 41
        Align = alTop
        Alignment = taLeftJustify
        Caption = 'Channels configuration'
        TabOrder = 0
        DesignSize = (
          519
          41)
        object ChannelsNumberLabeledEdit: TLabeledEdit
          Left = 413
          Top = 14
          Width = 86
          Height = 21
          Anchors = [akTop, akRight]
          EditLabel.Width = 84
          EditLabel.Height = 13
          EditLabel.Caption = 'Channels Number'
          TabOrder = 0
          Text = '1'
        end
        object UpDown1: TUpDown
          Left = 499
          Top = 14
          Width = 16
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
        Top = 41
        Width = 519
        Height = 359
        Align = alTop
        Anchors = [akLeft, akTop, akRight, akBottom]
        Caption = ' Channel Properties'
        TabOrder = 1
        object StringGrid1: TStringGrid
          Left = 2
          Top = 15
          Width = 47
          Height = 342
          Align = alLeft
          ColCount = 1
          DefaultColWidth = 24
          FixedCols = 0
          FixedRows = 0
          TabOrder = 0
          ExplicitLeft = -4
          ExplicitTop = 17
        end
        object Panel4: TPanel
          Left = 49
          Top = 15
          Width = 468
          Height = 342
          Align = alClient
          TabOrder = 1
          ExplicitLeft = 216
          ExplicitTop = 48
          ExplicitWidth = 185
          ExplicitHeight = 41
          object Panel2: TPanel
            Left = 1
            Top = 168
            Width = 466
            Height = 173
            Align = alBottom
            TabOrder = 0
            DesignSize = (
              466
              173)
            object ProjectTimeStepGroupBox: TGroupBox
              Left = 1
              Top = 1
              Width = 464
              Height = 54
              Align = alTop
              Caption = ' Time step duration  '
              TabOrder = 0
              ExplicitWidth = 171
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
              Width = 464
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
              ExplicitWidth = 171
            end
            object InitAfterLoadCheckBox: TCheckBox
              Left = 6
              Top = 151
              Width = 97
              Height = 17
              Anchors = [akLeft, akBottom]
              Caption = 'Init After Load'
              TabOrder = 2
              ExplicitTop = 320
            end
            object ResetAfterLoadCheckBox: TCheckBox
              Left = 98
              Top = 151
              Width = 97
              Height = 17
              Anchors = [akLeft, akBottom]
              Caption = 'Reset After Load'
              TabOrder = 3
              ExplicitTop = 320
            end
            object DebugModeCheckBox: TCheckBox
              Left = 206
              Top = 151
              Width = 50
              Height = 17
              Anchors = [akLeft, akBottom]
              Caption = 'Debug'
              TabOrder = 4
              ExplicitTop = 320
            end
            object GroupBox5: TGroupBox
              Left = 1
              Top = 96
              Width = 464
              Height = 54
              Align = alTop
              Caption = ' Min Intersteps Interval (ms)  '
              TabOrder = 5
              ExplicitWidth = 171
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
            Width = 466
            Height = 167
            Align = alClient
            TabOrder = 1
            ExplicitLeft = 264
            ExplicitTop = 0
            ExplicitWidth = 204
            ExplicitHeight = 342
            object ModelPageControl: TPageControl
              Left = 1
              Top = 1
              Width = 464
              Height = 165
              ActivePage = PredefinedModelTabSheet
              Align = alClient
              MultiLine = True
              TabOrder = 0
              TabPosition = tpRight
              object DontChangeTabSheet: TTabSheet
                Caption = 'Don'#39't Change'
                ImageIndex = 3
                ExplicitWidth = 416
              end
              object PredefinedModelTabSheet: TTabSheet
                Caption = 'Predefined Model'
                ExplicitWidth = 416
                DesignSize = (
                  396
                  157)
                object PredefinedModelComboBox: TComboBox
                  Left = 6
                  Top = 15
                  Width = 387
                  Height = 21
                  Anchors = [akLeft, akTop, akRight]
                  TabOrder = 0
                  Text = '0'
                end
              end
              object ModelFromFileTabSheet: TTabSheet
                Caption = 'From File'
                ImageIndex = 1
                ExplicitWidth = 416
                DesignSize = (
                  396
                  157)
                object ProjectModelFileNameLabeledEdit: TLabeledEdit
                  Left = 3
                  Top = 18
                  Width = 377
                  Height = 21
                  Anchors = [akLeft, akTop, akRight]
                  EditLabel.Width = 74
                  EditLabel.Height = 13
                  EditLabel.Caption = 'Model file name'
                  TabOrder = 0
                end
                object OpenModelButton: TButton
                  Left = 321
                  Top = 16
                  Width = 75
                  Height = 25
                  Anchors = [akTop, akRight]
                  Caption = 'Browse'
                  TabOrder = 1
                  OnClick = OpenModelButtonClick
                end
              end
              object ModelFromComponentTabSheet: TTabSheet
                Caption = 'From Component'
                ImageIndex = 2
                ExplicitWidth = 416
                object GroupBox2: TGroupBox
                  Left = 0
                  Top = 0
                  Width = 396
                  Height = 157
                  Align = alClient
                  Caption = ' Root Model Component Name '
                  TabOrder = 0
                  ExplicitWidth = 416
                  inline UClassesListFrame1: TUClassesListFrame
                    Left = 2
                    Top = 15
                    Width = 392
                    Height = 140
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
                    ExplicitTop = 32
                    ExplicitWidth = 412
                    ExplicitHeight = 123
                    inherited PageControl: TPageControl
                      Width = 392
                      Height = 140
                      ActivePage = UClassesListFrame1.NameTabSheet
                      ExplicitWidth = 412
                      ExplicitHeight = 123
                      inherited NameTabSheet: TTabSheet
                        ExplicitLeft = 4
                        ExplicitTop = 24
                        ExplicitWidth = 404
                        ExplicitHeight = 95
                        inherited StringGrid: TStringGrid
                          Width = 384
                          Height = 112
                          ExplicitWidth = 404
                          ExplicitHeight = 95
                        end
                      end
                      inherited LibsTabSheet: TTabSheet
                        ExplicitLeft = 4
                        ExplicitTop = 24
                        ExplicitWidth = 499
                        ExplicitHeight = 515
                      end
                      inherited LibsControlTabSheet: TTabSheet
                        ExplicitLeft = 4
                        ExplicitTop = 24
                        ExplicitWidth = 182
                        ExplicitHeight = 250
                        inherited Splitter1: TSplitter
                          Top = -40
                          Width = 182
                          ExplicitTop = -203
                          ExplicitWidth = 507
                        end
                        inherited Panel1: TPanel
                          Top = 127
                          Width = 182
                          ExplicitTop = 127
                          ExplicitWidth = 182
                          inherited LoadLibraryButton: TButton
                            Width = 180
                            ExplicitWidth = 180
                          end
                          inherited CreateRuntimeLibraryButton: TButton
                            Width = 180
                            ExplicitWidth = 180
                          end
                          inherited DeleteLibraryButton: TButton
                            Width = 180
                            ExplicitWidth = 180
                          end
                          inherited RenameRuntimeLibraryButton: TButton
                            Width = 180
                            ExplicitWidth = 180
                          end
                          inherited AddClassButton: TButton
                            Width = 180
                            ExplicitLeft = 1
                            ExplicitTop = 101
                            ExplicitWidth = 180
                          end
                        end
                        inherited GroupBox1: TGroupBox
                          Width = 182
                          ExplicitWidth = 182
                          ExplicitHeight = 225
                          inherited LibsListStringGrid: TStringGrid
                            Width = 178
                            ExplicitWidth = 178
                            ExplicitHeight = 208
                          end
                        end
                        inherited GroupBox2: TGroupBox
                          Top = -33
                          Width = 182
                          ExplicitTop = -33
                          ExplicitWidth = 182
                          inherited LibComponentListStringGrid: TStringGrid
                            Width = 178
                            ExplicitWidth = 178
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
        Top = 406
        Width = 126
        Height = 17
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
