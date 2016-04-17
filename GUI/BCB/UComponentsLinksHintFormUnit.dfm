object UComponentsLinksHintForm: TUComponentsLinksHintForm
  Left = 0
  Top = 0
  BorderStyle = bsSizeToolWin
  Caption = 'UComponentsLinksHintForm'
  ClientHeight = 451
  ClientWidth = 424
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 0
    Top = 225
    Width = 424
    Height = 7
    Cursor = crVSplit
    Align = alBottom
    ExplicitLeft = 1
    ExplicitTop = 72
    ExplicitWidth = 757
  end
  object GroupBox1: TGroupBox
    Left = 0
    Top = 0
    Width = 424
    Height = 225
    Align = alClient
    Caption = 'Input links'
    TabOrder = 0
    inline InputsFrame: TUComponentIOFrame
      Left = 2
      Top = 15
      Width = 420
      Height = 208
      Align = alClient
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 420
      ExplicitHeight = 208
      inherited StringGrid: TStringGrid
        Width = 420
        Height = 208
        DefaultRowHeight = 20
        ExplicitWidth = 420
        ExplicitHeight = 208
      end
    end
  end
  object GroupBox2: TGroupBox
    Left = 0
    Top = 232
    Width = 424
    Height = 219
    Align = alBottom
    Caption = 'Output links'
    TabOrder = 1
    inline OutputsFrame: TUComponentIOFrame
      Left = 2
      Top = 15
      Width = 420
      Height = 202
      Align = alClient
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      ExplicitLeft = 2
      ExplicitTop = 15
      ExplicitWidth = 420
      ExplicitHeight = 202
      inherited StringGrid: TStringGrid
        Width = 420
        Height = 202
        DefaultRowHeight = 20
        ExplicitWidth = 420
        ExplicitHeight = 202
      end
    end
  end
end
