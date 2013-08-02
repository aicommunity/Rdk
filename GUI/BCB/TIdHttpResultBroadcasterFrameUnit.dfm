inherited IdHttpResultBroadcasterFrame: TIdHttpResultBroadcasterFrame
  Width = 536
  Height = 376
  ExplicitWidth = 536
  ExplicitHeight = 376
  object GroupBox2: TGroupBox
    Left = 0
    Top = 0
    Width = 536
    Height = 376
    Align = alClient
    Caption = ' Client data '
    TabOrder = 0
    DesignSize = (
      536
      376)
    object ServerAddressLabeledEdit: TLabeledEdit
      Left = 8
      Top = 32
      Width = 521
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 153
      EditLabel.Height = 13
      EditLabel.Caption = 'Server Address (http://url:port)'
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
    object ImagesComponentOutputNameLabeledEdit: TLabeledEdit
      Left = 8
      Top = 261
      Width = 521
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 160
      EditLabel.Height = 13
      EditLabel.Caption = 'Images Component Output Name'
      TabOrder = 3
    end
    object ImagesComponentNameLabeledEdit: TLabeledEdit
      Left = 8
      Top = 213
      Width = 521
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      EditLabel.Width = 123
      EditLabel.Height = 13
      EditLabel.Caption = 'Images Component Name'
      TabOrder = 4
    end
    object EnableXmlTranslationCheckBox: TCheckBox
      Left = 8
      Top = 287
      Width = 193
      Height = 17
      Caption = 'Enable Xml Translation '
      TabOrder = 5
    end
    object EnableImagesTranslationCheckBox: TCheckBox
      Left = 8
      Top = 310
      Width = 249
      Height = 17
      Caption = 'Enable Images Translation'
      TabOrder = 6
    end
    object Panel1: TPanel
      Left = 2
      Top = 333
      Width = 532
      Height = 41
      Align = alBottom
      TabOrder = 7
      object ConnectButton: TButton
        Left = 6
        Top = 8
        Width = 75
        Height = 25
        Caption = 'Connect'
        TabOrder = 0
        OnClick = ConnectButtonClick
      end
      object DisconnectButton: TButton
        Left = 87
        Top = 8
        Width = 75
        Height = 25
        Caption = 'Disconnect'
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
      TabOrder = 8
    end
  end
  object IdHTTP: TIdHTTP
    OnDisconnected = IdHTTPDisconnected
    OnConnected = IdHTTPConnected
    AllowCookies = True
    ProxyParams.BasicAuthentication = False
    ProxyParams.ProxyPort = 0
    Request.ContentLength = -1
    Request.ContentRangeEnd = -1
    Request.ContentRangeStart = -1
    Request.ContentRangeInstanceLength = -1
    Request.Accept = 'text/html, */*'
    Request.BasicAuthentication = False
    Request.UserAgent = 'Mozilla/3.0 (compatible; Indy Library)'
    Request.Ranges.Units = 'bytes'
    Request.Ranges = <>
    HTTPOptions = [hoForceEncodeParams]
    Left = 120
    Top = 32
  end
  object PageProducer: TPageProducer
    Left = 168
    Top = 32
  end
end
