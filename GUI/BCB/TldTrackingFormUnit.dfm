object TldTrackingForm: TTldTrackingForm
  Left = 0
  Top = 0
  Caption = 'TldTrackingForm'
  ClientHeight = 494
  ClientWidth = 718
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Panel1: TPanel
    Left = 0
    Top = 453
    Width = 718
    Height = 41
    Align = alBottom
    TabOrder = 0
  end
  object Panel2: TPanel
    Left = 533
    Top = 0
    Width = 185
    Height = 453
    Align = alRight
    TabOrder = 1
    object Label1: TLabel
      Left = 6
      Top = 3
      Width = 76
      Height = 13
      Caption = 'Video source #:'
    end
    object Label2: TLabel
      Left = 6
      Top = 120
      Width = 79
      Height = 13
      Caption = 'Tracker number:'
    end
    object Label3: TLabel
      Left = 6
      Top = 160
      Width = 57
      Height = 13
      Caption = 'Tracker #2:'
      Visible = False
    end
    object GetFrameButton: TButton
      Left = 6
      Top = 52
      Width = 171
      Height = 25
      Caption = 'Get frame from video'
      TabOrder = 0
      OnClick = GetFrameButtonClick
    end
    object StartTrackingButton: TButton
      Left = 6
      Top = 241
      Width = 171
      Height = 25
      Caption = 'Start tracking'
      TabOrder = 1
      OnClick = StartTrackingButtonClick
    end
    object StopTrackingButton: TButton
      Left = 6
      Top = 272
      Width = 171
      Height = 25
      Caption = 'Stop tracking'
      TabOrder = 2
      OnClick = StopTrackingButtonClick
    end
    object EnableLogCheckBox: TCheckBox
      Left = 6
      Top = 335
      Width = 171
      Height = 17
      Caption = 'Enable log'
      Checked = True
      State = cbChecked
      TabOrder = 3
      OnClick = EnableLogCheckBoxClick
    end
    object GotoFirstFrameButton: TButton
      Left = 6
      Top = 303
      Width = 171
      Height = 25
      Caption = 'Goto first frame'
      TabOrder = 4
      OnClick = GotoFirstFrameButtonClick
    end
    object VideoSourceComboBox: TComboBox
      Left = 6
      Top = 22
      Width = 171
      Height = 21
      Style = csDropDownList
      TabOrder = 5
    end
    object SendObjectToButton: TButton
      Left = 6
      Top = 89
      Width = 171
      Height = 25
      Caption = 'Send selected object to ...'
      TabOrder = 6
      OnClick = SendObjectToButtonClick
    end
    object ObjectReceiverComboBox: TComboBox
      Left = 6
      Top = 136
      Width = 171
      Height = 21
      Style = csDropDownList
      TabOrder = 7
    end
    object ObjectReceiver2ComboBox: TComboBox
      Left = 6
      Top = 176
      Width = 171
      Height = 21
      Style = csDropDownList
      TabOrder = 8
      Visible = False
    end
    object CopyTrackerDataButton: TButton
      Left = 6
      Top = 206
      Width = 171
      Height = 25
      Caption = 'Copy Tracker Data'
      TabOrder = 9
      Visible = False
      OnClick = CopyTrackerDataButtonClick
    end
    object SaveTrackerDataButton: TButton
      Left = 6
      Top = 390
      Width = 171
      Height = 25
      Caption = 'Save tracker data'
      TabOrder = 10
      OnClick = SaveTrackerDataButtonClick
    end
    object LoadTrackerDataButton: TButton
      Left = 6
      Top = 421
      Width = 171
      Height = 25
      Caption = 'Load tracker data'
      TabOrder = 11
      OnClick = LoadTrackerDataButtonClick
    end
  end
  object Panel3: TPanel
    Left = 0
    Top = 0
    Width = 533
    Height = 453
    Align = alClient
    TabOrder = 2
    inline VideoOutputFrame1: TVideoOutputFrame
      Left = 1
      Top = 1
      Width = 531
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
        Width = 531
        Height = 379
        ExplicitWidth = 531
        ExplicitHeight = 379
        inherited Image: TImage
          Width = 527
          Height = 362
          ExplicitWidth = 527
          ExplicitHeight = 268
        end
        inherited VideoGrabber: TVideoGrabber
          Width = 527
          Height = 362
          ExplicitWidth = 527
          ExplicitHeight = 362
        end
      end
      inherited Panel1: TPanel
        Top = 410
        Width = 531
        Visible = False
        ExplicitTop = 410
        ExplicitWidth = 531
        inherited StopButton: TButton
          Left = 494
          ExplicitLeft = 494
        end
        inherited StartButton: TButton
          Left = 458
          ExplicitLeft = 458
        end
        inherited TimeEdit: TMaskEdit
          Left = 373
          ExplicitLeft = 373
        end
        inherited TrackBar: TTrackBar
          Width = 365
          ExplicitWidth = 365
        end
      end
      inherited Panel2: TPanel
        Top = 379
        Width = 531
        Visible = False
        ExplicitTop = 379
        ExplicitWidth = 531
        inherited SendToEdit: TEdit
          Width = 350
          ExplicitWidth = 350
        end
        inherited Button1: TButton
          Left = 373
          ExplicitLeft = 373
        end
        inherited SelectSendToButton: TButton
          Left = 458
          ExplicitLeft = 458
        end
      end
    end
  end
end
