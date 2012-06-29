object UClassesListForm: TUClassesListForm
  Left = 0
  Top = 321
  Caption = 'UClassesListForm'
  ClientHeight = 243
  ClientWidth = 527
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
  inline ClassesListFrame: TUClassesListFrame
    Left = 0
    Top = 0
    Width = 527
    Height = 243
    Align = alClient
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 0
    ExplicitLeft = 20
    ExplicitTop = -204
    ExplicitWidth = 527
    ExplicitHeight = 243
    inherited StringGrid: TStringGrid
      Width = 527
      Height = 243
      ExplicitWidth = 527
      ExplicitHeight = 243
    end
  end
end
