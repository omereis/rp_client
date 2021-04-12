object frmMain: TfrmMain
  Left = 0
  Top = 0
  Caption = 'Red Pitaya PSD, MCA'
  ClientHeight = 299
  ClientWidth = 635
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 56
    Top = 19
    Width = 22
    Height = 13
    Caption = 'Host'
  end
  object Label2: TLabel
    Left = 58
    Top = 46
    Width = 20
    Height = 13
    Caption = 'Port'
  end
  object edtHost: TEdit
    Left = 96
    Top = 16
    Width = 121
    Height = 21
    TabOrder = 0
    Text = 'edtHost'
  end
  object edtPort: TEdit
    Left = 96
    Top = 43
    Width = 121
    Height = 21
    TabOrder = 1
    Text = 'edtPort'
  end
  object btnConenct: TButton
    Left = 232
    Top = 14
    Width = 75
    Height = 25
    Caption = 'Conenct'
    TabOrder = 2
    OnClick = btnConenctClick
  end
  object btnDisconnect: TButton
    Left = 232
    Top = 45
    Width = 75
    Height = 25
    Caption = 'Disconnect'
    TabOrder = 3
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 280
    Width = 635
    Height = 19
    Panels = <>
  end
  object Button1: TButton
    Left = 328
    Top = 120
    Width = 75
    Height = 25
    Caption = 'Load'
    TabOrder = 5
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 104
    Top = 104
    Width = 75
    Height = 25
    Caption = 'Save'
    TabOrder = 6
    OnClick = Button2Click
  end
  object Panel1: TPanel
    Left = 80
    Top = 184
    Width = 185
    Height = 41
    Caption = 'Panel1'
    TabOrder = 7
  end
  object socket: TSocketConnection
    ServerName = 'DESKTOP-TDH2GBF'
    Host = 'localhost'
    Port = 5500
    SupportCallbacks = False
    Left = 264
    Top = 104
  end
  object ApplicationEvents1: TApplicationEvents
    OnIdle = ApplicationEvents1Idle
    Left = 376
    Top = 40
  end
end
