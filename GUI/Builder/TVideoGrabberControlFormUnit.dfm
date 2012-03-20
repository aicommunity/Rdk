object VideoGrabberControlForm: TVideoGrabberControlForm
  Left = 0
  Top = 0
  Caption = 'VideoGrabberControlForm'
  ClientHeight = 262
  ClientWidth = 584
  Color = clBtnFace
  Constraints.MinHeight = 300
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
    Height = 262
    Align = alClient
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 0
    ExplicitLeft = 130
    ExplicitTop = -4
    inherited VCapturePageControl: TPageControl
      Width = 584
      Height = 262
      inherited DeviceTabSheet: TTabSheet
        inherited VDCapturePanel: TPanel
          Width = 576
          Height = 231
          inherited ModeLabel: TLabel
            Width = 560
          end
          inherited InputLabel: TLabel
            Width = 560
          end
          inherited DeviceLabel: TLabel
            Width = 560
          end
          inherited Label1: TLabel
            Width = 560
          end
          inherited Label2: TLabel
            Width = 560
          end
          inherited VideoSizeComboBox: TComboBox
            Width = 560
          end
          inherited InputComboBox: TComboBox
            Width = 560
          end
          inherited DeviceComboBox: TComboBox
            Width = 560
          end
          inherited VideoSubTypeComboBox: TComboBox
            Width = 560
          end
          inherited AnalogVideoStandardComboBox: TComboBox
            Width = 560
          end
        end
      end
      inherited VideoFileTabSheet: TTabSheet
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
      end
      inherited PictureFileTabSheet: TTabSheet
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
      end
    end
  end
end
