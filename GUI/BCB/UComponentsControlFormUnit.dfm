object UComponentsControlForm: TUComponentsControlForm
  Left = 0
  Top = 0
  Caption = 'UComponentsControlForm'
  ClientHeight = 436
  ClientWidth = 1009
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
  inline ComponentsControlFrame: TUComponentsControlFrame
    Left = 0
    Top = 0
    Width = 1009
    Height = 436
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 1009
    ExplicitHeight = 436
    inherited Panel1: TPanel
      Top = 398
      Width = 1009
      ExplicitTop = 398
      ExplicitWidth = 1009
      inherited HeaderControl: THeaderControl
        Width = 1007
        ExplicitWidth = 1007
      end
    end
    inherited Panel2: TPanel
      Width = 1009
      Height = 398
      ExplicitWidth = 1009
      ExplicitHeight = 398
      inherited Splitter1: TSplitter
        Height = 396
        ExplicitLeft = 305
        ExplicitTop = 1
        ExplicitHeight = 397
      end
      inherited GroupBox1: TGroupBox
        Width = 632
        Height = 396
        ExplicitWidth = 632
        ExplicitHeight = 397
        inherited ComponentsListFrame: TUComponentsListFrame
          Top = 19
          Width = 628
          Height = 375
          ExplicitWidth = 628
          ExplicitHeight = 380
          inherited Splitter1: TSplitter
            Height = 375
            ExplicitHeight = 380
          end
          inherited Panel1: TPanel
            Width = 259
            Height = 375
            ExplicitWidth = 260
            ExplicitHeight = 380
            inherited PageControl1: TPageControl
              Width = 257
              Height = 373
              ExplicitWidth = 258
              ExplicitHeight = 378
              inherited TabSheet6: TTabSheet
                ExplicitWidth = 249
                ExplicitHeight = 351
                inherited Splitter5: TSplitter
                  Top = 271
                  Width = 249
                  Height = 9
                  ExplicitTop = 278
                  ExplicitWidth = 252
                  ExplicitHeight = 9
                end
                inherited EnchancedSG1: TEnchancedSG
                  Width = 249
                  Height = 271
                  ExplicitWidth = 252
                  ExplicitHeight = 278
                end
                inherited HeaderControl2: THeaderControl
                  Top = 313
                  Width = 249
                  ExplicitTop = 320
                  ExplicitWidth = 252
                end
                inherited NiceParamValRichEdit: TRichEdit
                  Top = 280
                  Width = 249
                  ExplicitTop = 287
                  ExplicitWidth = 252
                end
              end
              inherited TabSheet7: TTabSheet
                inherited Splitter6: TSplitter
                  ExplicitTop = 354
                  ExplicitWidth = 301
                end
                inherited HeaderControl4: THeaderControl
                  ExplicitTop = 397
                  ExplicitWidth = 301
                end
                inherited NiceStateValRichEdit: TRichEdit
                  ExplicitTop = 364
                  ExplicitWidth = 301
                end
                inherited EnchancedSG2: TEnchancedSG
                  ExplicitWidth = 301
                  ExplicitHeight = 354
                end
              end
              inherited TabSheet3: TTabSheet
                inherited Splitter2: TSplitter
                  ExplicitTop = 239
                end
              end
              inherited TabSheet1: TTabSheet
                inherited ParametersHeaderControl: THeaderControl
                  ExplicitTop = 397
                  ExplicitWidth = 301
                end
                inherited ParametersRichEdit: TRichEdit
                  ExplicitWidth = 301
                  ExplicitHeight = 397
                end
              end
              inherited TabSheet2: TTabSheet
                inherited StateHeaderControl: THeaderControl
                  ExplicitTop = 397
                  ExplicitWidth = 301
                end
                inherited StateRichEdit: TRichEdit
                  ExplicitWidth = 301
                  ExplicitHeight = 397
                end
              end
            end
          end
          inherited Panel2: TPanel
            Height = 375
            ExplicitHeight = 380
            inherited StringGrid: TStringGrid
              Height = 321
              OnClick = ComponentsListFrameStringGridClick
              OnDblClick = ComponentsListFrameStringGridDblClick
              OnKeyDown = ComponentsListFrameStringGridKeyDown
              OnSelectCell = ComponentsListFrameStringGridSelectCell
              ExplicitHeight = 321
            end
            inherited ClassNamePanel: TPanel
              Top = 356
              ExplicitTop = 356
            end
          end
        end
      end
      inherited GroupBox2: TGroupBox
        Height = 396
        ExplicitHeight = 397
        inherited ClassesListFrame: TUClassesListFrame
          Top = 19
          Height = 375
          ExplicitHeight = 380
          inherited PageControl: TPageControl
            Height = 354
            ExplicitTop = 17
            ExplicitWidth = 301
            ExplicitHeight = 363
            inherited NameTabSheet: TTabSheet
              inherited StringGrid: TStringGrid
                Height = 379
                OnDblClick = ClassesListFrameStringGridDblClick
                ExplicitHeight = 379
              end
            end
            inherited LibsTabSheet: TTabSheet
              inherited TreeView: TTreeView
                ExplicitWidth = 399
                ExplicitHeight = 395
              end
            end
            inherited LibsControlTabSheet: TTabSheet
              ExplicitLeft = 0
              ExplicitTop = 0
              ExplicitHeight = 351
              inherited Splitter1: TSplitter
                Top = 61
                ExplicitLeft = 0
                ExplicitTop = 61
              end
              inherited GroupBox1: TGroupBox
                Height = 61
                ExplicitHeight = 61
                inherited LibsListStringGrid: TStringGrid
                  Height = 44
                  ExplicitLeft = 2
                  ExplicitTop = 15
                  ExplicitHeight = 44
                end
              end
              inherited GroupBox2: TGroupBox
                Top = 68
                Height = 160
                ExplicitTop = 68
                inherited LibComponentListStringGrid: TStringGrid
                  Height = 143
                  ExplicitLeft = 2
                  ExplicitHeight = 143
                end
              end
            end
          end
          inherited SearchEdit: TEdit
            Margins.Left = 2
            Margins.Top = 2
            Margins.Right = 2
            Margins.Bottom = 2
            ExplicitWidth = 301
          end
        end
      end
      inherited Panel3: TPanel
        Height = 396
        ExplicitHeight = 397
        inherited TakeObjectButton: TButton
          OnClick = ComponentsControlFrameTakeObjectButtonClick
        end
        inherited ReturnObjectButton: TButton
          OnClick = ComponentsControlFrameReturnObjectButtonClick
        end
      end
    end
  end
end
