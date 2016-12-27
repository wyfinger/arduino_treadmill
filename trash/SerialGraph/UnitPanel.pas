unit UnitPanel;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, UnitMain;

type
  TForm2 = class(TForm)
    ScrollBar6: TScrollBar;
    Label6a: TLabel;
    Label6: TLabel;
    ScrollBar7: TScrollBar;
    Label7a: TLabel;
    Label7: TLabel;
    ScrollBar8: TScrollBar;
    Label8a: TLabel;
    Label8: TLabel;
    Button1: TButton;
    Label2b: TLabel;
    Label1a: TLabel;
    Label1: TLabel;
    ScrollBar1: TScrollBar;
    Label1t: TLabel;
    Label2a: TLabel;
    Label2: TLabel;
    ScrollBar2: TScrollBar;
    Label3a: TLabel;
    Label3: TLabel;
    ScrollBar3: TScrollBar;
    Label4a: TLabel;
    Label4: TLabel;
    ScrollBar4: TScrollBar;
    Label5a: TLabel;
    Label5: TLabel;
    ScrollBar5: TScrollBar;
    Label2t: TLabel;
    Label3t: TLabel;
    Label1b: TLabel;
    Label9a: TLabel;
    Label9: TLabel;
    ScrollBar9: TScrollBar;
    procedure Button1Click(Sender: TObject);
    procedure UpdatePanel(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form2: TForm2;

implementation

{$R *.dfm}

procedure TForm2.Button1Click(Sender: TObject);
begin
 if FormMain.ComPort.Connected then
   begin
     FormMain.ComPort.WriteStr(Label2b.Caption+#13#10);
   end;
end;

procedure TForm2.UpdatePanel(Sender: TObject);
begin
 Label1.Caption := Format('0%.1f', [ScrollBar1.Position / 10]);
 Label1.Caption := Copy(Label1.Caption, Length(Label1.Caption)-3, 4);
 Label2.Caption := Format('0%.1f', [ScrollBar2.Position / 10]);
 Label2.Caption := Copy(Label2.Caption, Length(Label2.Caption)-3, 4);

 Label3.Caption := Format('%0.3d', [ScrollBar3.Position]);
 Label4.Caption := Format('%0.3d', [ScrollBar4.Position]);

 Label5.Caption := Format('%0.4d', [ScrollBar5.Position]);

 Label6.Caption := Format('%0.2f', [ScrollBar6.Position/100]);
 Label7.Caption := Format('%0.2f', [ScrollBar7.Position/100]);
 Label8.Caption := Format('%0.2f', [ScrollBar8.Position/100]);

 Label9.Caption := Format('%0.1f', [ScrollBar9.Position/10]);

 Label2b.Caption := Label1.Caption + '|' + Label2.Caption + '|' + Label3.Caption +
   '|' +Label4.Caption + '|' + Label5.Caption + '|' + Label6.Caption + '|' +
   Label7.Caption + '|' + Label8.Caption + '|' + Label9.Caption;
end;

end.
