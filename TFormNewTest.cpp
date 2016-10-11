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
	ListViewAlg->Items->Item[0]->Indent = FormMain->ListViewAlg->Items->Item[0]->Indent;
	ListViewAlg->Items->Item[1]->Indent = FormMain->ListViewAlg->Items->Item[1]->Indent;
	ListViewAlg->Items->Item[2]->Indent = FormMain->ListViewAlg->Items->Item[2]->Indent;
	ListViewAlg->Items->Item[3]->Indent = FormMain->ListViewAlg->Items->Item[3]->Indent;
	ListViewAlg->Items->Item[4]->Indent = FormMain->ListViewAlg->Items->Item[4]->Indent;
	ListViewAlg->Items->Item[5]->Indent = FormMain->ListViewAlg->Items->Item[5]->Indent;
	ListViewAlg->Items->Item[6]->Indent = FormMain->ListViewAlg->Items->Item[6]->Indent;
	ListViewAlg->Items->Item[7]->Indent = FormMain->ListViewAlg->Items->Item[7]->Indent;
	ListViewAlg->Items->Item[8]->Indent = FormMain->ListViewAlg->Items->Item[8]->Indent;

	ListViewAlg->Items->Item[0]->Caption = FormMain->ListViewAlg->Items->Item[0]->Caption;
	ListViewAlg->Items->Item[1]->Caption = FormMain->ListViewAlg->Items->Item[1]->Caption;
	ListViewAlg->Items->Item[2]->Caption = FormMain->ListViewAlg->Items->Item[2]->Caption;
	ListViewAlg->Items->Item[3]->Caption = FormMain->ListViewAlg->Items->Item[3]->Caption;
	ListViewAlg->Items->Item[4]->Caption = FormMain->ListViewAlg->Items->Item[4]->Caption;
	ListViewAlg->Items->Item[5]->Caption = FormMain->ListViewAlg->Items->Item[5]->Caption;
	ListViewAlg->Items->Item[6]->Caption = FormMain->ListViewAlg->Items->Item[6]->Caption;
	ListViewAlg->Items->Item[7]->Caption = FormMain->ListViewAlg->Items->Item[7]->Caption;
	ListViewAlg->Items->Item[8]->Caption = FormMain->ListViewAlg->Items->Item[8]->Caption;

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
