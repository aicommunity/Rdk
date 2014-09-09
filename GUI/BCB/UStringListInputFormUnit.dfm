object UStringListInputForm: TUStringListInputForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Input Parameters Values'
  ClientHeight = 253
  ClientWidth = 537
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 537
    Height = 212
    Align = alClient
    TabOrder = 0
    object GroupBox1: TGroupBox
      Left = 1
      Top = 1
      Width = 535
      Height = 210
      Align = alClient
      Caption = ' Parameters list '
      TabOrder = 0
      object StringGrid: TStringGrid
        Left = 2
        Top = 15
        Width = 531
        Height = 172
        Align = alClient
        ColCount = 2
        DefaultColWidth = 250
        FixedCols = 0
        Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goHorzLine, goRowSelect]
        TabOrder = 0
        OnClick = StringGridClick
        OnDblClick = StringGridDblClick
        OnSelectCell = StringGridSelectCell
      end
      object Edit: TEdit
        Left = 2
        Top = 187
        Width = 531
        Height = 21
        Align = alBottom
        TabOrder = 1
        OnKeyPress = EditKeyPress
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 212
    Width = 537
    Height = 41
    Align = alBottom
    TabOrder = 1
    DesignSize = (
      537
      41)
    object OkButton: TButton
      Left = 291
      Top = 7
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      TabOrder = 0
      OnClick = OkButtonClick
    end
    object CancelButton: TButton
      Left = 372
      Top = 7
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      TabOrder = 1
      OnClick = CancelButtonClick
    end
    object RestoreButton: TButton
      Left = 453
      Top = 7
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Restore'
      TabOrder = 2
      OnClick = RestoreButtonClick
    end
  end
end
