object FormMain: TFormMain
  Left = 163
  Top = 99
  Width = 1088
  Height = 734
  Caption = 'Graph from Serial port data'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Splitter2: TSplitter
    Left = 521
    Top = 153
    Width = 2
    Height = 543
    Color = clActiveCaption
    ParentColor = False
    OnMoved = UpdatePesets
  end
  object Chart1: TChart
    Left = 0
    Top = 153
    Width = 521
    Height = 543
    AllowPanning = pmNone
    BackWall.Brush.Color = clWhite
    BackWall.Color = clBlack
    Title.AdjustFrame = False
    Title.Text.Strings = (
      '')
    Title.Visible = False
    BackColor = clBlack
    View3D = False
    Align = alLeft
    BevelOuter = bvNone
    TabOrder = 0
    object Series1: TLineSeries
      Marks.ArrowLength = 8
      Marks.Visible = False
      SeriesColor = clRed
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = False
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1.000000000000000000
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1.000000000000000000
      YValues.Order = loNone
    end
    object Series2: TLineSeries
      Marks.ArrowLength = 8
      Marks.Visible = False
      SeriesColor = clGreen
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = False
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1.000000000000000000
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1.000000000000000000
      YValues.Order = loNone
    end
    object Series3: TLineSeries
      Marks.ArrowLength = 8
      Marks.Visible = False
      SeriesColor = 4227327
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = False
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1.000000000000000000
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1.000000000000000000
      YValues.Order = loNone
    end
    object Series4: TLineSeries
      Marks.ArrowLength = 8
      Marks.Visible = False
      SeriesColor = clBlue
      Pointer.InflateMargins = True
      Pointer.Style = psRectangle
      Pointer.Visible = False
      XValues.DateTime = False
      XValues.Name = 'X'
      XValues.Multiplier = 1.000000000000000000
      XValues.Order = loAscending
      YValues.DateTime = False
      YValues.Name = 'Y'
      YValues.Multiplier = 1.000000000000000000
      YValues.Order = loNone
    end
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 1072
    Height = 153
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object Splitter1: TSplitter
      Left = 609
      Top = 0
      Width = 2
      Height = 153
      Color = clActiveCaption
      ParentColor = False
      OnMoved = UpdatePesets
    end
    object Panel2: TPanel
      Left = 0
      Top = 0
      Width = 270
      Height = 153
      Align = alLeft
      BevelOuter = bvNone
      TabOrder = 0
      object Label1: TLabel
        Left = 6
        Top = 12
        Width = 19
        Height = 13
        Caption = 'Port'
      end
      object Label2: TLabel
        Left = 128
        Top = 12
        Width = 31
        Height = 13
        Caption = 'Speed'
      end
      object Label5: TLabel
        Left = 6
        Top = 35
        Width = 30
        Height = 13
        Caption = 'Preset'
      end
      object ComboBox1: TComboBox
        Left = 40
        Top = 8
        Width = 81
        Height = 21
        ItemHeight = 13
        ItemIndex = 25
        TabOrder = 0
        Text = 'COM26'
        OnSelect = UpdatePesets
        Items.Strings = (
          'COM1'
          'COM2'
          'COM3'
          'COM4'
          'COM5'
          'COM6'
          'COM7'
          'COM8'
          'COM9'
          'COM10'
          'COM11'
          'COM12'
          'COM13'
          'COM14'
          'COM15'
          'COM16'
          'COM17'
          'COM18'
          'COM19'
          'COM20'
          'COM21'
          'COM22'
          'COM23'
          'COM24'
          'COM25'
          'COM26'
          'COM27'
          'COM28'
          'COM29'
          'COM30')
      end
      object ComboBox2: TComboBox
        Left = 168
        Top = 8
        Width = 65
        Height = 21
        ItemHeight = 13
        ItemIndex = 3
        TabOrder = 1
        Text = '9600'
        OnSelect = UpdatePesets
        Items.Strings = (
          '1200'
          '2400'
          '4800'
          '9600'
          '14400'
          '19200'
          '28800'
          '38400'
          '57600'
          '115200'
          '230400')
      end
      object Button2: TButton
        Left = 8
        Top = 59
        Width = 57
        Height = 25
        Caption = 'Clear'
        TabOrder = 2
        OnClick = Button2Click
      end
      object Button1: TButton
        Left = 160
        Top = 59
        Width = 105
        Height = 25
        Caption = 'Start'
        TabOrder = 3
        OnClick = Button1Click
      end
      object Button3: TButton
        Left = 8
        Top = 91
        Width = 82
        Height = 25
        Caption = 'Copy Graph'
        TabOrder = 4
        OnClick = Button3Click
      end
      object Button4: TButton
        Left = 184
        Top = 91
        Width = 82
        Height = 25
        Caption = 'Copy Log'
        TabOrder = 5
        OnClick = Button4Click
      end
      object Button5: TButton
        Left = 237
        Top = 5
        Width = 30
        Height = 25
        Caption = '...'
        TabOrder = 6
        OnClick = Button5Click
      end
      object ComboBox3: TComboBox
        Left = 40
        Top = 32
        Width = 225
        Height = 21
        ItemHeight = 13
        TabOrder = 7
        OnChange = ComboBox3Change
        OnExit = UpdatePesets
        OnSelect = ComboBox3Select
      end
      object CheckBox1: TCheckBox
        Left = 184
        Top = 128
        Width = 89
        Height = 17
        Caption = 'Autoscroll log'
        Checked = True
        State = cbChecked
        TabOrder = 8
        OnClick = UpdatePesets
      end
      object CheckBox2: TCheckBox
        Left = 5
        Top = 128
        Width = 124
        Height = 17
        Caption = 'Max points in chart'
        Checked = True
        State = cbChecked
        TabOrder = 9
        OnClick = UpdatePesets
      end
      object SpinEdit1: TSpinEdit
        Left = 117
        Top = 126
        Width = 57
        Height = 22
        MaxValue = 0
        MinValue = 0
        TabOrder = 10
        Value = 300
        OnChange = UpdatePesets
      end
      object Button6: TButton
        Left = 96
        Top = 91
        Width = 82
        Height = 25
        Caption = 'Copy Form'
        TabOrder = 11
        OnClick = Button6Click
      end
    end
    object Panel3: TPanel
      Left = 270
      Top = 0
      Width = 339
      Height = 153
      Align = alLeft
      BevelOuter = bvNone
      BorderWidth = 4
      TabOrder = 1
      object Label3: TLabel
        Left = 4
        Top = 4
        Width = 331
        Height = 18
        Align = alTop
        AutoSize = False
        Caption = 'Chanals'
        Layout = tlCenter
      end
      object Memo1: TMemo
        Left = 4
        Top = 22
        Width = 331
        Height = 127
        Align = alClient
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Consolas'
        Font.Style = []
        Lines.Strings = (
          '$1 - Need speed'
          '$2 - Current speed'
          '$3 - Out speed'
          '$4 - Out value')
        ParentFont = False
        ScrollBars = ssBoth
        TabOrder = 0
        WordWrap = False
        OnChange = UpdatePesets
      end
    end
    object Panel4: TPanel
      Left = 611
      Top = 0
      Width = 461
      Height = 153
      Align = alClient
      BevelOuter = bvNone
      BorderWidth = 4
      TabOrder = 2
      object Label4: TLabel
        Left = 4
        Top = 4
        Width = 453
        Height = 18
        Align = alTop
        AutoSize = False
        Caption = 'RegExpr'
        Layout = tlCenter
      end
      object Memo2: TMemo
        Left = 4
        Top = 22
        Width = 453
        Height = 127
        Align = alClient
        Font.Charset = RUSSIAN_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Consolas'
        Font.Style = []
        Lines.Strings = (
          
            'Need speed\: (.+?), Current speed\: (.+?), Out speed\: (.+?), Ou' +
            't value\: (.+)')
        ParentFont = False
        ScrollBars = ssHorizontal
        TabOrder = 0
        WordWrap = False
        OnChange = UpdatePesets
      end
    end
  end
  object Memo3: TMemo
    Left = 523
    Top = 153
    Width = 549
    Height = 543
    Align = alClient
    BevelKind = bkFlat
    BorderStyle = bsNone
    Font.Charset = RUSSIAN_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Lucida Console'
    Font.Style = []
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 2
    WordWrap = False
  end
  object ComPort: TComPort
    BaudRate = brCustom
    Port = 'COM1'
    Parity.Bits = prNone
    StopBits = sbOneStopBit
    DataBits = dbEight
    Events = [evRxChar, evTxEmpty, evRxFlag, evRing, evBreak, evCTS, evDSR, evError, evRLSD, evRx80Full]
    FlowControl.OutCTSFlow = False
    FlowControl.OutDSRFlow = False
    FlowControl.ControlDTR = dtrDisable
    FlowControl.ControlRTS = rtsDisable
    FlowControl.XonXoffOut = False
    FlowControl.XonXoffIn = False
    StoredProps = [spBasic]
    TriggersOnRxChar = True
    OnRxChar = ComPortRxChar
    Left = 48
    Top = 160
  end
  object XPManifest: TXPManifest
    Left = 16
    Top = 161
  end
end
