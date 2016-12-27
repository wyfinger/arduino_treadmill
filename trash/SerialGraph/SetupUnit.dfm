object Form3: TForm3
  Left = 379
  Top = 124
  BiDiMode = bdRightToLeft
  BorderStyle = bsDialog
  Caption = 'Serial Port Setup'
  ClientHeight = 320
  ClientWidth = 426
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  ParentBiDiMode = False
  Position = poMainFormCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 16
    Top = 16
    Width = 43
    Height = 13
    Caption = 'Data Bits'
  end
  object Label2: TLabel
    Left = 16
    Top = 40
    Width = 43
    Height = 13
    Caption = 'Data Bits'
  end
  object ComboBox1: TComboBox
    Left = 68
    Top = 12
    Width = 147
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 0
    Items.Strings = (
      '5'
      '6'
      '7'
      '8')
  end
  object CheckBox1: TCheckBox
    Left = 72
    Top = 40
    Width = 97
    Height = 17
    Caption = 'Diskard Null'
    TabOrder = 1
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
    Left = 16
    Top = 144
  end
end
