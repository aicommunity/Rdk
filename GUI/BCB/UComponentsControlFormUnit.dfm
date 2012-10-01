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
              inherited TabSheet1: TTabSheet
                ExplicitWidth = 249
                ExplicitHeight = 351
                inherited ParametersHeaderControl: THeaderControl
                  Width = 249
                  ExplicitWidth = 249
                end
                inherited ParametersRichEdit: TRichEdit
                  Width = 249
                  Height = 317
                  ExplicitWidth = 249
                  ExplicitHeight = 317
                end
              end
            end
          end
          inherited Panel2: TPanel
            Height = 379
            ExplicitHeight = 379
            inherited StringGrid: TStringGrid
              Height = 343
              OnClick = ComponentsListFrameStringGridClick
              OnDblClick = ComponentsListFrameStringGridDblClick
              OnKeyPress = ComponentsListFrameStringGridKeyPress
              OnSelectCell = ComponentsListFrameStringGridSelectCell
              ExplicitHeight = 343
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
          inherited StringGrid: TStringGrid
            Height = 379
            OnDblClick = ClassesListFrameStringGridDblClick
            OnKeyPress = ClassesListFrameStringGridKeyPress
            ExplicitHeight = 379
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
