//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TFormNewGraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormNewGraph *FormNewGraph;
//---------------------------------------------------------------------------
__fastcall TFormNewGraph::TFormNewGraph(TComponent* Owner)
	: TForm(Owner)
{
	DateStr = "";
	ModalResult = false;
	N = 0;
	A = 0;
	VertexStr = vstr_t();
}
//---------------------------------------------------------------------------


void __fastcall TFormNewGraph::FormShow(TObject *Sender)
{
	ModalResult = false;

	N = 0;
	A = 0;
	VertexStr = vstr_t();

	TDateTime Date = TDateTime::CurrentDateTime();
	DateStr = Date.DateTimeString();
	DateStr = ReplaceStr(DateStr,":","_");
	DateStr = ReplaceStr(DateStr,".","");
	DateStr = ReplaceStr(DateStr," ","_");

	EditName->Text = "Graph_" + DateStr;
}
//---------------------------------------------------------------------------


void __fastcall TFormNewGraph::ButtonCreateClick(TObject *Sender)
{
	ModalResult = true;

	if (RadioButtonHand->Checked) {

		for (int i = 0; i < RichEditVertex->Lines->Count; ++i)
		    VertexStr.push_back(RichEditVertex->Lines->Strings[i]);

	} else {

		N = TextToInt(EditN->Text);
	    A = TextToInt(EditA->Text);
	}

	Close();
}
//---------------------------------------------------------------------------


void __fastcall TFormNewGraph::ButtonCancelClick(TObject *Sender)
{
	ModalResult = false;
	Close();
}
//---------------------------------------------------------------------------


void __fastcall TFormNewGraph::RadioButtonRandClick(TObject *Sender)
{
	if (RadioButtonRand->Checked) {

		EditN->Enabled = true;
		EditA->Enabled = true;

		RichEditVertex->Enabled = false;
	}
}
//---------------------------------------------------------------------------


void __fastcall TFormNewGraph::RadioButtonHandClick(TObject *Sender)
{
	if (RadioButtonHand->Checked) {

		EditN->Enabled = false;
		EditA->Enabled = false;

		RichEditVertex->Enabled = true;
	}
}
//---------------------------------------------------------------------------


int __fastcall TFormNewGraph::TextToInt(AnsiString Str)
{
	AnsiString Numbers = "0123456789";

	AnsiString Value;

	int i = 1;

	while ((i <= Str.Length()) && (Numbers.Pos(Str[i]) != 0)) {

		Value += Str[i];
		++i;
	}

	if (Value == "")
		return 0;
	else
	    return StrToInt(Value);
}
//------------------------------------------------------------------------------

