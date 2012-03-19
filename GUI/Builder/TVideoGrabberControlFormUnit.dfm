object VideoGrabberControlForm: TVideoGrabberControlForm
  Left = 0
  Top = 0
  Caption = 'VideoGrabberControlForm'
  ClientHeight = 307
  ClientWidth = 583
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  inline VideoGrabberControlFrame: TVideoGrabberControlFrame
    Left = 0
    Top = 0
    Width = 583
    Height = 307
    Align = alClient
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 0
    ExplicitLeft = 130
    ExplicitTop = -1
    inherited VCapturePageControl: TPageControl
      Width = 583
      Height = 307
      inherited DeviceTabSheet: TTabSheet
        inherited VDCapturePanel: TPanel
          Width = 575
          Height = 276
          inherited ModeLabel: TLabel
            Width = 559
          end
          inherited InputLabel: TLabel
            Width = 559
          end
          inherited DeviceLabel: TLabel
            Width = 559
          end
          inherited Label1: TLabel
            Width = 559
          end
          inherited Label2: TLabel
            Width = 559
          end
          inherited VideoSizeComboBox: TComboBox
            Width = 559
          end
          inherited InputComboBox: TComboBox
            Width = 559
          end
          inherited DeviceComboBox: TComboBox
            Width = 559
          end
          inherited VideoSubTypeComboBox: TComboBox
            Width = 559
          end
          inherited AnalogVideoStandardComboBox: TComboBox
            Width = 559
          end
        end
      end
      inherited VideoFileTabSheet: TTabSheet
        inherited VFCapturePanel: TPanel
          Width = 575
          Height = 276
        end
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
