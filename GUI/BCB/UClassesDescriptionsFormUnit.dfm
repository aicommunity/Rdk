object UClassesDescriptionsForm: TUClassesDescriptionsForm
  Left = 0
  Top = 0
  Caption = 'Classes Description'
  ClientHeight = 559
  ClientWidth = 1334
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 120
  TextHeight = 16
  inline ClassesDescriptionsFrame: TUClassesDescriptionsFrame
    Left = 0
    Top = 0
    Width = 1334
    Height = 559
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitLeft = -97
    ExplicitTop = -112
    inherited Panel1: TPanel
      Width = 1334
      Height = 504
      inherited ClassesListFrame: TUClassesListFrame
        Height = 502
        inherited PageControl: TPageControl
          Height = 502
          inherited LibsTabSheet: TTabSheet
            inherited TreeView: TTreeView
              Height = 474
            end
          end
        end
      end
      inherited Panel3: TPanel
        Width = 486
        Height = 502
        inherited ClassNameLabeledEdit: TLabeledEdit
          Width = 474
        end
        inherited ClassFullNameLabeledEdit: TLabeledEdit
          Width = 474
        end
        inherited ClassDescriptionGroupBox: TGroupBox
          Width = 474
          inherited ClassDescriptionRichEdit: TRichEdit
            Width = 470
          end
        end
        inherited GroupBox1: TGroupBox
          Width = 474
          inherited StringGrid1: TStringGrid
            Width = 470
          end
        end
      end
      inherited Panel4: TPanel
        Left = 759
        Height = 502
        inherited GroupBox2: TGroupBox
          Height = 262
          inherited PropertyDescriptionRichEdit: TRichEdit
            Height = 245
          end
        end
      end
    end
    inherited Panel2: TPanel
      Top = 504
      Width = 1334
      inherited ApplyButton: TButton
        Left = 1166
      end
      inherited RestoreButton: TButton
        Left = 1247
      end
    end
  end
end