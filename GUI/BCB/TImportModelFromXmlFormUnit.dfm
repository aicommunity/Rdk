object ImportModelFromXmlForm: TImportModelFromXmlForm
  Left = 0
  Top = 0
  Caption = 'ImportModelFromXmlForm'
  ClientHeight = 300
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  inline ImportModelFromXmlFrame: TImportModelFromXmlFrame
    Left = 0
    Top = 0
    Width = 635
    Height = 300
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitLeft = -4
    ExplicitTop = -93
    ExplicitWidth = 635
    ExplicitHeight = 300
    inherited Panel1: TPanel
      Width = 635
      Height = 251
      ExplicitWidth = 635
      ExplicitHeight = 251
      inherited PageControl1: TPageControl
        Width = 633
        Height = 249
        ExplicitWidth = 633
        ExplicitHeight = 249
        inherited TabSheet1: TTabSheet
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 629
          ExplicitHeight = 314
        end
        inherited TabSheet2: TTabSheet
          ExplicitLeft = 0
          ExplicitTop = 0
          ExplicitWidth = 0
          ExplicitHeight = 0
        end
      end
    end
    inherited Panel2: TPanel
      Top = 251
      Width = 635
      ExplicitLeft = 0
      ExplicitTop = 344
      ExplicitWidth = 635
      inherited Button1: TButton
        Left = 387
        ExplicitLeft = 387
      end
      inherited Button2: TButton
        Left = 468
        ExplicitLeft = 468
      end
      inherited Button3: TButton
        Left = 549
        ExplicitLeft = 549
      end
    end
  end
end
