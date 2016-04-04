inherited ImportModelFromXmlFrame: TImportModelFromXmlFrame
  Width = 639
  Height = 393
  ExplicitWidth = 639
  ExplicitHeight = 393
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 639
    Height = 344
    Align = alClient
    TabOrder = 0
    ExplicitWidth = 320
    ExplicitHeight = 191
    object PageControl1: TPageControl
      Left = 1
      Top = 1
      Width = 637
      Height = 342
      ActivePage = TabSheet1
      Align = alClient
      TabOrder = 0
      ExplicitWidth = 318
      ExplicitHeight = 189
      object TabSheet1: TTabSheet
        Caption = 'Destination'
        ExplicitWidth = 310
        ExplicitHeight = 161
      end
      object TabSheet2: TTabSheet
        Caption = 'Xml Source'
        ImageIndex = 1
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 344
    Width = 639
    Height = 49
    Align = alBottom
    TabOrder = 1
    ExplicitLeft = -1
    ExplicitTop = 345
    DesignSize = (
      639
      49)
    object Button1: TButton
      Left = 391
      Top = 11
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = '<< Prev'
      TabOrder = 0
    end
    object Button2: TButton
      Left = 472
      Top = 11
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Next >>'
      TabOrder = 1
    end
    object Button3: TButton
      Left = 553
      Top = 11
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Finish'
      Default = True
      TabOrder = 2
    end
  end
end
