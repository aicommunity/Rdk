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
    ExplicitLeft = -490
    ExplicitTop = -112
    inherited Panel1: TPanel
      Width = 1334
      Height = 504
      inherited ClassesListFrame: TUClassesListFrame
        Height = 502
        inherited PageControl: TPageControl
          Height = 502
          ExplicitWidth = 272
          inherited LibsTabSheet: TTabSheet
            ExplicitWidth = 264
            ExplicitHeight = 586
            inherited TreeView: TTreeView
              Height = 474
              ExplicitWidth = 264
              ExplicitHeight = 586
            end
          end
          inherited LibsControlTabSheet: TTabSheet
            inherited Splitter1: TSplitter
              ExplicitTop = 296
            end
            inherited Panel1: TPanel
              ExplicitTop = 463
            end
          end
        end
      end
      inherited Panel3: TPanel
        Width = 486
        Height = 502
        ExplicitLeft = 273
        ExplicitWidth = 583
        inherited ClassNameLabeledEdit: TLabeledEdit
          Width = 474
          EditLabel.ExplicitLeft = 0
          EditLabel.ExplicitTop = -16
          EditLabel.ExplicitWidth = 107
          ExplicitWidth = 571
        end
        inherited ClassFullNameLabeledEdit: TLabeledEdit
          Width = 474
          EditLabel.ExplicitLeft = 0
          EditLabel.ExplicitTop = -16
          EditLabel.ExplicitWidth = 123
          ExplicitWidth = 571
        end
        inherited ClassDescriptionGroupBox: TGroupBox
          Width = 474
          ExplicitWidth = 571
          inherited ClassDescriptionRichEdit: TRichEdit
            Width = 470
            ExplicitLeft = 2
            ExplicitTop = 15
            ExplicitWidth = 567
            ExplicitHeight = 117
          end
        end
        inherited GroupBox1: TGroupBox
          Width = 474
          ExplicitWidth = 571
          inherited StringGrid1: TStringGrid
            Width = 470
            ExplicitLeft = 2
            ExplicitTop = 15
            ExplicitWidth = 567
            ExplicitHeight = 353
          end
        end
      end
      inherited Panel4: TPanel
        Left = 759
        Height = 502
        inherited GroupBox2: TGroupBox
          Height = 500
          ExplicitLeft = 1
          ExplicitTop = 1
        end
      end
    end
    inherited Panel2: TPanel
      Top = 504
      Width = 1334
      ExplicitWidth = 1431
    end
  end
end
