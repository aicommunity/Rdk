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
    ExplicitLeft = 131
    ExplicitTop = -7
    inherited VCapturePageControl: TPageControl
      Width = 584
      Height = 262
      inherited VideoFileTabSheet: TTabSheet
        inherited VFCapturePanel: TPanel
          Width = 576
          Height = 231
          inherited VideoFileNameLabel: TLabel
            Width = 560
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
          Width = 560
        end
        inherited ImageFileNameEdit: TEdit
          Width = 475
        end
        inherited OpenImageFileButton: TButton
          Left = 489
        end
      end
    end
  end
end
