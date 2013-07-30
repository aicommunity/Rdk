inherited UHttpServerFrame: TUHttpServerFrame
  object Image1: TImage
    Left = 0
    Top = 0
    Width = 320
    Height = 240
    Align = alClient
    ExplicitLeft = 32
    ExplicitTop = 24
    ExplicitWidth = 249
    ExplicitHeight = 201
  end
  object IdHTTPServer: TIdHTTPServer
    Bindings = <
      item
        IP = '127.0.0.1'
        Port = 80
      end>
    AutoStartSession = True
    KeepAlive = True
    OnCommandGet = IdHTTPServerCommandGet
    Left = 16
    Top = 16
  end
  object PageProducer: TPageProducer
    Left = 56
    Top = 16
  end
end
