unit UnitMain;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, TeEngine, Series, ExtCtrls, TeeProcs, Chart, CPDrv, StdCtrls,
  RegExpr, CalcExpress, IniFiles, ClipBrd, CPort, Menus, Spin, XPMan;

type
  TFormMain = class(TForm)
    Chart1: TChart;
    Series1: TLineSeries;
    Series2: TLineSeries;
    Panel1: TPanel;
    ComboBox1: TComboBox;
    Label1: TLabel;
    ComboBox2: TComboBox;
    Label2: TLabel;
    Memo1: TMemo;
    Label3: TLabel;
    Memo2: TMemo;
    Button1: TButton;
    Label4: TLabel;
    Memo3: TMemo;
    Series3: TLineSeries;
    Series4: TLineSeries;
    Button2: TButton;
    Panel2: TPanel;
    Button3: TButton;
    Button4: TButton;
    Panel3: TPanel;
    Panel4: TPanel;
    Splitter1: TSplitter;
    Splitter2: TSplitter;
    ComPort: TComPort;
    Button5: TButton;
    Label5: TLabel;
    ComboBox3: TComboBox;
    XPManifest: TXPManifest;
    CheckBox1: TCheckBox;
    CheckBox2: TCheckBox;
    SpinEdit1: TSpinEdit;
    Button6: TButton;
    procedure Button1Click(Sender: TObject);
    procedure FormCreate(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
    procedure Button4Click(Sender: TObject);
    procedure ComPortRxChar(Sender: TObject; Count: Integer);
    procedure Button5Click(Sender: TObject);
    procedure ComboBox3Change(Sender: TObject);
    procedure UpdatePesets(Sender: TObject);
    procedure ComboBox3Select(Sender: TObject);
    procedure Button6Click(Sender: TObject);
  private
    { Private declarations }
    LoadSettingsFlag : Boolean;
    procedure SaveSettings(FileName: string);
    procedure LoadSettings(FileName: string);
    procedure ScanPesets;
    procedure ProcessLine(Line: string);
  public
    { Public declarations }
  end;

var
  FormMain  : TFormMain;
  buff      : string;
  calc      : TCalcExpress;

  s    : TStringStream;
  l    : TStringList;
  r    : TRegExpr;
  pl   : LongInt;  // номер последней выведенной строки данных
  ch   : TStringList;

implementation

{$R *.dfm}

procedure TFormMain.SaveSettings(FileName: string);
var
  IniFile : TIniFile;
  i       : Integer;
begin
 if LoadSettingsFlag then Exit;
 IniFile := TIniFile.Create(ExtractFilePath(Application.ExeName)+FileName);

 with ComPort do
   begin
     IniFile.WriteString('Serial', 'Port', Port);
     IniFile.WriteString('Serial', 'BaudRate', BaudRateToStr(BaudRate));
     if BaudRate = brCustom then
       IniFile.WriteInteger('Serial', 'CustomBaudRate', CustomBaudRate);
     IniFile.WriteString('Serial', 'StopBits', StopBitsToStr(StopBits));
     IniFile.WriteString('Serial', 'DataBits', DataBitsToStr(DataBits));
     IniFile.WriteString('Serial', 'Parity', ParityToStr(Parity.Bits));
     IniFile.WriteString('Serial', 'FlowControl', FlowControlToStr(FlowControl.FlowControl));
    
     IniFile.WriteString('Serial', 'EventChar', EventChar);
     IniFile.WriteString('Serial', 'DiscardNull', BoolToStr(DiscardNull));
    
     IniFile.WriteString('Serial', 'Parity.Check', BoolToStr(Parity.Check));
     IniFile.WriteString('Serial', 'Parity.Replace', BoolToStr(Parity.Replace));
     IniFile.WriteString('Serial', 'Parity.ReplaceChar', Parity.ReplaceChar);
    
     IniFile.WriteString('Serial', 'Parity.Check', BoolToStr(Parity.Check));
     IniFile.WriteString('Serial', 'Parity.Replace', BoolToStr(Parity.Replace));
     IniFile.WriteString('Serial', 'Parity.ReplaceChar', Parity.ReplaceChar);
    
     IniFile.WriteInteger('Serial', 'Buffer.OutputSize', Buffer.OutputSize);
     IniFile.WriteInteger('Serial', 'Buffer.InputSize', Buffer.InputSize);
    
     IniFile.WriteInteger('Serial', 'Timeouts.ReadInterval', Timeouts.ReadInterval);
     IniFile.WriteInteger('Serial', 'Timeouts.ReadTotalConstant', Timeouts.ReadTotalConstant);
     IniFile.WriteInteger('Serial', 'Timeouts.ReadTotalMultiplier', Timeouts.ReadTotalMultiplier);
     IniFile.WriteInteger('Serial', 'Timeouts.WriteTotalConstant', Timeouts.WriteTotalConstant);
     IniFile.WriteInteger('Serial', 'Timeouts.WriteTotalMultiplier', Timeouts.WriteTotalMultiplier);

     IniFile.WriteString('Serial', 'FlowControl.ControlRTS', RTSToStr(FlowControl.ControlRTS));
     IniFile.WriteString('Serial', 'FlowControl.ControlDTR', DTRToStr(FlowControl.ControlDTR));
     IniFile.WriteString('Serial', 'FlowControl.DSRSensitivity', BoolToStr(FlowControl.DSRSensitivity));
     IniFile.WriteString('Serial', 'FlowControl.OutCTSFlow', BoolToStr(FlowControl.OutCTSFlow));
     IniFile.WriteString('Serial', 'FlowControl.OutDSRFlow', BoolToStr(FlowControl.OutDSRFlow));
     IniFile.WriteString('Serial', 'FlowControl.TxContinueOnXoff', BoolToStr(FlowControl.TxContinueOnXoff));
     IniFile.WriteString('Serial', 'FlowControl.XonXoffIn', BoolToStr(FlowControl.XonXoffIn));
     IniFile.WriteString('Serial', 'FlowControl.XonXoffOut', BoolToStr(FlowControl.XonXoffOut));
     IniFile.WriteString('Serial', 'FlowControl.XoffChar', FlowControl.XoffChar);
     IniFile.WriteString('Serial', 'FlowControl.XonChar', FlowControl.XonChar);
   end;

 IniFile.WriteBool('Misc', 'MaxChartPoints', CheckBox2.Checked);
 IniFile.WriteInteger('Misc', 'MaxChartPointsValue', SpinEdit1.Value);
 IniFile.WriteBool('Misc', 'AutoscrollLog', CheckBox1.Checked);

 IniFile.EraseSection('Chanals');
 for i := 0 to Memo1.Lines.Count do
   IniFile.WriteString('Chanals', 'S'+IntToStr(i), StringReplace(Memo1.Lines[i], '=', '~', [rfReplaceAll]));

 IniFile.WriteString('Signal', 'RegExpr', Memo2.Lines[0]);
 IniFile.WriteInteger('Form', 'ChanalsEditWidth', Panel3.Width);
 IniFile.WriteInteger('Form', 'ChartWidth', Chart1.Width);

 IniFile.UpdateFile;
 IniFile.Free;
end;

procedure TFormMain.LoadSettings(FileName: string);
var
  IniFile : TIniFile;
  i       : Integer;
  l       : string;
begin
 LoadSettingsFlag := True;
 IniFile := TIniFile.Create(ExtractFilePath(Application.ExeName)+FileName);

  with ComPort do
   begin
     Port := IniFile.ReadString('Serial', 'Port', Port);
     ComboBox1.Text := Port;
     BaudRate := StrToBaudRate(IniFile.ReadString('Serial', 'BaudRate', BaudRateToStr(BaudRate)));
     if BaudRate = brCustom then
       CustomBaudRate := IniFile.ReadInteger('Serial', 'CustomBaudRate', 9600);
     ComboBox2.Text := IntToStr(CustomBaudRate);
     StopBits := StrToStopBits(IniFile.ReadString('Serial', 'StopBits', StopBitsToStr(StopBits)));
     DataBits := StrToDataBits(IniFile.ReadString('Serial', 'DataBits', DataBitsToStr(DataBits)));
     Parity.Bits := StrToParity(IniFile.ReadString('Serial', 'Parity', ParityToStr(Parity.Bits)));
     FlowControl.FlowControl := StrToFlowControl(
       IniFile.ReadString('Serial', 'FlowControl', FlowControlToStr(FlowControl.FlowControl)));

     EventChar := StrToChar(IniFile.ReadString('Serial', 'EventChar', CharToStr(EventChar)));
     DiscardNull := StrToBool(IniFile.ReadString('Serial', 'DiscardNull', BoolToStr(DiscardNull)));

     Parity.Check := StrToBool(IniFile.ReadString('Serial', 'Parity.Check', BoolToStr(Parity.Check)));
     Parity.Replace := StrToBool(IniFile.ReadString('Serial', 'Parity.Replace', BoolToStr(Parity.Replace)));
     Parity.ReplaceChar := StrToChar(IniFile.ReadString('Serial', 'Parity.ReplaceChar', CharToStr(Parity.ReplaceChar)));

     Buffer.OutputSize := IniFile.ReadInteger('Serial', 'Buffer.OutputSize', Buffer.OutputSize);
     Buffer.InputSize := IniFile.ReadInteger('Serial', 'Buffer.InputSize', Buffer.InputSize);

     Timeouts.ReadInterval := IniFile.ReadInteger('Serial', 'Timeouts.ReadInterval', Timeouts.ReadInterval);
     Timeouts.ReadTotalConstant := IniFile.ReadInteger('Serial', 'Timeouts.ReadTotalConstant', Timeouts.ReadTotalConstant);
     Timeouts.ReadTotalMultiplier := IniFile.ReadInteger('Serial', 'Timeouts.ReadTotalMultiplier', Timeouts.ReadTotalMultiplier);
     Timeouts.WriteTotalConstant := IniFile.ReadInteger('Serial', 'Timeouts.WriteTotalConstant', Timeouts.WriteTotalConstant);
     Timeouts.WriteTotalMultiplier := IniFile.ReadInteger('Serial', 'Timeouts.WriteTotalMultiplier', Timeouts.WriteTotalMultiplier);

     FlowControl.ControlRTS := StrToRTS(IniFile.ReadString('Serial', 'FlowControl.ControlRTS', RTSToStr(FlowControl.ControlRTS)));
     FlowControl.ControlDTR := StrToDTR(IniFile.ReadString('Serial', 'FlowControl.ControlDTR', DTRToStr(FlowControl.ControlDTR)));
     FlowControl.DSRSensitivity := StrToBool(IniFile.ReadString('Serial', 'FlowControl.DSRSensitivity', BoolToStr(FlowControl.DSRSensitivity)));
     FlowControl.OutCTSFlow := StrToBool(IniFile.ReadString('Serial', 'FlowControl.OutCTSFlow', BoolToStr(FlowControl.OutCTSFlow)));
     FlowControl.OutDSRFlow := StrToBool(IniFile.ReadString('Serial', 'FlowControl.OutDSRFlow', BoolToStr(FlowControl.OutCTSFlow)));
     FlowControl.TxContinueOnXoff := StrToBool(IniFile.ReadString('Serial', 'FlowControl.TxContinueOnXoff', BoolToStr(FlowControl.TxContinueOnXoff)));
     FlowControl.XonXoffIn := StrToBool(IniFile.ReadString('Serial', 'FlowControl.XonXoffIn', BoolToStr(FlowControl.XonXoffIn)));
     FlowControl.XonXoffOut := StrToBool(IniFile.ReadString('Serial', 'FlowControl.XonXoffOut', BoolToStr(FlowControl.XonXoffOut)));
     FlowControl.XoffChar := StrToChar(IniFile.ReadString('Serial', 'FlowControl.XoffChar', CharToStr(FlowControl.XoffChar)));
     FlowControl.XonChar := StrToChar(IniFile.ReadString('Serial', 'FlowControl.XonChar', CharToStr(FlowControl.XonChar)));
   end;

 CheckBox2.Checked := IniFile.ReadBool('Misc', 'MaxChartPoints', True);
 SpinEdit1.Value := IniFile.ReadInteger('Misc', 'MaxChartPointsValue', 300);
 CheckBox1.Checked := IniFile.ReadBool('Misc', 'AutoscrollLog', True);

 Memo1.Lines.Clear;
 for i := 0 to 20 do
   begin
     l := Trim(StringReplace(IniFile.ReadString('Chanals', 'S'+IntToStr(i), ''), '~','=',[rfReplaceAll]));
     if l <> '' then
       Memo1.Lines.Add(l)
     else
       Break;
   end;

 Memo2.Text := IniFile.ReadString('Signal', 'RegExpr', '');
 Panel3.Width := IniFile.ReadInteger('Form', 'ChanalsEditWidth', Panel3.Width);
 Chart1.Width := IniFile.ReadInteger('Form', 'ChartWidth', Chart1.Width);
 IniFile.Free;
 LoadSettingsFlag := False;
end;

procedure TFormMain.ScanPesets;
var
  F       : TSearchRec;
  IniFile : TIniFile;
  FName   : string;
begin
 ComboBox3.Items.Clear;
 if FindFirst(ExtractFilePath(Application.ExeName) + '*.ini', faAnyFile, F) = 0 then
   begin
     repeat
       Application.ProcessMessages;
       if (F.Attr and faDirectory) <> faDirectory then
         ComboBox3.Items.Add(ChangeFileExt(ExtractFileName(F.Name),''));
     until FindNext(F) <> 0;
     FindClose(F);
     IniFile := TIniFile.Create(ExtractFilePath(Application.ExeName)+'sgraph.set');
     FName := IniFile.ReadString('Preset', 'Name', '');
     IniFile.Free;
     if FileExists(ExtractFilePath(Application.ExeName) + FName + '.ini') then
       LoadSettings(FName + '.ini');
     ComboBox3.Text := FName;
   end;
end;

function CorrectFileName(FileName: string): string;
const
  BadChars : set of Char = ['.', '<', '>', '|', '"', '\', '/', ':', '*', '?'];
var
  i : Integer;
begin
 for i := 1 to Length(FileName) do
   if not (Filename[i] in BadChars) then Result := Result + Filename[i];
end;

procedure TFormMain.UpdatePesets(Sender: TObject);
var
  IniFile : TIniFile;
  br      : Integer;
begin
 if TryStrToInt(ComboBox2.Text, br) then ComPort.CustomBaudRate := br;
 Chart1.MaxPointsPerPage := SpinEdit1.Value;
 SaveSettings(ComboBox3.Text+'.ini');
 IniFile := TIniFile.Create(ExtractFilePath(Application.ExeName)+'sgraph.set');
 IniFile.WriteString('Preset', 'Name', ComboBox3.Text);
 IniFile.UpdateFile;
 IniFile.Free;
end;

//*****

procedure TFormMain.FormCreate(Sender: TObject);
begin
 r := TRegExpr.Create;
 calc := TCalcExpress.Create(Self);
 ScanPesets();
end;

procedure TFormMain.ComboBox3Change(Sender: TObject);
var
  sp : Integer;
begin
 sp := ComboBox3.SelStart;
 ComboBox3.Text := CorrectFileName(ComboBox3.Text);
 if sp <= Length(ComboBox3.Text) then ComboBox3.SelStart := sp
   else ComboBox3.SelStart := Length(ComboBox3.Text);
end;

procedure TFormMain.Button1Click(Sender: TObject);
var
  i : Integer;
  s : TLineSeries;
begin
 try
 if ComPort.Connected then
   begin
     ComPort.Connected := False;
     Button1.Caption := 'Start';
     ComboBox1.Enabled := True;
     ComboBox2.Enabled := True;
     Memo1.Enabled := True;
     Memo2.Enabled := True;
   end else begin
     ComPort.Port := ComboBox1.Text;
     ComPort.CustomBaudRate := StrToInt(Trim(ComboBox2.Text));
     ComPort.Connected := True;
       begin
         Button1.Caption := 'Stop';
         ComboBox1.Enabled := False;
         ComboBox2.Enabled := False;
         Memo1.Enabled := False;
         Memo2.Enabled := False;
         Memo3.Clear;
         Chart1.SeriesList.Clear;
         r.Expression := Memo2.Lines[0];
         for i := 0 to Memo1.Lines.Count-1 do
           if Trim(Memo1.Lines[i]) <> '' then
             begin
               s := TLineSeries.Create(Chart1);
               s.Title := Trim(Copy(Memo1.Lines[i], Pos('=', Memo1.Lines[i]) + 1 , 55));
               Chart1.AddSeries(s);
             end;
       end;
   end;
 except
   MessageDlg('Error :)', mtError, [mbOK], -1);
 end;  
end;

procedure TFormMain.ProcessLine(Line: string);
var
  sn, vn : string;
  sp     : Integer;
  i, j   : Integer;
  res    : Double;
begin
 Memo3.Lines.Add(Line);
 if r.Exec(Line) then
   begin
     for i := 0 to Memo1.Lines.Count-1 do
       if Trim(Memo1.Lines[i]) <> '' then
         begin
           sn := Memo1.Lines[i];
           sp := Pos('=', Memo1.Lines[i]);
           vn := Trim(Copy(sn, 1, sp-1));
           for j := 1 to 9 do
             vn := StringReplace(vn, '$' + IntToStr(j), r.Match[j], [rfReplaceAll]);
           calc.Formula := vn;
           try
             res := calc.calc([]);
           except
             res := 0;
           end;
           if CheckBox2.Checked then
             if (Chart1.Series[i].Count > 0) and
               (Chart1.Series[i].XValue[Chart1.Series[i].Count-1] >= SpinEdit1.Value)
                 then Chart1.Series[i].Delete(0);
           Chart1.Series[i].AddXY(Memo3.Lines.Count, res);
           //Chart1.Series[i].Add(res);
         end;
   end;
end;


procedure TFormMain.Button2Click(Sender: TObject);
var
  i : Integer;
begin
 for i := 0 to Chart1.SeriesList.Count-1 do
   Chart1.Series[i].Clear;
 Memo3.Lines.Clear;  
end;
       
procedure TFormMain.Button3Click(Sender: TObject);
var
  b: TBitmap;
  r: TRect;
begin
 b := TBitmap.Create;
 r := Chart1.BoundsRect;
 r.Right := r.Right - r.Left;
 r.Left := 0;
 r.Bottom := r.Bottom - r.Top;
 r.Top := 0;
 b.Width := r.Right;
 b.Height := r.Bottom;
 b.PixelFormat := pf24bit;
 Chart1.Draw(b.Canvas,r);
 Clipboard.Assign(b);
 b.Free;
end;

procedure TFormMain.Button6Click(Sender: TObject);
var
  b: TBitmap;
  r: TRect;
begin
 b := TBitmap.Create;
 r := FormMain.BoundsRect;
 r.Right := r.Right - r.Left;
 r.Left := 0;
 r.Bottom := r.Bottom - r.Top;
 r.Top := 0;
 b.Width := r.Right;
 b.Height := r.Bottom;
 b.PixelFormat := pf24bit;
 b.Canvas.CopyRect(r, Canvas, r);
 Clipboard.Assign(b);
 b.Free;
end;

procedure TFormMain.Button4Click(Sender: TObject);
begin
 Clipboard.AsText := Memo3.Lines.Text;
end;

procedure TFormMain.ComPortRxChar(Sender: TObject; Count: Integer);
var
  p       : Integer;
  line    : string;
begin
 try
 ComPort.ReadStr(line, Count);
 buff := buff + line;
 p := Pos(#13, buff);
 while p > 0 do
   begin
     line := Trim(Copy(buff, 1, p-1));
     if Length(line) > 0 then ProcessLine(Line);
     Delete(buff, 1, p);
     buff := Trim(buff);
     p := Pos(#13, buff);
   end;
 except
   //!!!
 end;
end;

procedure TFormMain.Button5Click(Sender: TObject);
begin
 ComPort.Port := ComboBox1.Text;
 ComPort.CustomBaudRate := StrToInt(ComboBox2.Text);
 ComPort.ShowSetupDialog;
 UpdatePesets(nil);
end;

procedure TFormMain.ComboBox3Select(Sender: TObject);
begin
 LoadSettings(ComboBox3.Text+'.ini');
end;



end.
