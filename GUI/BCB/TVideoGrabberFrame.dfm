object VideoGrabberFrame: TVideoGrabberFrame
  Left = 0
  Top = 0
  Width = 382
  Height = 259
  TabOrder = 0
  object Image1: TImage
    Left = 0
    Top = 0
    Width = 382
    Height = 259
    Align = alClient
    ExplicitLeft = 88
    ExplicitTop = 40
    ExplicitWidth = 105
    ExplicitHeight = 105
  end
  object VideoGrabber1: TVideoGrabber
    Left = 88
    Top = 48
    Width = 57
    Height = 33
    Align = alCustom
    Caption = 'VideoGrabber1'
    Color = clBlack
    Visible = False
    AspectRatioToUse = -1.000000000000000000
    AudioCompressor = 0
    AutoFileNameDateTimeFormat = 'yymmdd_hhmmss_zzz'
    AutoFilePrefix = 'vg'
    Cropping_Zoom = 1.000000000000000000
    ImageOverlayEnabled = False
    ImageOverlay_AlphaBlend = False
    ImageOverlay_AlphaBlendValue = 180
    ImageOverlay_ChromaKey = False
    ImageOverlay_ChromaKeyLeewayPercent = 25
    ImageOverlay_ChromaKeyRGBColor = 0
    ImageOverlay_Height = -1
    ImageOverlay_LeftLocation = 10
    ImageOverlay_VideoAlignment = oa_LeftTop
    ImageOverlay_StretchToVideoSize = False
    ImageOverlay_TopLocation = 10
    ImageOverlay_Transparent = False
    ImageOverlay_TransparentColorValue = 0
    ImageOverlay_UseTransparentColor = False
    ImageOverlay_Width = -1
    ImageOverlay_TargetDisplay = -1
    LicenseString = 'N/A'
    MotionDetector_Grid = 
      '5555555555 5555555555 5555555555 5555555555 5555555555 555555555' +
      '5 5555555555 5555555555 5555555555 5555555555'
    PlayerSpeedRatio = 1.000000000000000000
    TextOverlay_Enabled = False
    TextOverlay_Font.Charset = DEFAULT_CHARSET
    TextOverlay_Font.Color = clAqua
    TextOverlay_Font.Height = -16
    TextOverlay_Font.Name = 'Arial'
    TextOverlay_Font.Style = []
    TextOverlay_HighResFont = True
    TextOverlay_Left = 0
    TextOverlay_Top = 0
    TextOverlay_Right = -1
    TextOverlay_Scrolling = False
    TextOverlay_ScrollingSpeed = 1
    TextOverlay_Shadow = True
    TextOverlay_ShadowColor = clBlack
    TextOverlay_ShadowDirection = cd_Center
    TextOverlay_BkColor = clWhite
    TextOverlay_Align = tf_Left
    TextOverlay_GradientMode = gm_Disabled
    TextOverlay_GradientColor = clNavy
    TextOverlay_VideoAlignment = oa_LeftTop
    TextOverlay_String = 
      'Note: the date/time formats '#13#10'can be easily modified.'#13#10#13#10'system ' +
      'date/time: %sys_time[dd/mm/yy hh:nn:ss]%'#13#10'DV time code: %time_co' +
      'de%'#13#10'DV date/time: %dv_time[dd/mm/yy hh:nn:ss]%'#13#10'frame number: %' +
      'frame_count%'#13#10'time (full): %time_full%'#13#10'time (sec): %time_sec%'#13#10 +
      'time (ns): %time_100ns%'
    TextOverlay_TargetDisplay = -1
    TextOverlay_Transparent = True
    VideoCompression_Quality = 1.000000000000000000
    VideoCompressor = 0
    VideoDelay = 0
    VideoFromImages_TemporaryFile = 'SetOfBitmaps01.dat'
    VideoProcessing_RotationCustomAngle = 45.500000000000000000
    OnDeviceLost = VideoGrabber1DeviceLost
    OnFrameBitmap = VideoGrabber1FrameBitmap
    OnFrameCaptureCompleted = VideoGrabber1FrameCaptureCompleted
    OnLog = VideoGrabber1Log
    OnMouseDown = VideoGrabber1MouseDown
    OnMouseMove = VideoGrabber1MouseMove
    OnMouseUp = VideoGrabber1MouseUp
    OnPlayerEndOfStream = VideoGrabber1PlayerEndOfStream
    OnPlayerOpened = VideoGrabber1PlayerOpened
    OnPreviewStarted = VideoGrabber1PreviewStarted
  end
end
