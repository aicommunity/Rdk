object UCRConvolutionNetworkForm: TUCRConvolutionNetworkForm
  Left = 0
  Top = 0
  Caption = 'UCRConvolutionNetworkForm'
  ClientHeight = 356
  ClientWidth = 527
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 315
    Width = 527
    Height = 41
    Align = alBottom
    TabOrder = 0
    object Button1: TButton
      Left = 5
      Top = 7
      Width = 75
      Height = 25
      Caption = 'Update'
      TabOrder = 0
      OnClick = Button1Click
    end
  end
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 527
    Height = 315
    ActivePage = TabSheet2
    Align = alClient
    TabOrder = 1
    object TabSheet2: TTabSheet
      Caption = 'Outputs'
      ImageIndex = 1
      object OutputsStringGrid: TStringGrid
        Left = 0
        Top = 0
        Width = 519
        Height = 287
        Align = alClient
        DefaultColWidth = 48
        DefaultRowHeight = 18
        TabOrder = 0
      end
    end
  end
end
