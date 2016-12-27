unit SetupUnit;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, CPort;

type
  TForm3 = class(TForm)
    Label1: TLabel;
    ComPort: TComPort;
    ComboBox1: TComboBox;
    Label2: TLabel;
    CheckBox1: TCheckBox;
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form3: TForm3;

implementation

{$R *.dfm}

end.
