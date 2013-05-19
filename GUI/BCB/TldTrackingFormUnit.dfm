object TldTrackingForm: TTldTrackingForm
  Left = 0
  Top = 0
  Caption = 'TldTrackingForm'
  ClientHeight = 494
  ClientWidth = 759
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 453
    Width = 759
    Height = 41
    Align = alBottom
    TabOrder = 0
    ExplicitWidth = 718
    DesignSize = (
      759
      41)
    object SaveTrackerDataButton: TButton
      Left = 545
      Top = 6
      Width = 98
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Save tracker data'
      TabOrder = 0
      OnClick = SaveTrackerDataButtonClick
    end
    object LoadTrackerDataButton: TButton
      Left = 649
      Top = 6
      Width = 102
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Load tracker data'
      TabOrder = 1
      OnClick = LoadTrackerDataButtonClick
    end
    object StartTrackingButton: TButton
      Left = 8
      Top = 6
      Width = 129
      Height = 25
      Caption = 'Start tracking'
      TabOrder = 2
      OnClick = StartTrackingButtonClick
    end
    object StopTrackingButton: TButton
      Left = 143
      Top = 6
      Width = 130
      Height = 25
      Caption = 'Stop tracking'
      TabOrder = 3
      OnClick = StopTrackingButtonClick
    end
  end
  object Panel2: TPanel
    Left = 574
    Top = 0
    Width = 185
    Height = 453
    Align = alRight
    TabOrder = 1
    ExplicitLeft = 533
    object GetFrameButton: TButton
      Left = 1
      Top = 1
      Width = 183
      Height = 25
      Align = alTop
      Caption = 'Get frame from video'
      TabOrder = 0
      OnClick = GetFrameButtonClick
      ExplicitLeft = 6
      ExplicitTop = 52
      ExplicitWidth = 171
    end
    object EnableLogCheckBox: TCheckBox
      Left = 1
      Top = 435
      Width = 183
      Height = 17
      Align = alBottom
      Caption = 'Enable log'
      Checked = True
      State = cbChecked
      TabOrder = 1
      OnClick = EnableLogCheckBoxClick
      ExplicitLeft = 6
      ExplicitTop = 430
      ExplicitWidth = 171
    end
    object CopyTrackerDataButton: TButton
      Left = 1
      Top = 410
      Width = 183
      Height = 25
      Align = alBottom
      Caption = 'Copy Tracker Data'
      TabOrder = 2
      Visible = False
      OnClick = CopyTrackerDataButtonClick
      ExplicitLeft = 2
      ExplicitTop = 399
      ExplicitWidth = 171
    end
    object PageControl1: TPageControl
      Left = -5
      Top = 26
      Width = 189
      Height = 384
      ActivePage = PointsTabSheet
      Align = alRight
      TabOrder = 3
      object RectangleTabSheet: TTabSheet
        Caption = 'Rectangle'
        object Label2: TLabel
          Left = 7
          Top = 40
          Width = 79
          Height = 13
          Caption = 'Tracker number:'
        end
        object SendObjectToButton: TButton
          Left = 7
          Top = 9
          Width = 171
          Height = 25
          Caption = 'Send selected object to ...'
          TabOrder = 0
          OnClick = SendObjectToButtonClick
        end
        object ObjectReceiverComboBox: TComboBox
          Left = 7
          Top = 56
          Width = 171
          Height = 21
          Style = csDropDownList
          TabOrder = 1
        end
      end
      object PointsTabSheet: TTabSheet
        Caption = 'PointsTabSheet'
        ImageIndex = 1
        OnShow = PointsTabSheetShow
        object SendPointsButton: TButton
          Left = 0
          Top = 105
          Width = 181
          Height = 25
          Align = alTop
          Caption = 'Send selected objects'
          TabOrder = 0
          OnClick = SendPointsButtonClick
          ExplicitLeft = -3
          ExplicitTop = 136
        end
        object GroupBox1: TGroupBox
          Left = 0
          Top = 0
          Width = 181
          Height = 105
          Align = alTop
          Caption = ' Points rect options '
          TabOrder = 1
          ExplicitTop = 72
          ExplicitWidth = 185
          object RectWidthLabeledEdit: TLabeledEdit
            Left = 7
            Top = 32
            Width = 170
            Height = 21
            EditLabel.Width = 51
            EditLabel.Height = 13
            EditLabel.Caption = 'Rect width'
            TabOrder = 0
            Text = '30'
          end
          object RectHeightLabeledEdit: TLabeledEdit
            Left = 7
            Top = 74
            Width = 170
            Height = 21
            EditLabel.Width = 55
            EditLabel.Height = 13
            EditLabel.Caption = 'Rect height'
            TabOrder = 1
            Text = '30'
          end
        end
      end
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 0
    Width = 574
    Height = 453
    Align = alClient
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 2
    ExplicitWidth = 533
    inline VideoOutputFrame1: TVideoOutputFrame
      Left = 1
      Top = 1
      Width = 572
      Height = 451
      Align = alClient
      Constraints.MinHeight = 100
      Constraints.MinWidth = 200
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      PopupMenu = VideoOutputFrame1.PopupMenu
      TabOrder = 0
      TabStop = True
      ExplicitLeft = 1
      ExplicitTop = 1
      ExplicitWidth = 531
      ExplicitHeight = 451
      inherited GroupBox: TGroupBox
        Width = 572
        Height = 379
        ExplicitWidth = 531
        ExplicitHeight = 379
        inherited Image: TImage
          Width = 568
          Height = 362
          ExplicitWidth = 527
          ExplicitHeight = 268
        end
        inherited VideoGrabber: TVideoGrabber
          Width = 568
          Height = 362
          ExplicitWidth = 527
          ExplicitHeight = 362
        end
      end
      inherited Panel1: TPanel
        Top = 410
        Width = 572
        Visible = False
        ExplicitTop = 410
        ExplicitWidth = 531
        inherited StopButton: TButton
          Left = 535
          ExplicitLeft = 494
        end
        inherited StartButton: TButton
          Left = 499
          ExplicitLeft = 458
        end
        inherited TimeEdit: TMaskEdit
          Left = 414
          ExplicitLeft = 373
        end
        inherited TrackBar: TTrackBar
          Width = 406
          ExplicitWidth = 365
        end
      end
      inherited Panel2: TPanel
        Top = 379
        Width = 572
        Visible = False
        ExplicitTop = 379
        ExplicitWidth = 531
        inherited SendToEdit: TEdit
          Width = 391
          ExplicitWidth = 350
        end
        inherited Button1: TButton
          Left = 414
          ExplicitLeft = 373
        end
        inherited SelectSendToButton: TButton
          Left = 499
          ExplicitLeft = 458
        end
      end
    end
  end
  object OpenDialog1: TOpenDialog
    Options = [ofHideReadOnly, ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Left = 296
    Top = 192
  end
end
