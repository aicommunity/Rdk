object VideoGrabberControlForm: TVideoGrabberControlForm
  Left = 0
  Top = 0
  Caption = 'VideoGrabberControlForm'
  ClientHeight = 340
  ClientWidth = 599
  Color = clBtnFace
  Constraints.MinHeight = 330
  Constraints.MinWidth = 600
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  inline VideoGrabberControlFrame: TVideoGrabberControlFrame
    Left = 0
    Top = 0
    Width = 599
    Height = 299
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
    ExplicitWidth = 584
    ExplicitHeight = 292
    inherited VCapturePageControl: TPageControl
      Width = 599
      Height = 299
      ExplicitWidth = 584
      ExplicitHeight = 292
      inherited DeviceTabSheet: TTabSheet
        inherited VDCapturePanel: TPanel
          Width = 591
          Height = 268
          inherited ModeLabel: TLabel
            Width = 706
            ExplicitWidth = 560
          end
          inherited InputLabel: TLabel
            Width = 706
            ExplicitWidth = 560
          end
          inherited DeviceLabel: TLabel
            Width = 706
            ExplicitWidth = 560
          end
          inherited Label1: TLabel
            Width = 706
            ExplicitWidth = 560
          end
          inherited Label2: TLabel
            Width = 706
            ExplicitWidth = 560
          end
          inherited VideoSizeComboBox: TComboBox
            Width = 706
            ExplicitWidth = 560
          end
          inherited InputComboBox: TComboBox
            Width = 706
            ExplicitWidth = 560
          end
          inherited DeviceComboBox: TComboBox
            Width = 706
            ExplicitWidth = 560
          end
          inherited VideoSubTypeComboBox: TComboBox
            Width = 706
            ExplicitWidth = 560
          end
          inherited AnalogVideoStandardComboBox: TComboBox
            Width = 706
            ExplicitWidth = 560
          end
        end
      end
      inherited IPCameraTabSheet: TTabSheet
        ExplicitWidth = 576
        ExplicitHeight = 261
        inherited Panel1: TPanel
          Width = 591
          Height = 268
          ExplicitWidth = 576
          ExplicitHeight = 261
          inherited Label4: TLabel
            Width = 323
            ExplicitWidth = 308
          end
          inherited Label5: TLabel
            Width = 575
            ExplicitWidth = 560
          end
          inherited Label6: TLabel
            Width = 575
            ExplicitWidth = 560
          end
          inherited Label8: TLabel
            Left = 280
            Width = 234
            ExplicitLeft = 280
            ExplicitWidth = 219
          end
          inherited IPCameraUrlEdit: TEdit
            Width = 575
            ExplicitWidth = 560
          end
          inherited IPCameraUserNameEdit: TEdit
            Width = 575
            ExplicitWidth = 560
          end
          inherited IPCameraUserPasswordEdit: TEdit
            Width = 575
            ExplicitWidth = 560
          end
          inherited IPCameraControlPostfixEdit: TEdit
            Left = 280
            Width = 303
            ExplicitLeft = 280
            ExplicitWidth = 288
          end
        end
      end
      inherited VideoFileTabSheet: TTabSheet
        ExplicitWidth = 576
        ExplicitHeight = 261
        inherited VFCapturePanel: TPanel
          Width = 591
          Height = 268
          ExplicitWidth = 576
          ExplicitHeight = 261
          inherited VideoFileNameLabel: TLabel
            Width = 575
            ExplicitWidth = 560
          end
          inherited VFNameEdit: TEdit
            Width = 490
            ExplicitWidth = 475
          end
          inherited VFBrowseButton: TButton
            Left = 504
            ExplicitLeft = 489
          end
        end
      end
      inherited PictureFileTabSheet: TTabSheet
        ExplicitWidth = 576
        ExplicitHeight = 261
        inherited Label3: TLabel
          Width = 1099
          ExplicitWidth = 560
        end
        inherited ImageFileNameEdit: TEdit
          Width = 494
          ExplicitWidth = 494
        end
        inherited OpenImageFileButton: TButton
          Left = 508
          Top = 21
          Height = 23
          ExplicitLeft = 508
          ExplicitTop = 21
          ExplicitHeight = 23
        end
      end
      inherited ImageSequenceTabSheet: TTabSheet
        inherited Label7: TLabel
          Width = 837
          ExplicitWidth = 560
        end
        inherited ImageSequencePathEdit: TEdit
          Width = 496
          ExplicitWidth = 496
        end
        inherited ImageSequencePathCheckBox: TCheckBox
          TabOrder = 2
        end
        inherited ImageSequencePathBrowseButton: TButton
          Left = 510
          Top = 21
          Height = 23
          TabOrder = 1
          ExplicitLeft = 510
          ExplicitTop = 21
          ExplicitHeight = 23
        end
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 299
    Width = 599
    Height = 41
    Align = alBottom
    TabOrder = 1
    ExplicitLeft = 272
    ExplicitTop = 272
    ExplicitWidth = 185
    object CloseButton: TButton
      Left = 496
      Top = 8
      Width = 92
      Height = 25
      Caption = 'Apply && Close'
      TabOrder = 0
      OnClick = CloseButtonClick
    end
  end
end
