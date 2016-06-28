inherited UDrawEngineFrame: TUDrawEngineFrame
  Width = 781
  Height = 473
  OnContextPopup = FrameContextPopup
  ExplicitWidth = 781
  ExplicitHeight = 473
  object Splitter1: TSplitter
    Left = 541
    Top = 0
    Width = 11
    Height = 424
    Align = alRight
    ExplicitLeft = 509
    ExplicitHeight = 369
  end
  object Panel1: TPanel
    Left = 552
    Top = 0
    Width = 229
    Height = 424
    Align = alRight
    TabOrder = 0
    inline UClassesListFrame: TUClassesListFrame
      Left = 1
      Top = 1
      Width = 227
      Height = 422
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
      ExplicitHeight = 422
      inherited PageControl: TPageControl
        Width = 227
        Height = 422
        ActivePage = UClassesListFrame.LibsControlTabSheet
        ExplicitWidth = 227
        ExplicitHeight = 422
        inherited NameTabSheet: TTabSheet
          ExplicitLeft = 4
          ExplicitTop = 24
          ExplicitWidth = 499
          ExplicitHeight = 515
          inherited StringGrid: TStringGrid
            OnMouseMove = UClassesListFrameStringGridMouseMove
            OnMouseUp = UClassesListFrameStringGridMouseUp
          end
        end
        inherited LibsTabSheet: TTabSheet
          ExplicitLeft = 4
          ExplicitTop = 24
          ExplicitWidth = 499
          ExplicitHeight = 515
          inherited TreeView: TTreeView
            OnMouseMove = UClassesListFrameTreeViewMouseMove
            OnMouseUp = UClassesListFrameTreeViewMouseUp
          end
        end
        inherited LibsControlTabSheet: TTabSheet
          ExplicitLeft = 4
          ExplicitTop = 24
          ExplicitWidth = 219
          ExplicitHeight = 394
          inherited Splitter1: TSplitter
            Top = 104
            Width = 219
            ExplicitTop = 288
            ExplicitWidth = 219
          end
          inherited Panel1: TPanel
            Top = 271
            Width = 219
            ExplicitTop = 271
            ExplicitWidth = 219
            inherited LoadLibraryButton: TButton
              Width = 217
              ExplicitWidth = 217
            end
            inherited CreateRuntimeLibraryButton: TButton
              Width = 217
              ExplicitWidth = 217
            end
            inherited DeleteLibraryButton: TButton
              Width = 217
              ExplicitWidth = 217
            end
            inherited RenameRuntimeLibraryButton: TButton
              Width = 217
              ExplicitWidth = 217
            end
            inherited AddClassButton: TButton
              Width = 217
              OnClick = UClassesListFrameAddClassButtonClick
              ExplicitWidth = 217
            end
          end
          inherited GroupBox1: TGroupBox
            Width = 219
            Height = 104
            ExplicitWidth = 219
            ExplicitHeight = 104
            inherited LibsListStringGrid: TStringGrid
              Width = 215
              Height = 87
              ExplicitWidth = 215
              ExplicitHeight = 87
            end
          end
          inherited GroupBox2: TGroupBox
            Top = 111
            Width = 219
            ExplicitTop = 111
            ExplicitWidth = 219
            inherited LibComponentListStringGrid: TStringGrid
              Width = 215
              OnMouseMove = UClassesListFrameLibComponentListStringGridMouseMove
              OnMouseUp = UClassesListFrameLibComponentListStringGridMouseUp
              ExplicitWidth = 215
            end
          end
        end
      end
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 541
    Height = 424
    Align = alClient
    TabOrder = 1
    object ScrollBox: TScrollBox
      Left = 1
      Top = 1
      Width = 539
      Height = 422
      Align = alClient
      TabOrder = 0
      OnResize = ScrollBoxResize
      object Image: TImage
        Left = 20
        Top = 14
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
    Top = 424
    Width = 781
    Height = 49
    Align = alBottom
    TabOrder = 2
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
      Text = '100'
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
      Left = 418
      Top = 13
      Width = 75
      Height = 25
      Caption = 'Apply'
      Default = True
      TabOrder = 4
      OnClick = ApplyButtonClick
    end
    object RestoreButton: TButton
      Left = 499
      Top = 13
      Width = 75
      Height = 25
      Caption = 'Restore'
      TabOrder = 5
      OnClick = RestoreButtonClick
    end
    object ShowLinkDetailCheckBox: TCheckBox
      Left = 310
      Top = 24
      Width = 107
      Height = 17
      Caption = 'Show links detail'
      TabOrder = 6
    end
  end
  object PopupMenu: TPopupMenu
    Left = 384
    Top = 176
    object Breakinputlink1: TMenuItem
      Caption = 'View/Break link'
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
    object N1: TMenuItem
      Caption = '-'
    end
    object StartMoving1: TMenuItem
      Caption = 'Start moving'
      OnClick = StartMoving1Click
    end
    object Finishmoving1: TMenuItem
      Caption = 'Finish moving'
      Enabled = False
      OnClick = Finishmoving1Click
    end
    object Cancelmoving1: TMenuItem
      Caption = 'Cancel moving'
      Enabled = False
      OnClick = Cancelmoving1Click
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object Rename1: TMenuItem
      Caption = 'Rename'
      OnClick = Rename1Click
    end
    object Delete1: TMenuItem
      Caption = 'Delete'
      OnClick = Delete1Click
    end
    object N6: TMenuItem
      Caption = '-'
    end
    object CopynametoClipboard1: TMenuItem
      Caption = 'Copy name to Clipboard'
      OnClick = CopynametoClipboard1Click
    end
    object CopylongnametoClipboard1: TMenuItem
      Caption = 'Copy long name to Clipboard'
      OnClick = CopylongnametoClipboard1Click
    end
    object CopyclasstoClipboard1: TMenuItem
      Caption = 'Copy class to Clipboard'
      OnClick = CopyclasstoClipboard1Click
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object Reset1: TMenuItem
      Caption = 'Reset'
    end
    object Calculate1: TMenuItem
      Caption = 'Calculate'
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object GUI1: TMenuItem
      Caption = 'GUI'
    end
  end
end
