object UComponentLinksForm: TUComponentLinksForm
  Left = 0
  Top = 0
  Caption = 'UComponentLinksForm'
  ClientHeight = 535
  ClientWidth = 820
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  inline UComponentLinksFrame: TUComponentLinksFrame
    Left = 0
    Top = 0
    Width = 820
    Height = 535
    Align = alClient
    TabOrder = 0
    ExplicitWidth = 820
    ExplicitHeight = 535
    inherited Splitter1: TSplitter
      Top = 323
      Width = 820
      ExplicitTop = 325
      ExplicitWidth = 820
    end
    inherited GroupBox1: TGroupBox
      Left = 515
      Height = 323
      ExplicitLeft = 515
      ExplicitHeight = 323
      inherited NANetFrameInputs: TUComponentIOFrame
        Height = 306
        ExplicitHeight = 306
        inherited StringGrid: TStringGrid
          Height = 306
          ExplicitHeight = 306
        end
      end
    end
    inherited GroupBox2: TGroupBox
      Width = 515
      Height = 323
      ExplicitWidth = 515
      ExplicitHeight = 323
      inherited NANetFrameOutputs: TUComponentIOFrame
        Width = 511
        Height = 306
        ExplicitWidth = 511
        ExplicitHeight = 306
        inherited StringGrid: TStringGrid
          Width = 511
          Height = 306
          ExplicitWidth = 511
          ExplicitHeight = 306
        end
      end
    end
    inherited GroupBox3: TGroupBox
      Top = 330
      Width = 820
      ExplicitTop = 330
      ExplicitWidth = 820
      inherited NANetFrameLinks: TUComponentIOFrame
        Width = 816
        ExplicitWidth = 816
        inherited StringGrid: TStringGrid
          Width = 816
          ExplicitWidth = 816
        end
      end
    end
    inherited Panel1: TPanel
      Top = 498
      Width = 820
      ExplicitTop = 498
      ExplicitWidth = 820
      inherited HeaderControl1: THeaderControl
        Width = 818
        ExplicitWidth = 818
      end
    end
  end
end
