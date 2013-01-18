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
    ExplicitHeight = 292
    inherited VCapturePageControl: TPageControl
      Width = 584
      Height = 292
      ActivePage = VideoGrabberControlFrame.DeviceTabSheet
      ExplicitWidth = 584
      ExplicitHeight = 292
      inherited DeviceTabSheet: TTabSheet
        ExplicitWidth = 576
        ExplicitHeight = 261
        inherited VDCapturePanel: TPanel
          Width = 576
          Height = 261
          ExplicitWidth = 576
          ExplicitHeight = 261
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
          inherited Label4: TLabel
            ExplicitWidth = 560
          end
          inherited Label5: TLabel
            ExplicitWidth = 560
          end
          inherited Label6: TLabel
            ExplicitWidth = 560
          end
        end
      end
      inherited VideoFileTabSheet: TTabSheet
        inherited VFCapturePanel: TPanel
          Width = 576
          Height = 261
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
          Width = 953
          ExplicitWidth = 560
        end
        inherited ImageFileNameEdit: TEdit
          Width = 476
          ExplicitWidth = 476
        end
        inherited OpenImageFileButton: TButton
          Left = 490
          ExplicitLeft = 490
        end
      end
      inherited ImageSequenceTabSheet: TTabSheet
        inherited Label7: TLabel
          Width = 691
          ExplicitWidth = 560
        end
        inherited ImageSequencePathEdit: TEdit
          Width = 476
          ExplicitWidth = 476
        end
        inherited ImageSequencePathCheckBox: TCheckBox
          TabOrder = 2
        end
        inherited ImageSequencePathBrowseButton: TButton
          Left = 490
          TabOrder = 1
          ExplicitLeft = 490
        end
      end
    end
  end
end
