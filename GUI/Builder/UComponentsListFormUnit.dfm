object UComponentsListForm: TUComponentsListForm
  Left = 857
  Top = 64
  Caption = 'Components List'
  ClientHeight = 379
  ClientWidth = 565
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  PixelsPerInch = 96
  TextHeight = 13
  inline ComponentsListFrame1: TUComponentsListFrame
    Left = 0
    Top = 0
    Width = 565
    Height = 379
    Align = alClient
    TabOrder = 0
    ExplicitWidth = 565
    ExplicitHeight = 379
    inherited Splitter1: TSplitter
      Height = 379
    end
    inherited Panel1: TPanel
      Width = 196
      Height = 379
      inherited ParametersRichEdit: TRichEdit
        Width = 194
        Height = 343
      end
      inherited ParametersHeaderControl: THeaderControl
        Width = 194
      end
    end
    inherited Panel2: TPanel
      Height = 379
      inherited StringGrid: TStringGrid
        Height = 343
        ExplicitHeight = 343
      end
    end
  end
end
