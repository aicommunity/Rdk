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
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  inline UComponentLinksFrame: TUComponentLinksFrame
    Left = 0
    Top = 0
    Width = 820
    Height = 535
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 820
    ExplicitHeight = 535
    inherited Splitter1: TSplitter
      Top = 282
      Width = 820
      ExplicitTop = 325
      ExplicitWidth = 820
    end
    inherited GroupBox1: TGroupBox
      Left = 515
      Height = 282
      ExplicitLeft = 515
      ExplicitHeight = 282
      inherited NANetFrameInputs: TUComponentIOFrame
        Height = 265
        ExplicitHeight = 265
        inherited StringGrid: TStringGrid
          Height = 265
          ExplicitHeight = 265
        end
      end
    end
    inherited GroupBox2: TGroupBox
      Width = 515
      Height = 282
      ExplicitWidth = 515
      ExplicitHeight = 282
      inherited NANetFrameOutputs: TUComponentIOFrame
        Width = 511
        Height = 265
        ExplicitWidth = 511
        ExplicitHeight = 265
        inherited StringGrid: TStringGrid
          Width = 511
          Height = 265
          ExplicitWidth = 511
          ExplicitHeight = 265
        end
      end
    end
    inherited GroupBox3: TGroupBox
      Top = 289
      Width = 820
      ExplicitTop = 289
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
      Top = 457
      Width = 820
      ExplicitTop = 457
      ExplicitWidth = 820
      inherited HeaderControl1: THeaderControl
        Width = 818
        ExplicitWidth = 818
      end
    end
    inherited ButtonPanel: TPanel
      Top = 494
      Width = 820
      ExplicitTop = 494
      ExplicitWidth = 820
      DesignSize = (
        820
        41)
      inherited OkButton: TButton
        Left = 649
        ExplicitLeft = 649
      end
      inherited CancelButton: TButton
        Left = 732
        ExplicitLeft = 732
      end
    end
  end
end
