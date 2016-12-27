program SGraph;

uses
  Forms,
  UnitMain in 'UnitMain.pas' {FormMain},
  UnitPanel in 'UnitPanel.pas' {Form2};

{$R *.res}

begin
  Application.Initialize;
  Application.CreateForm(TFormMain, FormMain);
  Application.CreateForm(TForm2, Form2);
  Application.Run;
end.
