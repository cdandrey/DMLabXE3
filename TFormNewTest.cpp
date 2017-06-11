//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "TFormNewTest.h"
#include "TFormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TFormNewTest *FormNewTest;
//---------------------------------------------------------------------------


__fastcall TFormNewTest::TFormNewTest(TComponent* Owner)
	: TForm(Owner)
{
	for (int i = 0; i < FormMain->ListViewAlgMIS->Items->Count; ++i) {
		ListViewAlg->Items->Item[i]->Indent = FormMain->ListViewAlgMIS->Items->Item[i]->Indent;
		ListViewAlg->Items->Item[i]->Caption = FormMain->ListViewAlgMIS->Items->Item[i]->Caption;

	}

	ModalResult = false;
}
//---------------------------------------------------------------------------
void __fastcall TFormNewTest::FormShow(TObject *Sender)
{
	ModalResult = false;

	TDateTime Date = TDateTime::CurrentDateTime();
	DateStr = Date.DateTimeString();
	DateStr = ReplaceStr(DateStr,":","_");
	DateStr = ReplaceStr(DateStr,".","");
	DateStr = ReplaceStr(DateStr," ","_");

	EditName->Text = "Test_" + DateStr;
}
//---------------------------------------------------------------------------
void __fastcall TFormNewTest::ButtonCancelClick(TObject *Sender)
{
	ModalResult = false;
	Close();
}
//---------------------------------------------------------------------------
void __fastcall TFormNewTest::ButtonCreateClick(TObject *Sender)
{
    try {

		ModalResult = true;

	} catch(...) {
		ModalResult = false;
	}
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
