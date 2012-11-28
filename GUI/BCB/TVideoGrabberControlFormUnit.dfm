object VideoGrabberControlForm: TVideoGrabberControlForm
  Left = 0
  Top = 0
  Caption = 'VideoGrabberControlForm'
  ClientHeight = 292
  ClientWidth = 584
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
    Width = 584
    Height = 292
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
    ExplicitHeight = 298
    inherited VCapturePageControl: TPageControl
      Width = 584
      Height = 292
      ExplicitWidth = 584
      ExplicitHeight = 298
      inherited DeviceTabSheet: TTabSheet
        ExplicitWidth = 576
        ExplicitHeight = 267
        inherited VDCapturePanel: TPanel
          Width = 576
          Height = 261
          ExplicitWidth = 576
          ExplicitHeight = 267
          inherited ModeLabel: TLabel
            Width = 560
            ExplicitWidth = 560
          end
          inherited InputLabel: TLabel
            Width = 560
            ExplicitWidth = 560
          end
          inherited DeviceLabel: TLabel
            Width = 560
            ExplicitWidth = 560
          end
          inherited Label1: TLabel
            Width = 560
            ExplicitWidth = 560
          end
          inherited Label2: TLabel
            Width = 560
            ExplicitWidth = 560
          end
          inherited VideoSizeComboBox: TComboBox
            Width = 560
            ExplicitWidth = 560
          end
          inherited InputComboBox: TComboBox
            Width = 560
            ExplicitWidth = 560
          end
          inherited DeviceComboBox: TComboBox
            Width = 560
            ExplicitWidth = 560
          end
          inherited VideoSubTypeComboBox: TComboBox
            Width = 560
            ExplicitWidth = 560
          end
          inherited AnalogVideoStandardComboBox: TComboBox
            Width = 560
            ExplicitWidth = 560
          end
        end
      end
      inherited IPCameraTabSheet: TTabSheet
        inherited Panel1: TPanel
          Width = 576
          Height = 267
          inherited Label4: TLabel
            Width = 560
            ExplicitWidth = 560
          end
          inherited Label5: TLabel
            Width = 560
            ExplicitWidth = 560
          end
          inherited Label6: TLabel
            Width = 560
            ExplicitWidth = 560
          end
          inherited IPCameraUrlEdit: TEdit
            Width = 560
          end
          inherited IPCameraUserNameEdit: TEdit
            Width = 560
          end
          inherited IPCameraUserPasswordEdit: TEdit
            Width = 560
          end
        end
      end
      inherited VideoFileTabSheet: TTabSheet
        inherited VFCapturePanel: TPanel
          Width = 576
          Height = 267
          inherited VideoFileNameLabel: TLabel
            Width = 560
            ExplicitWidth = 560
          end
          inherited VFNameEdit: TEdit
            Width = 475
          end
          inherited VFBrowseButton: TButton
            Left = 489
          end
        end
      end
      inherited PictureFileTabSheet: TTabSheet
        inherited Label3: TLabel
          Width = 822
          ExplicitWidth = 560
        end
        inherited ImageFileNameEdit: TEdit
          Width = 476
          ExplicitWidth = 544
        end
        inherited OpenImageFileButton: TButton
          Left = 490
          ExplicitLeft = 558
        end
      end
      inherited ImageSequenceTabSheet: TTabSheet
        inherited Label7: TLabel
          Width = 560
        end
        inherited ImageSequencePathEdit: TEdit
          Width = 482
          ExplicitWidth = 482
        end
        inherited ImageSequencePathBrowseButton: TButton
          Left = 496
          ExplicitLeft = 496
        end
      end
    end
  end
end
