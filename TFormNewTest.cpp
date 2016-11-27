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
	for (int i = 0; i < FormMain->ListViewAlg->Items->Count; ++i) {
		ListViewAlg->Items->Item[i]->Indent = FormMain->ListViewAlg->Items->Item[i]->Indent;
		ListViewAlg->Items->Item[i]->Caption = FormMain->ListViewAlg->Items->Item[i]->Caption;

	}
//	ListViewAlg->Items->Item[1]->Indent = FormMain->ListViewAlg->Items->Item[1]->Indent;
//	ListViewAlg->Items->Item[2]->Indent = FormMain->ListViewAlg->Items->Item[2]->Indent;
//	ListViewAlg->Items->Item[3]->Indent = FormMain->ListViewAlg->Items->Item[3]->Indent;
//	ListViewAlg->Items->Item[4]->Indent = FormMain->ListViewAlg->Items->Item[4]->Indent;
//	ListViewAlg->Items->Item[5]->Indent = FormMain->ListViewAlg->Items->Item[5]->Indent;
//	ListViewAlg->Items->Item[6]->Indent = FormMain->ListViewAlg->Items->Item[6]->Indent;
//	ListViewAlg->Items->Item[7]->Indent = FormMain->ListViewAlg->Items->Item[7]->Indent;
//
//	ListViewAlg->Items->Item[1]->Caption = FormMain->ListViewAlg->Items->Item[1]->Caption;
//	ListViewAlg->Items->Item[2]->Caption = FormMain->ListViewAlg->Items->Item[2]->Caption;
//	ListViewAlg->Items->Item[3]->Caption = FormMain->ListViewAlg->Items->Item[3]->Caption;
//	ListViewAlg->Items->Item[4]->Caption = FormMain->ListViewAlg->Items->Item[4]->Caption;
//	ListViewAlg->Items->Item[5]->Caption = FormMain->ListViewAlg->Items->Item[5]->Caption;
//	ListViewAlg->Items->Item[6]->Caption = FormMain->ListViewAlg->Items->Item[6]->Caption;
//	ListViewAlg->Items->Item[7]->Caption = FormMain->ListViewAlg->Items->Item[7]->Caption;

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
