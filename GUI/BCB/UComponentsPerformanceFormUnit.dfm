object UComponentsPerformanceForm: TUComponentsPerformanceForm
  Left = 0
  Top = 0
  Caption = 'UComponentsPerformanceForm'
  ClientHeight = 352
  ClientWidth = 558
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  inline UComponentsPerformanceFrame: TUComponentsPerformanceFrame
    Left = 0
    Top = 0
    Width = 558
    Height = 352
    Align = alClient
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    ExplicitWidth = 558
    ExplicitHeight = 352
    inherited Chart: TChart
      Width = 558
      Height = 352
      ExplicitWidth = 558
      ExplicitHeight = 352
      inherited Series1: TBarSeries
        Data = {
          00060000000000000000C87E400000000000187A400000000000F07940000000
          0000A0794000000000000074400000000000807140}
      end
    end
  end
end
