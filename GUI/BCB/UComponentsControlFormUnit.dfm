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
        ExplicitHeight = 396
      end
      inherited GroupBox1: TGroupBox
        Width = 632
        Height = 396
        ExplicitWidth = 632
        ExplicitHeight = 396
        inherited ComponentsListFrame: TUComponentsListFrame
          Width = 628
          Height = 379
          ExplicitWidth = 628
          ExplicitHeight = 379
          inherited Splitter1: TSplitter
            Height = 379
            ExplicitHeight = 379
          end
          inherited Panel1: TPanel
            Width = 259
            Height = 379
            ExplicitWidth = 259
            ExplicitHeight = 379
            inherited PageControl1: TPageControl
              Width = 257
              Height = 377
              ExplicitWidth = 257
              ExplicitHeight = 377
              inherited TabSheet6: TTabSheet
                ExplicitWidth = 249
                ExplicitHeight = 351
                inherited Splitter5: TSplitter
                  Top = 274
                  Width = 249
                end
                inherited EnchancedSG1: TEnchancedSG
                  Width = 249
                  Height = 274
                  ExplicitWidth = 249
                  ExplicitHeight = 274
                end
                inherited HeaderControl2: THeaderControl
                  Top = 317
                  Width = 249
                  ExplicitTop = 317
                  ExplicitWidth = 249
                end
                inherited NiceParamValRichEdit: TRichEdit
                  Top = 284
                  Width = 249
                  ExplicitTop = 284
                  ExplicitWidth = 249
                end
              end
              inherited TabSheet1: TTabSheet
                inherited ParametersHeaderControl: THeaderControl
                  ExplicitWidth = 376
                end
              end
            end
          end
          inherited Panel2: TPanel
            Height = 379
            ExplicitHeight = 379
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
        ExplicitHeight = 396
        inherited ClassesListFrame: TUClassesListFrame
          Height = 379
          ExplicitHeight = 379
          inherited PageControl: TPageControl
            Height = 379
            ExplicitHeight = 379
            inherited NameTabSheet: TTabSheet
              inherited StringGrid: TStringGrid
                Height = 379
                OnDblClick = ClassesListFrameStringGridDblClick
                ExplicitHeight = 379
              end
            end
            inherited LibsControlTabSheet: TTabSheet
              ExplicitHeight = 351
              inherited Splitter1: TSplitter
                Top = 61
              end
              inherited Panel1: TPanel
                Top = 228
                ExplicitTop = 228
              end
              inherited GroupBox1: TGroupBox
                Height = 61
                ExplicitHeight = 61
                inherited LibsListStringGrid: TStringGrid
                  Height = 44
                  ExplicitHeight = 44
                end
              end
              inherited GroupBox2: TGroupBox
                Top = 68
                ExplicitTop = 68
              end
            end
          end
        end
      end
      inherited Panel3: TPanel
        Height = 396
        ExplicitHeight = 396
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
