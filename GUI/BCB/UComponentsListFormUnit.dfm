object UComponentsListForm: TUComponentsListForm
  Left = 857
  Top = 64
  Caption = 'Components List'
  ClientHeight = 420
  ClientWidth = 682
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
    Width = 682
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
      Width = 313
      Height = 379
      ExplicitWidth = 196
      ExplicitHeight = 379
      inherited PageControl1: TPageControl
        Width = 311
        Height = 377
        ExplicitWidth = 194
        ExplicitHeight = 377
        inherited TabSheet4: TTabSheet
          inherited Splitter3: TSplitter
            Top = 274
            Width = 303
          end
          inherited ParametersListStringGrid: TStringGrid
            Width = 303
            Height = 274
            OnDblClick = ComponentsListFrame1ParametersListStringGridDblClick
          end
          inherited ParameterValueRichEdit: TRichEdit
            Top = 284
            Width = 303
          end
          inherited HeaderControl3: THeaderControl
            Top = 317
            Width = 303
          end
        end
        inherited TabSheet5: TTabSheet
          inherited StatesListStringGrid: TStringGrid
            OnDblClick = ComponentsListFrame1StatesListStringGridDblClick
            ExplicitWidth = 303
            ExplicitHeight = 321
          end
        end
        inherited TabSheet3: TTabSheet
          inherited Splitter2: TSplitter
            Top = 201
            Width = 303
            ExplicitTop = 201
            ExplicitWidth = 186
          end
          inherited IOHeaderControl: THeaderControl
            Width = 303
            ExplicitWidth = 303
          end
          inherited GroupBox: TGroupBox
            Width = 303
            Height = 167
            ExplicitWidth = 303
            ExplicitHeight = 167
            inherited OutputsStringGrid: TStringGrid
              Width = 299
              Height = 150
              OnDblClick = ComponentsListFrame1OutputsStringGridDblClick
              ExplicitWidth = 299
              ExplicitHeight = 150
            end
          end
          inherited GroupBox2: TGroupBox
            Top = 209
            Width = 303
            Height = 142
            ExplicitTop = 209
            ExplicitWidth = 303
            ExplicitHeight = 142
            inherited InputsStringGrid: TStringGrid
              Width = 299
              Height = 125
              OnDblClick = ComponentsListFrame1InputsStringGridDblClick
              ExplicitWidth = 299
              ExplicitHeight = 125
            end
          end
        end
        inherited TabSheet1: TTabSheet
          ExplicitWidth = 186
          ExplicitHeight = 351
          inherited ParametersHeaderControl: THeaderControl
            ExplicitWidth = 186
          end
          inherited ParametersRichEdit: TRichEdit
            ExplicitWidth = 186
            ExplicitHeight = 317
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
    Width = 682
    Height = 41
    Align = alBottom
    TabOrder = 1
    ExplicitWidth = 565
    DesignSize = (
      682
      41)
    object OkButton: TButton
      Left = 514
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'OK'
      Default = True
      ModalResult = 1
      TabOrder = 0
      ExplicitLeft = 397
    end
    object CancelButton: TButton
      Left = 597
      Top = 8
      Width = 75
      Height = 25
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
      ExplicitLeft = 480
    end
  end
end
