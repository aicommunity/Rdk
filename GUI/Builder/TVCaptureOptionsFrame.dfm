object VCaptureOptionsFrame: TVCaptureOptionsFrame
  Left = 0
  Top = 0
  Width = 453
  Height = 169
  DoubleBuffered = True
  ParentDoubleBuffered = False
  TabOrder = 0
  object VCapturePageControl: TPageControl
    Left = 0
    Top = 0
    Width = 453
    Height = 169
    ActivePage = DeviceTabSheet
    Align = alClient
    DoubleBuffered = True
    ParentDoubleBuffered = False
    Style = tsFlatButtons
    TabOrder = 0
    OnChange = VCapturePageControlChange
    object DeviceTabSheet: TTabSheet
      Caption = 'Device'
      object VDCapturePanel: TPanel
        Left = 0
        Top = 0
        Width = 445
        Height = 138
        Align = alClient
        TabOrder = 0
        DesignSize = (
          445
          138)
        object ModeLabel: TLabel
          Left = 8
          Top = 89
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Mode:'
          ExplicitWidth = 390
        end
        object InputLabel: TLabel
          Left = 8
          Top = 47
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Crossbar:'
          ExplicitWidth = 390
        end
        object DeviceLabel: TLabel
          Left = 8
          Top = 3
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Video device:'
          ExplicitWidth = 369
        end
        object ModeComboBox: TComboBox
          Left = 8
          Top = 104
          Width = 429
          Height = 21
          Style = csDropDownList
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 2
          OnSelect = ModeComboBoxSelect
        end
        object InputComboBox: TComboBox
          Left = 8
          Top = 62
          Width = 429
          Height = 21
          Style = csDropDownList
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 1
          OnSelect = InputComboBoxSelect
        end
        object DeviceComboBox: TComboBox
          Left = 8
          Top = 19
          Width = 429
          Height = 21
          Style = csDropDownList
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 0
          OnSelect = DeviceComboBoxSelect
        end
      end
    end
    object VideoFileTabSheet: TTabSheet
      Caption = 'Video File'
      ImageIndex = 1
      object VFCapturePanel: TPanel
        Left = 0
        Top = 0
        Width = 445
        Height = 138
        Align = alClient
        TabOrder = 0
        DesignSize = (
          445
          138)
        object VideoFileNameLabel: TLabel
          Left = 8
          Top = 3
          Width = 429
          Height = 13
          Anchors = [akLeft, akTop, akRight]
          AutoSize = False
          Caption = 'Video file name:'
          ExplicitWidth = 369
        end
        object VFNameEdit: TEdit
          Left = 15
          Top = 22
          Width = 337
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 0
        end
        object VFBrowseButton: TButton
          Left = 358
          Top = 20
          Width = 79
          Height = 25
          Anchors = [akTop, akRight]
          Caption = 'Browse'
          TabOrder = 1
          OnClick = VFBrowseButtonClick
        end
        object TimeTrackBar: TTrackBar
          Left = 8
          Top = 51
          Width = 348
          Height = 33
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 2
          OnChange = TimeTrackBarChange
        end
        object TimeEdit: TMaskEdit
          Left = 358
          Top = 51
          Width = 79
          Height = 21
          Anchors = [akTop, akRight]
          EditMask = '000\:00\:00\:00;1;_'
          MaxLength = 12
          TabOrder = 3
          Text = '000:00:00:00'
          OnChange = TimeEditChange
        end
      end
    end
    object PictureFileTabSheet: TTabSheet
      Caption = 'Picture File'
      ImageIndex = 2
    end
  end
  object VFOpenDialog: TOpenDialog
    DefaultExt = 'avi'
    Filter = 'Video (*.avi)|*.avi|All files (*.*)|*.*'
    Left = 296
    Top = 264
  end
end
