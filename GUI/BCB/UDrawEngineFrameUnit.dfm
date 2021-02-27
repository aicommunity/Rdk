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
        Height = 401
        ExplicitWidth = 227
        ExplicitHeight = 401
        inherited NameTabSheet: TTabSheet
          ExplicitWidth = 219
          ExplicitHeight = 373
          inherited StringGrid: TStringGrid
            Width = 219
            Height = 373
            OnMouseMove = UClassesListFrameStringGridMouseMove
            OnMouseUp = UClassesListFrameStringGridMouseUp
            ExplicitWidth = 219
            ExplicitHeight = 373
          end
        end
        inherited LibsTabSheet: TTabSheet
          ExplicitWidth = 219
          ExplicitHeight = 373
          inherited TreeView: TTreeView
            Width = 219
            Height = 373
            OnMouseMove = UClassesListFrameTreeViewMouseMove
            OnMouseUp = UClassesListFrameTreeViewMouseUp
            ExplicitWidth = 219
            ExplicitHeight = 373
          end
        end
        inherited LibsControlTabSheet: TTabSheet
          ExplicitWidth = 219
          ExplicitHeight = 373
          inherited Splitter1: TSplitter
            Top = 83
            Width = 219
            ExplicitTop = 288
            ExplicitWidth = 219
          end
          inherited GroupBox1: TGroupBox
            Width = 219
            Height = 83
            ExplicitWidth = 219
            ExplicitHeight = 83
            inherited LibsListStringGrid: TStringGrid
              Width = 215
              Height = 66
              ExplicitWidth = 215
              ExplicitHeight = 66
            end
          end
          inherited GroupBox2: TGroupBox
            Top = 90
            Width = 219
            Height = 283
            ExplicitTop = 90
            ExplicitWidth = 219
            ExplicitHeight = 283
            inherited LibComponentListStringGrid: TStringGrid
              Width = 215
              Height = 266
              OnMouseMove = UClassesListFrameLibComponentListStringGridMouseMove
              OnMouseUp = UClassesListFrameLibComponentListStringGridMouseUp
              ExplicitWidth = 215
              ExplicitHeight = 266
            end
          end
        end
      end
      inherited SearchEdit: TEdit
        Width = 227
        ExplicitWidth = 227
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
      ExplicitLeft = -4
      object Image: TImage
        Left = 20
        Top = 19
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
    object N7: TMenuItem
      Caption = '-'
    end
    object SwitchLink1: TMenuItem
      Caption = 'Switch Link'
      OnClick = SwitchLink1Click
    end
    object FinishSwitching1: TMenuItem
      Caption = 'Finish Switching'
      Enabled = False
      OnClick = FinishSwitching1Click
    end
    object CancelSwitching1: TMenuItem
      Caption = 'Cancel Switching'
      Enabled = False
      OnClick = CancelSwitching1Click
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
    object Clone1: TMenuItem
      Caption = 'Clone'
      OnClick = Clone1Click
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
    object N8: TMenuItem
      Caption = '-'
    end
    object Droptoruntimelibrary1: TMenuItem
      Caption = 'Drop to runtime library'
      OnClick = Droptoruntimelibrary1Click
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
  object FileOpenDialog: TFileOpenDialog
    FavoriteLinks = <>
    FileTypes = <>
    Options = [fdoPickFolders, fdoForceFileSystem, fdoPathMustExist]
    Left = 396
    Top = 44
  end
end
