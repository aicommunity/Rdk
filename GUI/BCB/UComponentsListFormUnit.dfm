object UComponentsListForm: TUComponentsListForm
  Left = 857
  Top = 64
  Caption = 'Components List'
  ClientHeight = 439
  ClientWidth = 714
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
  object Panel1: TPanel
    Left = 0
    Top = 397
    Width = 714
    Height = 42
    Align = alBottom
    TabOrder = 0
    DesignSize = (
      714
      42)
    object OkButton: TButton
      Left = 538
      Top = 8
      Width = 78
      Height = 26
      Anchors = [akTop, akRight]
      Caption = 'OK'
      Default = True
      ModalResult = 1
      TabOrder = 0
    end
    object CancelButton: TButton
      Left = 625
      Top = 8
      Width = 78
      Height = 26
      Anchors = [akTop, akRight]
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 1
    end
  end
  inline ComponentsListFrame1: TUComponentsListFrame
    Left = 0
    Top = 0
    Width = 714
    Height = 397
    Align = alClient
    DoubleBuffered = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 1
    ExplicitWidth = 714
    ExplicitHeight = 397
    inherited Splitter1: TSplitter
      Left = 378
      Height = 397
      ExplicitLeft = 378
      ExplicitHeight = 397
    end
    inherited Panel1: TPanel
      Left = 386
      Width = 328
      Height = 397
      ExplicitLeft = 386
      ExplicitWidth = 328
      ExplicitHeight = 397
      inherited PageControl1: TPageControl
        Width = 326
        Height = 395
        ExplicitWidth = 326
        ExplicitHeight = 395
        inherited TabSheet6: TTabSheet
          inherited Splitter5: TSplitter
            Top = 288
            Width = 318
            ExplicitTop = 290
            ExplicitWidth = 319
          end
          inherited EnchancedSG1: TEnchancedSG
            Width = 318
            Height = 288
            ExplicitWidth = 319
            ExplicitHeight = 290
            inherited BasicStringGrid: TStringGrid
              Left = 17
              Top = 58
              Width = 168
              Height = 102
              ExplicitLeft = 17
              ExplicitTop = 58
              ExplicitWidth = 168
              ExplicitHeight = 102
              ColWidths = (
                64
                64)
              RowHeights = (
                18
                18)
            end
            inherited txtStringEdit: TEdit
              Left = 34
              Top = 259
              Width = 126
              Height = 17
              ExplicitLeft = 34
              ExplicitTop = 259
              ExplicitWidth = 126
              ExplicitHeight = 17
            end
            inherited chbBoolEdit: TCheckBox
              Left = 37
              Top = 383
              Width = 101
              Height = 18
              ExplicitLeft = 37
              ExplicitTop = 383
              ExplicitWidth = 101
              ExplicitHeight = 18
            end
            inherited udBorderedInt: TUpDown
              Left = 159
              Top = 339
              Width = 18
              Height = 22
              ExplicitLeft = 159
              ExplicitTop = 339
              ExplicitWidth = 18
              ExplicitHeight = 22
            end
            inherited txtDoubleEdit: TEdit
              Left = 31
              Top = 218
              Width = 127
              Height = 16
              ExplicitLeft = 31
              ExplicitTop = 218
              ExplicitWidth = 127
              ExplicitHeight = 16
            end
            inherited txtIntEdit: TEdit
              Left = 34
              Top = 407
              Width = 126
              Height = 17
              ExplicitLeft = 34
              ExplicitTop = 407
              ExplicitWidth = 126
              ExplicitHeight = 17
            end
            inherited txtBorderedIntEdit: TEdit
              Left = 34
              Top = 339
              Width = 126
              Height = 17
              ExplicitLeft = 34
              ExplicitTop = 339
              ExplicitWidth = 126
              ExplicitHeight = 17
            end
            inherited cmbListEdit: TComboBox
              Left = 34
              Top = 300
              Width = 151
              Height = 20
              Font.Height = -10
              ExplicitLeft = 34
              ExplicitTop = 300
              ExplicitWidth = 151
              ExplicitHeight = 20
            end
          end
          inherited HeaderControl2: THeaderControl
            Top = 333
            Width = 318
            Height = 35
            ExplicitTop = 335
            ExplicitWidth = 319
            ExplicitHeight = 35
          end
          inherited NiceParamValRichEdit: TRichEdit
            Top = 298
            Width = 318
            Height = 35
            Font.Height = -12
            Constraints.MinHeight = 20
            ExplicitTop = 300
            ExplicitWidth = 319
            ExplicitHeight = 35
          end
        end
        inherited TabSheet7: TTabSheet
          inherited Splitter6: TSplitter
            Top = 458
            Height = 11
            ExplicitTop = 350
            ExplicitWidth = 301
            ExplicitHeight = 11
          end
          inherited HeaderControl4: THeaderControl
            Top = 503
            Height = 36
            ExplicitTop = 395
            ExplicitWidth = 301
            ExplicitHeight = 36
          end
          inherited NiceStateValRichEdit: TRichEdit
            Top = 469
            Height = 34
            Font.Height = -12
            Constraints.MinHeight = 20
            ExplicitTop = 361
            ExplicitWidth = 301
            ExplicitHeight = 34
          end
          inherited EnchancedSG2: TEnchancedSG
            Height = 458
            ExplicitWidth = 301
            ExplicitHeight = 350
            inherited BasicStringGrid: TStringGrid
              Left = 17
              Top = 58
              Width = 168
              Height = 102
              ExplicitLeft = 17
              ExplicitTop = 58
              ExplicitWidth = 168
              ExplicitHeight = 102
              ColWidths = (
                64
                64)
            end
            inherited txtStringEdit: TEdit
              Left = 34
              Top = 259
              Width = 126
              Height = 17
              ExplicitLeft = 34
              ExplicitTop = 259
              ExplicitWidth = 126
              ExplicitHeight = 17
            end
            inherited chbBoolEdit: TCheckBox
              Left = 37
              Top = 383
              Width = 101
              Height = 18
              ExplicitLeft = 37
              ExplicitTop = 383
              ExplicitWidth = 101
              ExplicitHeight = 18
            end
            inherited udBorderedInt: TUpDown
              Left = 159
              Top = 339
              Width = 18
              Height = 22
              ExplicitLeft = 159
              ExplicitTop = 339
              ExplicitWidth = 18
              ExplicitHeight = 22
            end
            inherited txtDoubleEdit: TEdit
              Left = 31
              Top = 218
              Width = 127
              Height = 16
              ExplicitLeft = 31
              ExplicitTop = 218
              ExplicitWidth = 127
              ExplicitHeight = 16
            end
            inherited txtIntEdit: TEdit
              Left = 34
              Top = 407
              Width = 126
              Height = 17
              ExplicitLeft = 34
              ExplicitTop = 407
              ExplicitWidth = 126
              ExplicitHeight = 17
            end
            inherited txtBorderedIntEdit: TEdit
              Left = 34
              Top = 339
              Width = 126
              Height = 17
              ExplicitLeft = 34
              ExplicitTop = 339
              ExplicitWidth = 126
              ExplicitHeight = 17
            end
            inherited cmbListEdit: TComboBox
              Left = 34
              Top = 300
              Width = 151
              Height = 20
              Font.Height = -10
              ExplicitLeft = 34
              ExplicitTop = 300
              ExplicitWidth = 151
              ExplicitHeight = 20
            end
          end
        end
        inherited TabSheet3: TTabSheet
          inherited Splitter2: TSplitter
            Top = 250
            Width = 394
            ExplicitTop = 250
            ExplicitWidth = 394
          end
          inherited GroupBox: TGroupBox
            Width = 394
            Height = 250
            ExplicitWidth = 394
            ExplicitHeight = 250
            inherited OutputsStringGrid: TStringGrid
              Top = 16
              Width = 389
              Height = 232
              ExplicitTop = 16
              ExplicitWidth = 389
              ExplicitHeight = 232
              ColWidths = (
                64
                64
                64
                64
                64)
              RowHeights = (
                18
                18
                18
                18
                18)
            end
          end
          inherited GroupBox2: TGroupBox
            Top = 258
            Width = 394
            Height = 306
            ExplicitTop = 258
            ExplicitWidth = 394
            ExplicitHeight = 306
            inherited InputsStringGrid: TStringGrid
              Top = 16
              Width = 389
              Height = 287
              ExplicitTop = 16
              ExplicitWidth = 389
              ExplicitHeight = 287
              ColWidths = (
                64
                64
                64
                64
                64)
              RowHeights = (
                18
                18
                18
                18
                18)
            end
          end
        end
        inherited TabSheet1: TTabSheet
          inherited ParametersHeaderControl: THeaderControl
            Top = 504
            Height = 35
            ExplicitTop = 396
            ExplicitWidth = 301
            ExplicitHeight = 35
          end
          inherited ParametersRichEdit: TRichEdit
            Height = 504
            Font.Height = -12
            ExplicitWidth = 301
            ExplicitHeight = 396
          end
        end
        inherited TabSheet2: TTabSheet
          inherited StateHeaderControl: THeaderControl
            Top = 504
            Height = 35
            ExplicitTop = 396
            ExplicitWidth = 301
            ExplicitHeight = 35
          end
          inherited StateRichEdit: TRichEdit
            Height = 504
            Font.Height = -12
            ExplicitWidth = 301
            ExplicitHeight = 396
          end
        end
      end
    end
    inherited Panel2: TPanel
      Width = 378
      Height = 397
      ExplicitWidth = 378
      ExplicitHeight = 397
      inherited StringGrid: TStringGrid
        Top = 37
        Width = 376
        Height = 336
        ExplicitTop = 37
        ExplicitWidth = 376
        ExplicitHeight = 336
        ColWidths = (
          64
          64
          64
          64
          64)
        RowHeights = (
          24
          24
          24
          24
          24)
      end
      inherited HeaderControl: THeaderControl
        Width = 376
        Height = 36
        ExplicitWidth = 376
        ExplicitHeight = 36
      end
      inherited ClassNamePanel: TPanel
        Top = 373
        Width = 376
        ExplicitTop = 373
        ExplicitWidth = 376
      end
    end
  end
end
