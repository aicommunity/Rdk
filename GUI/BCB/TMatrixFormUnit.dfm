object MatrixForm: TMatrixForm
  Left = 0
  Top = 0
  Caption = 'Matrix Watch'
  ClientHeight = 307
  ClientWidth = 505
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object ComponentNamePanel: TPanel
    Left = 0
    Top = 0
    Width = 505
    Height = 25
    Align = alTop
    Alignment = taLeftJustify
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 0
  end
  object Panel2: TPanel
    Left = 0
    Top = 25
    Width = 505
    Height = 231
    Align = alClient
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 1
    object StringGrid: TStringGrid
      Left = 1
      Top = 1
      Width = 503
      Height = 229
      Align = alClient
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRangeSelect, goRowSizing, goColSizing]
      TabOrder = 0
      OnSelectCell = StringGridSelectCell
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 256
    Width = 505
    Height = 51
    Align = alBottom
    Alignment = taLeftJustify
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 2
    DesignSize = (
      505
      51)
    object OkButton: TButton
      Left = 343
      Top = 14
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      Default = True
      ModalResult = 1
      TabOrder = 0
    end
    object CancelButton: TButton
      Left = 424
      Top = 14
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
  end
end
