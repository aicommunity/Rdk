object DebugControlForm: TDebugControlForm
  Left = 0
  Top = 0
  Caption = 'DebugControlForm'
  ClientHeight = 362
  ClientWidth = 577
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  inline DebugControlFrame1: TDebugControlFrame
    Left = 0
    Top = 0
    Width = 577
    Height = 362
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitLeft = 137
    ExplicitTop = -12
    inherited DebugImagesGroupBox: TGroupBox
      Top = 186
      Width = 577
      DesignSize = (
        577
        176)
      inherited ImgTLLabel: TLabel
        Anchors = [akTop]
      end
      inherited ImgTRLabel: TLabel
        Anchors = [akTop]
      end
      inherited ImgRBLabel: TLabel
        Anchors = [akTop]
      end
      inherited ImgLBLabel: TLabel
        Anchors = [akTop]
      end
      inherited ImgTLEdit: TEdit
        Anchors = [akTop]
      end
      inherited ImgTLButton: TButton
        Anchors = [akTop]
      end
      inherited ImgTREdit: TEdit
        Anchors = [akTop]
      end
      inherited ImgTRButton: TButton
        Anchors = [akTop]
      end
      inherited ImgRBEdit: TEdit
        Anchors = [akTop]
      end
      inherited ImgRBButton: TButton
        Anchors = [akTop]
      end
      inherited ImgLBButton: TButton
        Anchors = [akTop]
      end
      inherited ImgLBEdit: TEdit
        Anchors = [akTop]
      end
    end
  end
end
