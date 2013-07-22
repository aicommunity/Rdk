object UHttpServerFrame: TUHttpServerFrame
  Left = 0
  Top = 0
  Width = 320
  Height = 240
  TabOrder = 0
  object Image1: TImage
    Left = 32
    Top = 24
    Width = 249
    Height = 201
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
    Left = 120
    Top = 88
  end
  object PageProducer: TPageProducer
    Left = 208
    Top = 32
  end
end
