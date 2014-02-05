object UComponentsListForm: TUComponentsListForm
  Left = 857
  Top = 64
  Caption = 'Components List'
  ClientHeight = 420
  ClientWidth = 682
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 379
    Width = 682
    Height = 41
    Align = alBottom
    TabOrder = 0
    DesignSize = (
      682
      41)
    object OkButton: TButton
      Left = 514
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      Default = True
      ModalResult = 1
      TabOrder = 0
    end
    object CancelButton: TButton
      Left = 597
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
  end
  inline ComponentsListFrame1: TUComponentsListFrame
    Left = 0
    Top = 0
    Width = 682
    Height = 379
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    ExplicitLeft = -73
    ExplicitTop = -147
    inherited Splitter1: TSplitter
      Height = 379
    end
    inherited Panel1: TPanel
      Width = 313
      Height = 379
      inherited PageControl1: TPageControl
        Width = 311
        Height = 377
        inherited TabSheet7: TTabSheet
          inherited Splitter6: TSplitter
            Top = 274
            Width = 303
          end
          inherited HeaderControl4: THeaderControl
            Top = 317
            Width = 303
          end
          inherited NiceStateValRichEdit: TRichEdit
            Top = 284
            Width = 303
          end
          inherited EnchancedSG2: TEnchancedSG
            Width = 303
            Height = 274
          end
        end
      end
    end
    inherited Panel2: TPanel
      Height = 379
      inherited StringGrid: TStringGrid
        Height = 321
      end
      inherited ClassNamePanel: TPanel
        Top = 356
      end
    end
  end
end
