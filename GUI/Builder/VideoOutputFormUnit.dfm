object VideoOutputForm: TVideoOutputForm
  Left = 0
  Top = 250
  Caption = #1048#1089#1090#1086#1095#1085#1080#1082
  ClientHeight = 531
  ClientWidth = 645
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
  inline VideoOutputFrame: TVideoOutputFrame
    Left = 0
    Top = 0
    Width = 645
    Height = 531
    Align = alClient
    Constraints.MinHeight = 100
    Constraints.MinWidth = 200
    PopupMenu = VideoOutputFrame.PopupMenu
    TabOrder = 0
    TabStop = True
    ExplicitWidth = 645
    ExplicitHeight = 531
    inherited GroupBox: TGroupBox
      Width = 645
      Height = 490
      ExplicitWidth = 645
      ExplicitHeight = 490
      inherited Image: TImage
        Width = 641
        Height = 473
        OnMouseUp = VideoOutputFrame1ImageMouseUp
        ExplicitWidth = 422
        ExplicitHeight = 165
      end
    end
    inherited Panel1: TPanel
      Top = 490
      Width = 645
      ExplicitTop = 490
      ExplicitWidth = 645
      inherited StopButton: TButton
        Left = 608
        ExplicitLeft = 608
      end
      inherited StartButton: TButton
        Left = 572
        ExplicitLeft = 572
      end
      inherited TimeEdit: TMaskEdit
        Left = 487
        ExplicitLeft = 487
      end
      inherited TrackBar: TTrackBar
        Width = 479
        ExplicitWidth = 479
      end
    end
    inherited PopupMenu: TPopupMenu
      inherited N1: TMenuItem
        OnClick = VideoOutputFrameN1Click
      end
    end
  end
end
