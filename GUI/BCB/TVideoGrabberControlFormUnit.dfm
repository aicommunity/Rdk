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
    ExplicitWidth = 599
    ExplicitHeight = 299
    inherited VCapturePageControl: TPageControl
      Width = 599
      Height = 299
      ActivePage = VideoGrabberControlFrame.ImageSequenceTabSheet
      ExplicitWidth = 599
      ExplicitHeight = 299
      inherited DeviceTabSheet: TTabSheet
        inherited VDCapturePanel: TPanel
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
            ExplicitWidth = 706
          end
          inherited InputComboBox: TComboBox
            Width = 706
            ExplicitWidth = 706
          end
          inherited DeviceComboBox: TComboBox
            Width = 706
            ExplicitWidth = 706
          end
          inherited VideoSubTypeComboBox: TComboBox
            Width = 706
            ExplicitWidth = 706
          end
          inherited AnalogVideoStandardComboBox: TComboBox
            Width = 706
            ExplicitWidth = 706
          end
        end
      end
      inherited IPCameraTabSheet: TTabSheet
        inherited Panel1: TPanel
          inherited Label4: TLabel
            ExplicitWidth = 308
          end
          inherited Label5: TLabel
            ExplicitWidth = 560
          end
          inherited Label6: TLabel
            ExplicitWidth = 560
          end
          inherited Label8: TLabel
            Left = 280
            Width = 88
            ExplicitLeft = 280
            ExplicitWidth = 219
          end
          inherited IPCameraControlPostfixEdit: TEdit
            Left = 280
            Width = 157
            ExplicitLeft = 280
            ExplicitWidth = 157
          end
        end
      end
      inherited VideoFileTabSheet: TTabSheet
        ExplicitWidth = 591
        ExplicitHeight = 268
        inherited VFCapturePanel: TPanel
          Width = 591
          Height = 268
          ExplicitWidth = 591
          ExplicitHeight = 268
          inherited VideoFileNameLabel: TLabel
            Width = 575
            ExplicitWidth = 560
          end
          inherited VFNameEdit: TEdit
            Width = 490
            ExplicitWidth = 490
          end
          inherited VFBrowseButton: TButton
            Left = 504
            ExplicitLeft = 504
          end
        end
      end
      inherited PictureFileTabSheet: TTabSheet
        inherited Label3: TLabel
          Width = 1245
          ExplicitWidth = 560
        end
        inherited ImageFileNameEdit: TEdit
          Width = 491
          ExplicitWidth = 491
        end
        inherited OpenImageFileButton: TButton
          Left = 505
          Top = 21
          Height = 23
          ExplicitLeft = 505
          ExplicitTop = 21
          ExplicitHeight = 23
        end
      end
      inherited ImageSequenceTabSheet: TTabSheet
        inherited Label7: TLabel
          Width = 983
          ExplicitWidth = 560
        end
        inherited ImageSequencePathEdit: TEdit
          Width = 491
          ExplicitWidth = 491
        end
        inherited ImageSequencePathCheckBox: TCheckBox
          TabOrder = 2
        end
        inherited ImageSequencePathBrowseButton: TButton
          Left = 505
          Top = 21
          Height = 23
          TabOrder = 1
          ExplicitLeft = 505
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
    DesignSize = (
      599
      41)
    object CloseButton: TButton
      Left = 496
      Top = 6
      Width = 92
      Height = 25
      Anchors = [akRight, akBottom]
      Caption = 'Apply && Close'
      TabOrder = 0
      OnClick = CloseButtonClick
    end
  end
end
