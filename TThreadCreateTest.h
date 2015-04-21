//---------------------------------------------------------------------------

#ifndef TThreadCreateTestH
#define TThreadCreateTestH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include "Typedef.h"
//---------------------------------------------------------------------------
class TThreadCreateTest : public TThread
{
private:

	inline void __fastcall Create();
	inline v_t  __fastcall LineToVec(const AnsiString &Line);
	inline void __fastcall Lock();
	inline  int __fastcall TextToInt(const AnsiString &Str);
	inline void __fastcall ToConsol(const AnsiString &Message);
	inline void __fastcall UnLock();

protected:
	void __fastcall Execute();
public:
	__fastcall TThreadCreateTest(bool CreateSuspended);

	AnsiString FileName;

	AnsiString RangeN;
	AnsiString RangeA;
	AnsiString RangeT;

	AnsiString CountStr;

	v_t Alg;
};
//---------------------------------------------------------------------------
#endif
