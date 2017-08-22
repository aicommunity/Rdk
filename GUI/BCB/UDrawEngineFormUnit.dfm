object UDrawEngineForm: TUDrawEngineForm
  Left = 0
  Top = 0
  Caption = 'Model Editor'
  ClientHeight = 369
  ClientWidth = 749
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter1: TSplitter
    Left = 509
    Top = 0
    Width = 11
    Height = 320
    Align = alRight
    ExplicitHeight = 369
  end
  object Panel1: TPanel
    Left = 520
    Top = 0
    Width = 229
    Height = 320
    Align = alRight
    TabOrder = 0
    inline UClassesListFrame: TUClassesListFrame
      Left = 1
      Top = 1
      Width = 227
      Height = 318
      Align = alClient
      DoubleBuffered = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 0
      ExplicitLeft = 1
      ExplicitTop = 1
      ExplicitWidth = 227
      ExplicitHeight = 318
      inherited PageControl: TPageControl
        Width = 227
        Height = 318
        ExplicitWidth = 227
        ExplicitHeight = 318
        inherited NameTabSheet: TTabSheet
          inherited StringGrid: TStringGrid
            Width = 227
            Height = 318
            OnMouseMove = UClassesListFrameStringGridMouseMove
            OnMouseUp = UClassesListFrameStringGridMouseUp
            ExplicitWidth = 227
            ExplicitHeight = 318
          end
        end
        inherited LibsTabSheet: TTabSheet
          ExplicitLeft = 4
          ExplicitTop = 24
          ExplicitWidth = 499
          ExplicitHeight = 515
        end
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 509
    Height = 320
    Align = alClient
    TabOrder = 1
    object ScrollBox: TScrollBox
      Left = 1
      Top = 1
      Width = 507
      Height = 318
      Align = alClient
      TabOrder = 0
      OnResize = ScrollBoxResize
      object Image: TImage
        Left = 0
        Top = 0
        Width = 305
        Height = 241
        OnDragDrop = ImageDragDrop
        OnDragOver = ImageDragOver
        OnMouseDown = ImageMouseDown
        OnMouseMove = ImageMouseMove
        OnMouseUp = ImageMouseUp
      end
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 320
    Width = 749
    Height = 49
    Align = alBottom
    TabOrder = 2
    DesignSize = (
      749
      49)
    object Label1: TLabel
      Left = 3
      Top = 5
      Width = 49
      Height = 13
      Caption = 'Font Type'
    end
    object Label2: TLabel
      Left = 152
      Top = 5
      Width = 43
      Height = 13
      Caption = 'Font size'
    end
    object RectWidthLabeledEdit: TLabeledEdit
      Left = 216
      Top = 24
      Width = 41
      Height = 21
      EditLabel.Width = 28
      EditLabel.Height = 13
      EditLabel.Caption = 'Width'
      TabOrder = 2
      Text = '80'
    end
    object FontTypeComboBox: TComboBox
      Left = 3
      Top = 24
      Width = 142
      Height = 21
      Style = csDropDownList
      TabOrder = 0
      OnSelect = FontTypeComboBoxSelect
    end
    object FontSizeComboBox: TComboBox
      Left = 152
      Top = 24
      Width = 57
      Height = 21
      Style = csDropDownList
      TabOrder = 1
    end
    object RectHeightLabeledEdit: TLabeledEdit
      Left = 263
      Top = 24
      Width = 41
      Height = 21
      EditLabel.Width = 31
      EditLabel.Height = 13
      EditLabel.Caption = 'Height'
      TabOrder = 3
      Text = '20'
    end
    object ApplyButton: TButton
      Left = 583
      Top = 12
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Apply'
      Default = True
      TabOrder = 4
      OnClick = ApplyButtonClick
    end
    object RestoreButton: TButton
      Left = 664
      Top = 12
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Restore'
      TabOrder = 5
      OnClick = RestoreButtonClick
    end
  end
  object PopupMenu: TPopupMenu
    Left = 384
    Top = 176
    object Breakinputlink1: TMenuItem
      Caption = 'Break link'
      OnClick = Breakinputlink1Click
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object Createlonglink1: TMenuItem
      Caption = 'Create link'
      OnClick = Createlonglink1Click
    end
    object Finishlonglink1: TMenuItem
      Caption = 'Finish link'
      Enabled = False
      OnClick = Finishlonglink1Click
    end
    object Cancellonglink1: TMenuItem
      Caption = 'Cancel link'
      Enabled = False
      OnClick = Cancellonglink1Click
    end
  end
end
