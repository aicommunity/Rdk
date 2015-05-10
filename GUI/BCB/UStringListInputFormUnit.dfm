object UStringListInputForm: TUStringListInputForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Input Parameters Values'
  ClientHeight = 331
  ClientWidth = 702
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -14
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 17
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 702
    Height = 277
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alClient
    TabOrder = 0
    object GroupBox1: TGroupBox
      Left = 1
      Top = 1
      Width = 700
      Height = 275
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Align = alClient
      Caption = ' Parameters list '
      TabOrder = 0
      object StringGrid: TStringGrid
        Left = 2
        Top = 19
        Width = 696
        Height = 229
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alClient
        ColCount = 2
        DefaultColWidth = 250
        FixedCols = 0
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
        TabOrder = 0
        OnClick = StringGridClick
        OnDblClick = StringGridDblClick
        OnSelectCell = StringGridSelectCell
        ExplicitHeight = 233
      end
      object Edit: TEdit
        Left = 2
        Top = 248
        Width = 696
        Height = 25
        Margins.Left = 4
        Margins.Top = 4
        Margins.Right = 4
        Margins.Bottom = 4
        Align = alBottom
        TabOrder = 1
        OnKeyPress = EditKeyPress
        ExplicitTop = 252
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 277
    Width = 702
    Height = 54
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alBottom
    TabOrder = 1
    DesignSize = (
      702
      54)
    object OkButton: TButton
      Left = 381
      Top = 9
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akTop, akRight]
      Caption = 'OK'
      TabOrder = 0
      OnClick = OkButtonClick
    end
    object CancelButton: TButton
      Left = 486
      Top = 9
      Width = 99
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      TabOrder = 1
      OnClick = CancelButtonClick
    end
    object RestoreButton: TButton
      Left = 592
      Top = 9
      Width = 98
      Height = 33
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Anchors = [akTop, akRight]
      Caption = 'Restore'
      TabOrder = 2
      OnClick = RestoreButtonClick
    end
  end
end
