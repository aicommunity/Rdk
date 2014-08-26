object UCreateProjectWizardForm: TUCreateProjectWizardForm
  Left = 0
  Top = 0
  Caption = 'Create Project Wizard'
  ClientHeight = 403
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
    Top = 362
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
    Height = 362
    ActivePage = TabSheet3
    Align = alClient
    TabOrder = 1
    OnChange = PageControlChange
    object TabSheet1: TTabSheet
      Caption = 'Name'
      DesignSize = (
        519
        334)
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
        Height = 155
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
        Top = 316
        Width = 70
        Height = 17
        Anchors = [akLeft, akBottom]
        Caption = 'Autosave'
        Checked = True
        State = cbChecked
        TabOrder = 5
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
        Top = 41
        Width = 519
        Height = 40
        Align = alTop
        Caption = 'Project type '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'Universal'
          'Video analysis')
        TabOrder = 2
        OnClick = ProjectTypeRadioGroupClick
      end
      object ProjectTimeStepGroupBox: TGroupBox
        Left = 0
        Top = 161
        Width = 519
        Height = 54
        Align = alTop
        Caption = ' Time step duration (or default frame processing duration) '
        TabOrder = 1
        object Label2: TLabel
          Left = 10
          Top = 23
          Width = 28
          Height = 13
          Caption = 'T=1./'
        end
        object ProjectTimeStepEdit: TEdit
          Left = 44
          Top = 20
          Width = 45
          Height = 21
          TabOrder = 0
          Text = '30'
        end
      end
      object ProjectCalculationModeRadioGroup: TRadioGroup
        Left = 0
        Top = 121
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
        TabOrder = 3
      end
      object VideoAnalysisGroupBox: TGroupBox
        Left = 0
        Top = 215
        Width = 519
        Height = 119
        Align = alClient
        Caption = ' Video Analysis Additional Parameters '
        TabOrder = 4
        Visible = False
        DesignSize = (
          519
          119)
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
        Top = 81
        Width = 519
        Height = 40
        Align = alTop
        Caption = ' Calculation time source '
        Columns = 2
        ItemIndex = 1
        Items.Strings = (
          'System Time'
          'External Data Time')
        TabOrder = 5
        OnClick = ProjectTypeRadioGroupClick
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Model'
      ImageIndex = 2
      object TitlePanel3: TPanel
        Left = 0
        Top = 0
        Width = 519
        Height = 41
        Align = alTop
        Alignment = taLeftJustify
        Caption = 'Select predefined model or root component name'
        TabOrder = 0
      end
      object GroupBox2: TGroupBox
        Left = 0
        Top = 185
        Width = 519
        Height = 149
        Align = alClient
        TabOrder = 1
        inline UClassesListFrame1: TUClassesListFrame
          Left = 2
          Top = 32
          Width = 515
          Height = 115
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
          ExplicitWidth = 515
          ExplicitHeight = 115
          inherited PageControl: TPageControl
            Width = 515
            Height = 115
            ActivePage = UClassesListFrame1.NameTabSheet
            ExplicitWidth = 515
            ExplicitHeight = 115
            inherited NameTabSheet: TTabSheet
              ExplicitLeft = 4
              ExplicitTop = 24
              ExplicitWidth = 499
              ExplicitHeight = 515
              inherited StringGrid: TStringGrid
                Width = 507
                Height = 87
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
              ExplicitWidth = 507
              ExplicitHeight = 87
              inherited Splitter1: TSplitter
                Top = -203
                Width = 507
              end
              inherited Panel1: TPanel
                Top = -36
                Width = 507
                ExplicitTop = -36
                ExplicitWidth = 507
                inherited LoadLibraryButton: TButton
                  Width = 505
                  ExplicitWidth = 505
                end
                inherited CreateRuntimeLibraryButton: TButton
                  Width = 505
                  ExplicitWidth = 505
                end
                inherited DeleteLibraryButton: TButton
                  Width = 505
                  ExplicitWidth = 505
                end
                inherited RenameRuntimeLibraryButton: TButton
                  Width = 505
                  ExplicitWidth = 505
                end
                inherited AddClassButton: TButton
                  Width = 505
                  ExplicitLeft = 1
                  ExplicitTop = 101
                  ExplicitWidth = 505
                end
              end
              inherited GroupBox1: TGroupBox
                Width = 507
                ExplicitWidth = 507
                ExplicitHeight = 225
                inherited LibsListStringGrid: TStringGrid
                  Width = 503
                  ExplicitWidth = 503
                  ExplicitHeight = 208
                end
              end
              inherited GroupBox2: TGroupBox
                Top = -196
                Width = 507
                ExplicitTop = -196
                ExplicitWidth = 507
                inherited LibComponentListStringGrid: TStringGrid
                  Width = 503
                  ExplicitWidth = 503
                  ExplicitHeight = 143
                end
              end
            end
          end
        end
        object RootModelComponentNameRadioButton: TRadioButton
          Left = 2
          Top = 15
          Width = 515
          Height = 17
          Align = alTop
          Caption = 'Root model component name'
          TabOrder = 1
          OnClick = RootModelComponentNameRadioButtonClick
        end
      end
      object GroupBox3: TGroupBox
        Left = 0
        Top = 41
        Width = 519
        Height = 72
        Align = alTop
        TabOrder = 2
        DesignSize = (
          519
          72)
        object PredefinedModelRadioButton: TRadioButton
          Left = 2
          Top = 15
          Width = 515
          Height = 17
          Align = alTop
          Caption = 'Predefined model'
          Checked = True
          TabOrder = 1
          TabStop = True
          OnClick = PredefinedModelRadioButtonClick
        end
        object PredefinedModelComboBox: TComboBox
          Left = 4
          Top = 38
          Width = 508
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 0
          Text = '0'
        end
      end
      object GroupBox1: TGroupBox
        Left = 0
        Top = 113
        Width = 519
        Height = 72
        Align = alTop
        TabOrder = 3
        DesignSize = (
          519
          72)
        object ModelFileNameRadioButton: TRadioButton
          Left = 2
          Top = 15
          Width = 515
          Height = 17
          Align = alTop
          Caption = 'Load model from file'
          TabOrder = 0
          OnClick = ModelFileNameRadioButtonClick
        end
        object ProjectModelFileNameLabeledEdit: TLabeledEdit
          Left = 3
          Top = 51
          Width = 432
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          EditLabel.Width = 74
          EditLabel.Height = 13
          EditLabel.Caption = 'Model file name'
          TabOrder = 1
        end
        object OpenModelButton: TButton
          Left = 441
          Top = 47
          Width = 75
          Height = 25
          Caption = 'Browse'
          TabOrder = 2
          OnClick = OpenModelButtonClick
        end
      end
    end
  end
  object OpenTextFileDialog: TOpenTextFileDialog
    DefaultExt = 'xml'
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Select model file name'
    Left = 176
    Top = 72
  end
end
