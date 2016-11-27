//---------------------------------------------------------------------------

#ifndef TFormNewGraphH
#define TFormNewGraphH
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.ComCtrls.hpp>
//---------------------------------------------------------------------------

#include "Typedef.h"
//---------------------------------------------------------------------------


class TFormNewGraph : public TForm
{
__published:	// IDE-managed Components
	TLabeledEdit *EditName;
	TLabeledEdit *EditN;
	TLabeledEdit *EditA;
	TButton *ButtonCancel;
	TButton *ButtonCreate;
	TRichEdit *RichEditVertex;
	TMemo *MemoExampl;
	TRadioButton *RadioButtonRand;
	TRadioButton *RadioButtonHand;
	void __fastcall ButtonCreateClick(TObject *Sender);
	void __fastcall ButtonCancelClick(TObject *Sender);
	void __fastcall RadioButtonRandClick(TObject *Sender);
	void __fastcall RadioButtonHandClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);

private:	// User declarations

	AnsiString DateStr;

	int  __fastcall TextToInt(AnsiString Str);

public:		// User declarations
	__fastcall TFormNewGraph(TComponent* Owner);

	bool ModalResult;

	int N;    // количество вершин в графе
	int A;    // плотность графа

	vstr_t VertexStr;    // список вершин в строковом формате
};
//---------------------------------------------------------------------------
extern PACKAGE TFormNewGraph *FormNewGraph;
//---------------------------------------------------------------------------
#endif
