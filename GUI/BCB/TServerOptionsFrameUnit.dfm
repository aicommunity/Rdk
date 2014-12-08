inherited ServerOptionsFrame: TServerOptionsFrame
  DesignSize = (
    320
    240)
  object BindAddressLabeledEdit: TLabeledEdit
    Left = 3
    Top = 24
    Width = 314
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    EditLabel.Width = 97
    EditLabel.Height = 13
    EditLabel.Caption = 'Server Bind Address'
    TabOrder = 0
  end
  object BindPortLabeledEdit: TLabeledEdit
    Left = 3
    Top = 72
    Width = 314
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    EditLabel.Width = 78
    EditLabel.Height = 13
    EditLabel.Caption = 'Server Bind Port'
    TabOrder = 1
  end
end
