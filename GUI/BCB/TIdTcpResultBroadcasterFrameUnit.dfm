inherited IdTcpResultBroadcasterFrame: TIdTcpResultBroadcasterFrame
  Width = 536
  Height = 279
  ExplicitWidth = 536
  ExplicitHeight = 279
  object GroupBox2: TGroupBox
    Left = 0
    Top = 0
    Width = 536
    Height = 279
    Align = alClient
    Caption = ' Client data '
    TabOrder = 0
    ExplicitHeight = 258
    DesignSize = (
      536
      279)
    object ServerAddressLabeledEdit: TLabeledEdit
      Left = 8
      Top = 32
      Width = 521
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 121
      EditLabel.Height = 13
      EditLabel.Caption = 'Server Address (url:port)'
      TabOrder = 0
    end
    object XmlComponentNameLabeledEdit: TLabeledEdit
      Left = 8
      Top = 120
      Width = 521
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 104
      EditLabel.Height = 13
      EditLabel.Caption = 'Xml Component Name'
      TabOrder = 1
    end
    object XmlComponentStateNameLabeledEdit: TLabeledEdit
      Left = 8
      Top = 168
      Width = 521
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 133
      EditLabel.Height = 13
      EditLabel.Caption = 'Xml Component State Name'
      TabOrder = 2
    end
    object EnableXmlTranslationCheckBox: TCheckBox
      Left = 3
      Top = 192
      Width = 193
      Height = 17
      Caption = 'Enable Xml Translation '
      TabOrder = 3
      OnClick = EnableXmlTranslationCheckBoxClick
    end
    object Panel1: TPanel
      Left = 2
      Top = 236
      Width = 532
      Height = 41
      Align = alBottom
      TabOrder = 4
      ExplicitTop = 215
      object ConnectButton: TButton
        Left = 6
        Top = 8
        Width = 75
        Height = 25
        Caption = 'Apply'
        TabOrder = 0
        OnClick = ConnectButtonClick
      end
      object DisconnectButton: TButton
        Left = 87
        Top = 8
        Width = 75
        Height = 25
        Caption = 'Restore'
        TabOrder = 1
        OnClick = DisconnectButtonClick
      end
    end
    object ChannelIndexLabeledEdit: TLabeledEdit
      Left = 8
      Top = 75
      Width = 521
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 70
      EditLabel.Height = 13
      EditLabel.Caption = 'Channel Index'
      TabOrder = 5
    end
  end
  object IdTCPClient: TIdTCPClient
    ConnectTimeout = 0
    IPVersion = Id_IPv4
    Port = 0
    ReadTimeout = -1
    Left = 232
    Top = 32
  end
end
