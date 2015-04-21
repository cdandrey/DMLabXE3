object FormNewGraph: TFormNewGraph
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  Caption = #1053#1086#1074#1099#1081' '#1075#1088#1072#1092
  ClientHeight = 282
  ClientWidth = 519
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  ShowHint = True
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object EditName: TLabeledEdit
    AlignWithMargins = True
    Left = 168
    Top = 16
    Width = 344
    Height = 21
    EditLabel.Width = 53
    EditLabel.Height = 13
    EditLabel.Caption = #1048#1084#1103' '#1075#1088#1072#1092#1072
    LabelPosition = lpLeft
    LabelSpacing = 105
    TabOrder = 1
    Text = 'G'
  end
  object EditN: TLabeledEdit
    AlignWithMargins = True
    Left = 278
    Top = 43
    Width = 83
    Height = 21
    EditLabel.Width = 101
    EditLabel.Height = 13
    EditLabel.Caption = #1050#1086#1083#1080#1095#1077#1089#1090#1074#1086' '#1074#1077#1088#1096#1080#1085
    LabelPosition = lpLeft
    LabelSpacing = 10
    NumbersOnly = True
    TabOrder = 2
    Text = '10'
  end
  object EditA: TLabeledEdit
    AlignWithMargins = True
    Left = 452
    Top = 43
    Width = 59
    Height = 21
    EditLabel.Width = 66
    EditLabel.Height = 13
    EditLabel.Caption = #1055#1083#1086#1090#1085#1086#1089#1100', %'
    LabelPosition = lpLeft
    LabelSpacing = 10
    NumbersOnly = True
    TabOrder = 3
    Text = '30'
  end
  object ButtonCancel: TButton
    AlignWithMargins = True
    Left = 437
    Top = 248
    Width = 75
    Height = 25
    Caption = #1054#1090#1084#1077#1085#1072
    ModalResult = 2
    TabOrder = 4
    OnClick = ButtonCancelClick
  end
  object ButtonCreate: TButton
    AlignWithMargins = True
    Left = 356
    Top = 248
    Width = 75
    Height = 25
    Caption = #1057#1086#1079#1076#1072#1090#1100
    ModalResult = 1
    TabOrder = 0
    OnClick = ButtonCreateClick
  end
  object RichEditVertex: TRichEdit
    Left = 168
    Top = 70
    Width = 343
    Height = 172
    Enabled = False
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssBoth
    TabOrder = 5
  end
  object MemoExampl: TMemo
    Left = 6
    Top = 91
    Width = 160
    Height = 130
    BevelOuter = bvSpace
    BorderStyle = bsNone
    Color = clBtnFace
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clGray
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    Lines.Strings = (
      #1060#1086#1088#1084#1072#1090' '#1074#1074#1086#1076#1072' '#1089#1087#1080#1089#1082#1072' '#1074#1077#1088#1096#1080#1085':'
      ''
      '    1 :  2  3  4'
      '    2 :  1  3'
      '    3 :  1  2  4'
      '    4 :  1  3')
    ParentFont = False
    TabOrder = 6
  end
  object RadioButtonRand: TRadioButton
    Left = 8
    Top = 45
    Width = 113
    Height = 17
    Caption = #1057#1083#1091#1095#1072#1081#1085#1099#1081' '#1075#1088#1072#1092
    Checked = True
    TabOrder = 7
    TabStop = True
    OnClick = RadioButtonRandClick
  end
  object RadioButtonHand: TRadioButton
    Left = 8
    Top = 68
    Width = 154
    Height = 17
    Caption = #1057#1086#1079#1076#1072#1090#1100' '#1087#1086' '#1089#1087#1080#1089#1082#1091' '#1074#1077#1088#1096#1080#1085
    TabOrder = 8
    OnClick = RadioButtonHandClick
  end
end
