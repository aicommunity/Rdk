object UComponentsListForm: TUComponentsListForm
  Left = 857
  Top = 64
  Caption = 'Components List'
  ClientHeight = 420
  ClientWidth = 565
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
      ExplicitHeight = 379
    end
    inherited Panel1: TPanel
      Width = 196
      Height = 379
      ExplicitWidth = 196
      ExplicitHeight = 379
      inherited ParametersRichEdit: TRichEdit
        Width = 194
        Height = 343
        ExplicitWidth = 194
        ExplicitHeight = 343
      end
      inherited ParametersHeaderControl: THeaderControl
        Width = 194
        ExplicitWidth = 194
      end
    end
    inherited Panel2: TPanel
      Height = 379
      ExplicitHeight = 379
      inherited StringGrid: TStringGrid
        Height = 343
        ExplicitHeight = 343
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 379
    Width = 565
    Height = 41
    Align = alBottom
    TabOrder = 1
    ExplicitLeft = -1
    ExplicitTop = 343
    DesignSize = (
      565
      41)
    object OkButton: TButton
      Left = 397
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
      Left = 480
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
  end
end
