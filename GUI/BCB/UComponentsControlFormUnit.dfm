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
    TabOrder = 0
    ExplicitLeft = 22
    ExplicitTop = -90
    inherited Panel1: TPanel
      Top = 398
      Width = 1009
      inherited HeaderControl: THeaderControl
        Width = 1007
      end
    end
    inherited Panel2: TPanel
      Width = 1009
      Height = 398
      inherited Splitter1: TSplitter
        Height = 396
      end
      inherited GroupBox1: TGroupBox
        Width = 632
        Height = 396
        inherited ComponentsListFrame: TUComponentsListFrame
          Width = 628
          Height = 379
          inherited Splitter1: TSplitter
            Height = 379
          end
          inherited Panel1: TPanel
            Width = 259
            Height = 379
            inherited ParametersRichEdit: TRichEdit
              Width = 257
              Height = 343
            end
            inherited ParametersHeaderControl: THeaderControl
              Width = 257
            end
          end
          inherited Panel2: TPanel
            Height = 379
            inherited StringGrid: TStringGrid
              Height = 343
              OnClick = ComponentsListFrameStringGridClick
              OnDblClick = ComponentsListFrameStringGridDblClick
              OnKeyPress = ComponentsListFrameStringGridKeyPress
              OnSelectCell = ComponentsListFrameStringGridSelectCell
            end
          end
        end
      end
      inherited GroupBox2: TGroupBox
        Height = 396
        inherited ClassesListFrame: TUClassesListFrame
          Height = 379
          inherited StringGrid: TStringGrid
            Height = 379
            OnDblClick = ClassesListFrameStringGridDblClick
            OnKeyPress = ClassesListFrameStringGridKeyPress
          end
        end
      end
      inherited Panel3: TPanel
        Height = 396
      end
    end
  end
end
