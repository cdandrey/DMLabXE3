//---------------------------------------------------------------------------

#ifndef TFormNewTestH
#define TFormNewTestH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
//---------------------------------------------------------------------------


#include "Typedef.h"
//---------------------------------------------------------------------------


class TFormNewTest : public TForm
{
__published:	// IDE-managed Components
	TLabeledEdit *EditRangeN;
	TLabeledEdit *EditRangeA;
	TLabeledEdit *EditRangeT;
	TLabeledEdit *EditName;
	TLabeledEdit *EditCount;
	TListView *ListViewAlg;
	TButton *ButtonCreate;
	TButton *ButtonCancel;
	void __fastcall FormShow(TObject *Sender);
	void __fastcall ButtonCancelClick(TObject *Sender);
	void __fastcall ButtonCreateClick(TObject *Sender);
private:	// User declarations

	AnsiString DateStr;

public:		// User declarations
	__fastcall TFormNewTest(TComponent* Owner);

	bool ModalResult;

    test_t Test;
};
//---------------------------------------------------------------------------
extern PACKAGE TFormNewTest *FormNewTest;
//---------------------------------------------------------------------------
#endif
