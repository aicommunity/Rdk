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
    Height = 202
    Align = alClient
    DoubleBuffered = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 527
    ExplicitHeight = 243
    inherited StringGrid: TStringGrid
      Width = 527
      Height = 202
      ExplicitWidth = 527
      ExplicitHeight = 243
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 202
    Width = 527
    Height = 41
    Align = alBottom
    TabOrder = 1
    ExplicitLeft = -38
    ExplicitWidth = 565
    DesignSize = (
      527
      41)
    object OkButton: TButton
      Left = 359
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      Default = True
      ModalResult = 1
      TabOrder = 0
      ExplicitLeft = 397
    end
    object CancelButton: TButton
      Left = 442
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
      ExplicitLeft = 480
    end
  end
end
