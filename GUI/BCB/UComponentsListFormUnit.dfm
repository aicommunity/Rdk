object UComponentsListForm: TUComponentsListForm
  Left = 857
  Top = 64
  Caption = 'Components List'
  ClientHeight = 420
  ClientWidth = 565
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  inline ComponentsListFrame1: TUComponentsListFrame
    Left = 0
    Top = 0
    Width = 565
    Height = 379
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 565
    ExplicitHeight = 379
    inherited Splitter1: TSplitter
      Height = 379
      ExplicitHeight = 379
    end
    inherited Panel1: TPanel
      Width = 196
      Height = 379
      ExplicitWidth = 196
      ExplicitHeight = 379
      inherited PageControl1: TPageControl
        Width = 194
        Height = 377
        ExplicitWidth = 194
        ExplicitHeight = 377
        inherited TabSheet1: TTabSheet
          ExplicitWidth = 186
          ExplicitHeight = 351
          inherited ParametersHeaderControl: THeaderControl
            Width = 186
            ExplicitWidth = 186
          end
          inherited ParametersRichEdit: TRichEdit
            Width = 186
            Height = 317
            ExplicitWidth = 186
            ExplicitHeight = 317
          end
        end
        inherited TabSheet2: TTabSheet
          ExplicitWidth = 186
          ExplicitHeight = 351
          inherited StateHeaderControl: THeaderControl
            ExplicitWidth = 186
          end
          inherited StateRichEdit: TRichEdit
            ExplicitWidth = 186
            ExplicitHeight = 317
          end
        end
        inherited TabSheet3: TTabSheet
          ExplicitWidth = 186
          ExplicitHeight = 351
          inherited Splitter2: TSplitter
            Width = 186
            ExplicitWidth = 186
          end
          inherited IOHeaderControl: THeaderControl
            Width = 186
            ExplicitWidth = 186
          end
          inherited GroupBox: TGroupBox
            Width = 186
            ExplicitWidth = 186
            inherited OutputsStringGrid: TStringGrid
              Width = 182
              ExplicitWidth = 182
            end
          end
          inherited GroupBox2: TGroupBox
            Width = 186
            Height = 70
            ExplicitWidth = 186
            ExplicitHeight = 70
            inherited InputsStringGrid: TStringGrid
              Width = 182
              Height = 53
              ExplicitWidth = 182
              ExplicitHeight = 53
            end
          end
        end
        inherited TabSheet4: TTabSheet
          ExplicitWidth = 186
          ExplicitHeight = 351
          inherited HeaderControl1: THeaderControl
            Width = 186
            ExplicitWidth = 186
          end
          inherited ParametersListStringGrid: TStringGrid
            Width = 186
            Height = 321
            OnDblClick = ComponentsListFrame1ParametersListStringGridDblClick
            ExplicitLeft = 0
            ExplicitTop = 30
            ExplicitWidth = 186
            ExplicitHeight = 321
          end
        end
        inherited TabSheet5: TTabSheet
          ExplicitWidth = 186
          ExplicitHeight = 351
          inherited HeaderControl2: THeaderControl
            Width = 186
            ExplicitWidth = 186
          end
        end
      end
    end
    inherited Panel2: TPanel
      Height = 379
      ExplicitHeight = 379
      inherited StringGrid: TStringGrid
        Height = 343
        OnDblClick = ComponentsListFrame1StringGridDblClick
        ExplicitHeight = 343
      end
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 379
    Width = 565
    Height = 41
    Align = alBottom
    TabOrder = 1
    DesignSize = (
      565
      41)
    object OkButton: TButton
      Left = 397
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      Default = True
      ModalResult = 1
      TabOrder = 0
    end
    object CancelButton: TButton
      Left = 480
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
  end
end
